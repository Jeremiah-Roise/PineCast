
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

  /// uses system command to start podcast with default application should be able to tolerate spaces.
  void playMp3(string path)
  {
    string FName = "xdg-open \"" + path + "\" &";
    cout << "the command is: " << FName << endl;
    system(FName.data());
  }

  //  plays localy downloaded podcasts. should be called when a podcast is already downloaded
   void play(PodcastDataBundle Podcast){
    string filepath = DataTools::filePathFromEpisode(Podcast.Episode,Podcast.Podcast);
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

  PodcastDataBundle Podcast;
  size_t EventPoint;
  float lastUpdate = 0;
  bool downloadFinished = false;
  PodcastDataBundle empty;

public:
  void StartDownload(){
    std::thread downThread = std::thread([this](PodcastDataBundle A){this->DownloadPodcast(A);},Podcast);
    downThread.detach();
    return;
  }

  PlayPodcast(PodcastDataBundle podcastBundle,size_t EventPoint) : Podcast(podcastBundle), EventPoint(EventPoint){}


  int progressUpdate(double dltotal,   double dlnow){
    double check = dlnow/dltotal;
    if (check >= 1 && downloadFinished == false)
    {
      downloadFinished = true;
      cout << check << endl;
      return 0;
    }
    if (lastUpdate + 0.01 < check)
    {
      lastUpdate = check;
      cout << check << endl;
      return 0;
    }
    return 0;
  }

  /// Downloads a podcast and returns the progress through the double pointer
  void DownloadPodcast(PodcastDataBundle lclPodcast){
      cout << lclPodcast.Episode.mp3Link << endl;
    string filepath = DataTools::filePathFromEpisode(lclPodcast.Episode,lclPodcast.Podcast);
    cout << "downloading podcast" << endl;
    try
    {
      cout << "created file" << endl;
      cURLpp::Easy handle;
      handle.setOpt(cURLpp::options::NoProgress(false));
      handle.setOpt(cURLpp::options::ProgressFunction([this](double A,double B,double C,double D){return this->progressUpdate(A,B);}));
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


};