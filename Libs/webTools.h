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
  static PodcastDataList itunesSearch(string search = ""){
    if(search == "" || search.c_str() == NULL){PodcastDataList test;return test;}// catch if search string is empty
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
    PodcastDataList tmp;
    int tmp0 = 0;
    int tmp1 = 0;
    int tmp2 = 0;
    int tmp3 = 0;
    for (int i = 0; i != items; i++)
    {
      PodcastData tmpData;
      // get image url's
      tmpData.image30  = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl30\":\"","\"",tmp0,tmp0);
      tmpData.image60  = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl60\":\"","\"",tmp0,tmp0);
      tmpData.image100 = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl100\":\"","\"",tmp0,tmp0);
      tmpData.image600 = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl600\":\"","\"",tmp0,tmp0);

      // get feed url
      tmpData.RssFeed  = DataTools::GetFieldAndReturnIndex(data,"\"feedUrl\":\"","\"",tmp1,tmp1);

      // get collection Name
      tmpData.title    = DataTools::GetFieldAndReturnIndex(data,"\"collectionName\":\"","\"",tmp2,tmp2);

      // get artist
      tmpData.artist   = DataTools::GetFieldAndReturnIndex(data,"\"artistName\":\"","\"",tmp3,tmp3);
      
    tmp.push_back(tmpData);
    tmp.front().index = (tmp.size() - 1);
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
};

