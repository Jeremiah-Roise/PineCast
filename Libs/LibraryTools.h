#include<string>
#include<fstream>
#include<iostream>
#include"PodcastMetaDataLists.h"
#include"DataTools.h"
#include"filepaths.h"
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
  file.open(filepaths::lclFiles() + "/MyPodcasts.xml",std::ios::app);
  if(file.fail()){cout << "file write failed" << endl;}
  file.write(XML.data(),XML.size());
  cout << "wrote to library" << endl;
  file.close();

  string newPodDir = filepaths::lclFiles().c_str();
  newPodDir += DataTools::cleanString(currentPodcast.title);
  mkdir(newPodDir.c_str(),ACCESSPERMS);
}

void removeFromLibrary(PodcastMetaData currentPodcast){
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
void loadLib(PodcastMetaDataList &list)
{
  cout << "start loading library" << endl;
  // open file To read
  string fileData = DataTools::getFile(filepaths::lclFiles()+"/MyPodcasts.xml");
  int index = 0;
  for (size_t i = 0; i < fileData.length(); i++)
  {
    string Podcast = DataTools::GetFieldAndReturnIndex(fileData, "<Podcast>", "</Podcast>", index, index);
    if (Podcast == "")
    {
      break;
    }
    list.createAndAddPodcast(DataTools::GetField(Podcast, "<Artist=\"", "\">"),
                             DataTools::GetField(Podcast, "<RssFeed=\"", "\">"),
                             DataTools::GetField(Podcast, "<Title=\"", "\">"),
                             DataTools::GetField(Podcast, "<Image30=\"", "\">"),
                             DataTools::GetField(Podcast, "<Image60=\"", "\">"),
                             DataTools::GetField(Podcast, "<Image100=\"", "\">"),
                             DataTools::GetField(Podcast, "<Image600=\"", "\">"));
    struct stat tmp;
    string folderPath = filepaths::lclFiles();
    folderPath += DataTools::cleanString(list.GetPodcastAtIndex(i).title);
    cout << folderPath << endl;
    if (stat(folderPath.c_str(), &tmp) != 0 && S_ISDIR(tmp.st_mode) != 1)
    {
      mkdir(folderPath.c_str(), ACCESSPERMS);
    }
  }
  cout << "finished loading library" << endl;
}