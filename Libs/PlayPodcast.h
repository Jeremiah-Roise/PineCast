
#include<iostream>
#include<string>
#include<stdio.h>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<sstream>
#include<cmath>
#include<gtk/gtk.h>
#include<functional>
#include<thread>
#include"filepaths.h"
#include"DataTools.h"
#include"PodcastMetaDataLists.h"
#include"PodcastDataBundle.h"
#include"AudioPlayer.h"
#pragma once
using std::cout;
using std::endl;
using namespace std;
using namespace std::placeholders;




class PlayPodcast
{
private:

  PodcastDataBundle Podcast;
  float lastUpdate = 0;
  PodcastDataBundle empty;
  bool finished = false;
  cURLpp::Easy handle;


public:
  void reset(){
	  lastUpdate = 0;
	  finished = false;
  }
  std::function<void(double)> updateFunc = nullptr;


  void StartDownload(){
    std::thread downThread = std::thread([this](PodcastDataBundle A){this->DownloadPodcast(A);},Podcast);
    downThread.detach();
    return;
  }
  int progressUpdate(double dltotal, double dlnow,double,double){
    double check = dlnow/dltotal;
    
    //  checks for validity
    //  impoves performance only when tangible progress has been made
    if (std::isnan(check) || (check < lastUpdate + 0.01 && check != 1)){
	    return 0;
    }

    if (check >= 1 && finished == false)
    {
      finished = true;
      check = 1;
      updateFunc(check);
      return 0;
    }
    if (finished == false)
    {
    lastUpdate = check;
    updateFunc(check);
    return 0;
    }
    if (finished == true)
    {
      return 0;
    }
    
    else
    {
      cout << "something is wrong here possibly a negative value" << endl;
      return 1;
    }
    
  }

  PlayPodcast(PodcastDataBundle podcastBundle) : Podcast(podcastBundle){}



  /// Downloads a podcast and returns the progress through the double pointer
  void DownloadPodcast(PodcastDataBundle lclPodcast){
      cout << lclPodcast.Episode.mp3Link << endl;
    string filepath = DataTools::filePathFromEpisode(lclPodcast.Episode,lclPodcast.Podcast);
    cout << "downloading podcast" << endl;
  
    try
    {
      cout << "created file" << endl;
      handle.setOpt(cURLpp::options::NoProgress(false));
      // this jank is to allow the download system to work inside a single file
      handle.setOpt(cURLpp::options::ProgressFunction(bind(&PlayPodcast::progressUpdate,this,_1,_2,_3,_4)));
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
