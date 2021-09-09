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
#pragma once
using std::cout;
using std::endl;
using std::string;
size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
void getWebFile(string,string);
class webTools
{
  public:
  static string getFileInMem(string url){
    try
    {
      cURLpp::Easy handle;
      std::stringbuf str;
      std::ostream test(&str);
      handle.setOpt(cURLpp::options::NoProgress(true));
      handle.setOpt(cURLpp::Options::Url(url));
      handle.setOpt(cURLpp::options::FollowLocation(true));
      handle.setOpt(cURLpp::options::WriteStream(&test));
      handle.perform();
      string tmp = str.str();
      return tmp;
    }
    catch(curlpp::RuntimeError & e)
    {
      std::cout << e.what() << std::endl;
    }

    catch(curlpp::LogicError & e)
    {
      std::cout << e.what() << std::endl;
    }
    return "big problem";
  }

  /// Searches Itunes Database of podcasts and parses the results into: Podcast name; artist; feedurl; image url.
  static PodcastMetaDataList itunesSearch(string search = ""){
    if(search == "" || search.c_str() == NULL){PodcastMetaDataList test;return test;}// catch if search string is empty
    for (unsigned int i = 0; i < search.length(); i++)
    {
      if (search[i] == ' '){search[i] = '+';}
    }
    
    string itunesQuery = "https://itunes.apple.com/search?media=podcast&term=" + search;
    cout << itunesQuery <<"\n"<< endl;
    std::string data;
    data = webTools::getFileInMem(itunesQuery);
    // how to parse Json
    int items = std::stoi(DataTools::GetField(data,"\"resultCount\":",","));
    data = DataTools::GetField(data,"\"results\":","");
    PodcastMetaDataList tmp;
    int tmp0 = 0;
    int tmp1 = 0;
    int tmp2 = 0;
    int tmp3 = 0;
    for (int i = 0; i != items; i++)
    {
      // get image url's
      string image30 = DataTools::GetFieldP(data,"\"artworkUrl30\":\"","\"",tmp0,tmp0);
      string image60 = DataTools::GetFieldP(data,"\"artworkUrl60\":\"","\"",tmp0,tmp0);
      string image100 = DataTools::GetFieldP(data,"\"artworkUrl100\":\"","\"",tmp0,tmp0);
      string image600 = DataTools::GetFieldP(data,"\"artworkUrl600\":\"","\"",tmp0,tmp0);

      // get feed url
      string feedUrl = DataTools::GetFieldP(data,"\"feedUrl\":\"","\"",tmp1,tmp1);

      // get collection Name
      string collectionName = DataTools::GetFieldP(data,"\"collectionName\":\"","\"",tmp2,tmp2);

      // get artist
      string artist = DataTools::GetFieldP(data,"\"artistName\":\"","\"",tmp3,tmp3);
      tmp.createAndAddPodcast(artist,feedUrl,collectionName,image30,image60,image100,image600);
    }
    return tmp;
  }
  /// create's an image from a url
  static GdkPixbuf* createImage(string imageUrl, int scaleX, int scaleY)
  {
    string imagedata = webTools::getFileInMem(imageUrl); //  getting image data from web

    GdkPixbufLoader *test = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_set_size(test, scaleX, scaleY);
    gdk_pixbuf_loader_write(test, (const guchar*)imagedata.c_str(), imagedata.size(), nullptr);

    GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(test);
    gdk_pixbuf_loader_close(test, nullptr);

    return pixbuf;
  }
  /// Downloads a file from the web and writes it to the disk
  static void getWebFile(string url,string filename){

    try
  {
    //CURL *webhandle;
    FILE *fp;
    fp = fopen(filename.c_str(),"wb");
    cURLpp::Easy handle;
    handle.setOpt(cURLpp::options::NoProgress(false));
    handle.setOpt(cURLpp::Options::Url(url));
    handle.setOpt(cURLpp::options::FollowLocation(true));
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
  /// Downloads a podcast and returns the progress through the double pointer
  static void DownloadPodcast(string url,string filepath,double* Pprogress){
    cout << "downloading podcast" << endl;
    cout << filepath << endl;
    try
  {

    
    FILE *fp;
    fp = fopen(filepath.c_str(),"wb");
    cout << "created file" << endl;
    cURLpp::Easy handle;
    handle.setOpt(cURLpp::options::NoProgress(false));
    handle.setOpt(cURLpp::options::ProgressFunction([=](double dltotal,   double dlnow,   double ultotal,   double ulnow){
      double check= dlnow/dltotal;
      if (check >= 0)
      {
        *Pprogress = check;
        return 0;
      }
      return 0;
      }));
      handle.setOpt(cURLpp::Options::Url(url));
      handle.setOpt(cURLpp::options::FollowLocation(true));
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