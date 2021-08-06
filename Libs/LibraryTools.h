#include<string>
#include<fstream>
#include<iostream>
#include"PodcastMetaDataLists.h"
#include"DataTools.h"
using std::cout;
using std::endl;
using std::string;


//  this ONLY adds the entry to the library file 
void addToLibrary(PodcastMetaData currentPodcast){
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
  file.open("Podcasts/MyPodcasts.xml",std::ios::app);
  cout << "created file" << endl;
  file.write(XML.data(),XML.size());
  cout << "wrote to file" << endl;
  file.close();
  //loadLib(&Library);
  //createSearchResults(LibraryUi,Library);
}

void removeFromLibrary(PodcastMetaData currentPodcast){
  string XML;
  XML = DataTools::getFile("Podcasts/MyPodcasts.xml");
  size_t index;
  size_t end;
  index = XML.find("<Title=\""+ currentPodcast.title +"\">");
  if (index == string::npos)
  {
    return;
  }
  index -= sizeof("<Podcast>");

  end = XML.find("</Podcast>");
    if (end == string::npos)
  {
    return;
  }
  end += sizeof("</Podcast>");
  XML.erase(index,end - index);

  fstream file;
  file.open("Podcasts/MyPodcasts.xml",fstream::out);
  file.write(XML.c_str(),XML.size());
  
  cout << "removed from lib" << endl;
}