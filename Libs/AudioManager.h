
#include<iostream>
#include<string>
#include<stdio.h>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<sstream>
#include<gtk/gtk.h>
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


class PlayPodcast
{

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
        this->atestfunc(check,Podcast);
      }
      else
      {
      lastUpdate = check;
      atestfunc(check,Podcast);
      }
      
      return 0;
    }
    return 0;
  }
public:

  /// Downloads a podcast and returns the progress through the double pointer
  void DownloadPodcast(PodcastDataBundle lclPodcast){
      cout << lclPodcast.Episode.mp3Link << endl;
    string filepath = DataTools::filePathFromEpisode(lclPodcast.Episode,lclPodcast.Podcast);
    cout << "downloading podcast" << endl;
    try
    {
      cout << "created file" << endl; cURLpp::Easy handle;
      handle.setOpt(cURLpp::options::NoProgress(false));
      handle.setOpt(cURLpp::options::ProgressFunction( [this](double A,double B,double C,double D){cout << "this is the callback function" << endl; return this->progressUpdate(A,B,C,D); }));


      handle.setOpt(cURLpp::Options::Url(lclPodcast.Episode.mp3Link));
      handle.setOpt(cURLpp::options::FollowLocation(true));
      FILE *fp;
      fp = fopen(filepath.c_str(),"wb");
      handle.setOpt(cURLpp::options::WriteFile(fp));
      handle.perform();
      fclose(fp);
    }
    catch(curlpp::RuntimeError & e)
    {
      std::cout << e.what() << std::endl;
      cout << "no connection" << endl;
    }
    catch(curlpp::LogicError & e)
    {
      std::cout << e.what() << std::endl;
      cout << "no connection" << endl;
    }
  }
  //  creates a function type to hold an event arg
  void atestfunc(double val,PodcastDataBundle val2){
    cout << val << " : " << val2.Episode.title << endl;
  }

  PlayPodcast(size_t tmpEventPoint,PodcastDataBundle podcastBundle) : Podcast(podcastBundle)
  {
    EventPoint = tmpEventPoint;
  }

  void StartDownload(){
    cout << Podcast.Episode.mp3Link << endl;
    std::thread downThread = std::thread([this](PodcastDataBundle A){this->DownloadPodcast(A);},Podcast);
    downThread.detach();
    return;
  }
};