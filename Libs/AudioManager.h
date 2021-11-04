
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
#pragma once
using std::cout;
using std::endl;
using std::string;

class PlayPodcast
{
private:
  size_t EventPoint;
  PodcastEpisode episode;
  PodcastData Podcast;
  float lastUpdate = 0;

  /// uses system command to start podcast with default application should be able to tolerate spaces.
  static void playMp3(string path)
  {
    string FName = "xdg-open \"" + path + "\" &";
    cout << "the command is: " << FName << endl;
    system(FName.data());
  }

  int progressUpdate(double dltotal,   double dlnow,   double ultotal,   double ulnow){
    double check = dlnow/dltotal;
    if (((check >= 0) && (check  >= lastUpdate + 0.01) && (check <= lastUpdate + 0.019)) || (check >= 1))
    {
      lastUpdate = check;
      cout << check << endl;
      //  ADD UPDATE FUNCTION
      return 0;
    }
    return 0;
  }

  /// Downloads a podcast and returns the progress through the double pointer
  void DownloadPodcast(){
    string filepath = DataTools::filePathFromEpisode(episode,Podcast);
    cout << "downloading podcast" << endl;
    try
    {
      cout << "created file" << endl; cURLpp::Easy handle;
      handle.setOpt(cURLpp::options::NoProgress(false));
      handle.setOpt(cURLpp::options::ProgressFunction(
        [this](double A,double B,double C,double D){
          return this->progressUpdate(A,B,C,D);
        }));
      handle.setOpt(cURLpp::Options::Url(episode.mp3Link));
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



public:
  PlayPodcast(size_t tmpEventPoint, const PodcastEpisode tmpEpisode, const PodcastData tmpPodcast){
    EventPoint = tmpEventPoint;
    Podcast = tmpPodcast;
    episode = tmpEpisode;
  }

  void StartDownload(){
    std::thread downThread = std::thread([this](){this->DownloadPodcast();});
    downThread.detach();
    return;
  }

  //  plays localy downloaded podcasts. should be called when a podcast is already downloaded
  static void play(PodcastEpisode episode, PodcastData Podcast){
    string filepath = DataTools::filePathFromEpisode(episode,Podcast);
    if (filepaths::fileExists(filepath))
    {
      playMp3(filepath);
      return;
    }
    return;
  }
};