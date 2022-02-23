
#include<iostream>
#include<string>
#include<stdio.h>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<sstream>
#include<gtk-3.0/gtk/gtk.h>
#include"DataTools.h"
#include"PodcastMetaDataLists.h"
#include"PodcastDataBundle.h"
#pragma once
using std::cout;
using std::endl;
using std::string;


  /// uses system command to start podcast with default application should be able to tolerate spaces.
  void playMp3(string path)
  {
    string FName = "xdg-open \"" + path + "\" &";
    cout << "the command is: " << FName << endl;
    system(FName.data());
  }

  //  plays localy downloaded podcasts. should be called when a podcast is already downloaded
   void play(PodcastEpisode episode, PodcastData Podcast){
    string filepath = DataTools::filePathFromEpisode(episode,Podcast);
    if (filepaths::fileExists(filepath))
    {
      playMp3(filepath);
      return;
    }
    return;
  }


class IPlayPodcast
{
  public:
  std::vector<std::unique_ptr<IPlayPodcast>> myList;
  virtual void StartDownload(){
    cout << "this is the base Class" << endl;
  }
  virtual void play(){
    cout << "this is the base Class" << endl;
  }
  virtual ~IPlayPodcast(){}

};

template<typename eventFuncArg>
class PlayPodcast:public IPlayPodcast
{
public:
  //  creates a function type to hold an event arg
  std::function<void(double,PodcastDataBundle,eventFuncArg)> updateEventFunc = [](double,PodcastDataBundle,eventFuncArg){return;};
  std::function<void(double,PodcastDataBundle,eventFuncArg)> eventPointFunc = [](double,PodcastDataBundle,eventFuncArg){return;};

  eventFuncArg eventArg;
  PlayPodcast(size_t tmpEventPoint,PodcastDataBundle podcastBundle,eventFuncArg eventArgtmp){
    EventPoint = tmpEventPoint;
    Podcast = podcastBundle;
    eventArg = eventArgtmp;
  }

  void StartDownload(){
    std::thread downThread = std::thread([this](){this->DownloadPodcast();});
    downThread.detach();
    return;
  }

private:
  size_t EventPoint;
  PodcastDataBundle Podcast;
  float lastUpdate = 0;
  bool eventPointRun = false;
  bool updateEventRun = false;
  bool isFinished = false;

  int progressUpdate(double dltotal,   double dlnow,   double ultotal,   double ulnow){
    double check = dlnow/dltotal;
    if (((check >= 0) && (check  >= lastUpdate + 0.01)) || ((check >= 1) && isFinished == false))
    {
      if (check >= EventPoint && eventPointRun == false)
      {
        eventPointRun = true;
        playMp3(DataTools::filePathFromEpisode(Podcast.Episode,Podcast.Podcast));
        this->eventPointFunc(check,Podcast,eventArg);
      }
      else
      {
      lastUpdate = check;
      updateEventFunc(check,Podcast,eventArg);
      }
      
      return 0;
    }
    return 0;
  }

  /// Downloads a podcast and returns the progress through the double pointer
  void DownloadPodcast(){
    string filepath = DataTools::filePathFromEpisode(Podcast.Episode,Podcast.Podcast);
    cout << "downloading podcast" << endl;
    try
    {
      cout << "created file" << endl; cURLpp::Easy handle;
      handle.setOpt(cURLpp::options::NoProgress(false));
      handle.setOpt(cURLpp::options::ProgressFunction(
        [this](double A,double B,double C,double D){
          return this->progressUpdate(A,B,C,D);
        }));
      handle.setOpt(cURLpp::Options::Url(Podcast.Episode.mp3Link));
      handle.setOpt(cURLpp::options::FollowLocation(true));
      FILE *fp;
      fp = fopen(filepath.c_str(),"wb");
      handle.setOpt(cURLpp::options::WriteFile(fp));
      handle.perform();
      fclose(fp);
      delete this;
    }
    catch(curlpp::RuntimeError & e)
    {
      std::cout << e.what() << std::endl;
      cout << "no connection" << endl;
      delete this;
    }
    catch(curlpp::LogicError & e)
    {
      std::cout << e.what() << std::endl;
      cout << "no connection" << endl;
      delete this;
    }
}
};