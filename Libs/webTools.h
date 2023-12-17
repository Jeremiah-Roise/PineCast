#include<iostream>
#include<string>
#include<stdio.h>
#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Options.hpp>
#include<curlpp/Infos.hpp>
#include<sstream>
#include <gtk-3.0/gtk/gtk.h>
#include"Libs.h"
#pragma once
using std::cout;
using std::endl;
using std::string;
size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
void getWebFile(string,string);
class webTools
{
  public:
    static bool internetAccess(){
      cURLpp::Easy handle;
      handle.setOpt(cURLpp::options::NoProgress(true));
      handle.setOpt(cURLpp::Options::Url("google.com"));
      handle.setOpt(cURLpp::options::FollowLocation(false));
      handle.setOpt(cURLpp::options::NoBody(true));
      handle.setOpt(curlpp::options::Header(false));
      handle.perform();
      size_t responseCode = curlpp::infos::ResponseCode::get(handle);
      cout << responseCode << endl;
     
      //  look I know this a bad test but I'm tired
      if (responseCode >= 200 && responseCode <= 399)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
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

  /// create's an image from a url
  static GdkPixbuf* createImage(string imageUrl, int scaleX, int scaleY)
  {
    cout << imageUrl << std::endl;
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

