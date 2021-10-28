#include<string>
#include<fstream>
#include<iostream>
#include"PodcastMetaDataLists.h"
#include"DataTools.h"
#include"filepaths.h"
using std::cout;
using std::endl;
using std::string;

class Library
{
public:
  //  this ONLY adds the entry to the library file 
  static void addToLibrary(PodcastData currentPodcast){
    cout << "adding to library" << endl;
    string XML;
    XML = "\n<Podcast>";
    XML += "\n<Title=\""+currentPodcast.title+"\">";
    XML += "\n<RssFeed=\""+currentPodcast.RssFeed+"\">";
    XML += "\n<Image600=\""+currentPodcast.image600+"\">";
    XML += "\n<Image100=\""+currentPodcast.image100+"\">";
    XML += "\n<Image60=\""+currentPodcast.image60+"\">";
    XML += "\n<Image30=\""+currentPodcast.image30+"\">";
    XML += "\n</Podcast>";

    ofstream file;
    file.open(filepaths::lclFiles() + "/MyPodcasts.xml",std::ios::app);
    if(file.fail()){cout << "file write failed" << endl;}
    file.write(XML.data(),XML.size());
    cout << "wrote to library" << endl;
    file.close();

    string newPodDir = filepaths::lclFiles().c_str();
    newPodDir += DataTools::cleanString(currentPodcast.title);
    mkdir(newPodDir.c_str(),ACCESSPERMS);
  }

  static void removeFromLibrary(PodcastData currentPodcast){
    string XML;
    XML = DataTools::getFile(filepaths::lclFiles() + "/MyPodcasts.xml");
    size_t index;
    size_t end;
    index = XML.find("<Title=\""+ currentPodcast.title +"\">");
    cout << "<Title=\""+ currentPodcast.title +"\">" << endl;
    if (index == string::npos)
    {
      cout << "failed" << endl;
      return;
    }
    index -= sizeof("<Podcast>");

    end = XML.find("</Podcast>",index);
      if (end == string::npos)
    {
      return;
    }
    end += sizeof("</Podcast>");
    XML.erase(index,end - index);

    fstream file;
    file.open(filepaths::lclFiles() + "/MyPodcasts.xml",fstream::out);
    file.write(XML.c_str(),XML.size());

    string PodDir = filepaths::lclFiles().c_str();
    PodDir += DataTools::cleanString(currentPodcast.title);
    rmdir(PodDir.c_str());
    cout << PodDir << endl;
    
    cout << "removed from lib" << endl;
  }
  
  ///  update podcasts in library.
  static void loadLib(PodcastDataList &list)
    
  {
    cout << "start loading library" << endl;


      string filePath = filepaths::lclFiles()+"/MyPodcasts.xml";
      cout << filePath << endl;
      if (filepaths::fileExists(filePath) != true)
      {
        cout << "MyPodcasts.xml does not exist" << endl;
        return;
      }
      string fileData = DataTools::getFile(filePath);
      
      list = DataTools::extractPodcastDataFromString(fileData);
    // open file To read
    cout << "finished loading library" << endl;
  }
};

class Downloads
{
public:
  static void addToDownloads(PodcastEpisodeList& episodes){
    for (PodcastEpisode i:episodes){
      addToDownloads(i);
    }
  }

  static void addToDownloads(PodcastEpisode& episode){
    
    string XML = "<Title=\""+episode.title+"\">\n";

    ofstream file;
    file.open(filepaths::lclFiles() + "Downloaded.xml",std::ios::app);
    if(file.fail()){cout << "file write failed" << endl;}
    file.write(XML.data(),XML.size());
    file.close();
  }

  static void removeFromDownloads(PodcastEpisode& episodeToRemove)
  {
    
    string lineToRemove = "<Title=\""+episodeToRemove.title+"\">\n";

    string filepath = filepaths::lclFiles() + "Downloaded.xml";
    string filedata = DataTools::getFile(filepath);

    filedata.replace(filedata.find(lineToRemove),lineToRemove.length(),"");

    ofstream file;
    file.open(filepath,std::ios::out);
    if(file.fail()){cout << "file write failed" << endl;}
    file.write(filedata.c_str(),filedata.size());
    file.close();
  }

  static bool isEpisodeDownloaded(PodcastEpisode compare){
    PodcastEpisodeList downloaded = getDownloads();
    for (PodcastEpisode i:downloaded)
    {
      if (i.title == compare.title)
      {
        return true;
      }
      
    }
    
   return false; 
  }

  static PodcastEpisodeList getDownloads(){
    // open file To read
    string fileData = DataTools::getFile(filepaths::lclFiles()+"/Downloaded.xml");
    int index = 0;
    PodcastEpisodeList downloaded;
    for (size_t i = 0; i < fileData.length(); i++)
    {
      string Podcast = DataTools::GetFieldAndReturnIndex(fileData, "<Title=\"", "\">", index, index);
      if (Podcast == "")
        break;
      
      PodcastEpisode tmp;
      tmp.title = Podcast;
      downloaded.push_back(tmp);
    }
    return downloaded;
  }
};





