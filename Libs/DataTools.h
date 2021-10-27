#include<string.h>
#include<fstream>
#include<iostream>
#include"podcastEpisodeTypes.h"
using namespace std;
#pragma once
// test rss file is pinetalk.rss

// For getting data fields in strings of data example: datastring = {myAge="16yrs"} getField(datastring,"myAge=\\"","\\"") returns string 16yrs

class DataTools
{
    public:
    /// just gets the string data from a file.
    static std::string getFile(string file){
        
        ifstream myfile(file,ios::binary);// declaring file reader object opening file to read
        std::string text( (std::istreambuf_iterator<char>(myfile)),(std::istreambuf_iterator<char>()));
        myfile.close();
        return text;
    }

    /// gets particular values from podcast xml files and returns the index it was found at from the index reference.
    static string GetFieldAndReturnIndex(string PodcastRSS,string startAtChars,string endAtChars,int &index,int startAtIndex = 0){
        size_t itemStart = 0;
        size_t itemEnd = 0;
        if(startAtChars == ""){itemStart = 0;}
        else{itemStart = PodcastRSS.find(startAtChars,startAtIndex);}
        if(itemStart == string::npos){return "";}
        itemStart += startAtChars.length();
        if(endAtChars == ""){itemEnd = PodcastRSS.length();}
        else{itemEnd = PodcastRSS.find(endAtChars,itemStart);}
        string textblock = PodcastRSS.substr(itemStart,itemEnd-itemStart);
        if(itemStart != string::npos){index = itemStart;}
        return textblock;
    }

    /// gets particular values from podcast xml files.
    static string GetField(string PodcastRSS,string startAtChars,string endAtChars,int startAtIndex = 0){
        size_t itemStart = 0;
        size_t itemEnd = 0;
        if(startAtChars == ""){itemStart = 0;}
        else{itemStart = PodcastRSS.find(startAtChars,startAtIndex);}
        if(itemStart == string::npos){return "";}
        itemStart += startAtChars.length();
        if(endAtChars == ""){itemEnd = PodcastRSS.length();}
        else{itemEnd = PodcastRSS.find(endAtChars,itemStart);}
        string textblock = PodcastRSS.substr(itemStart,itemEnd-itemStart);
        return textblock;
    }

    /// extracts data from save file data.
    static PodcastEpisode GetPodcastData(const string& aRSSFile){

        PodcastEpisode MostRecent;
        // get title
        MostRecent.title = GetField(aRSSFile,"<title>","</title>");

        // get author
        MostRecent.artist = GetField(aRSSFile,"<itunes:author>","</itunes:author>");

        // get summary
        MostRecent.summary = GetField(aRSSFile,"<itunes:summary>","</itunes:summary>");
        // get episode
        //MostRecent.episode = stoi(RSSParser(RSSFile,"<itunes:episode>","</itunes:episode>"));
        
        // get duration
        MostRecent.duration = GetField(aRSSFile,"<itunes:duration>","</itunes:duration>");

        MostRecent.mp3Link = GetField(aRSSFile,"<enclosure url=\"","\"");

        MostRecent.thumbnailLink = GetField(aRSSFile,"<itunes:image href=\"","\"/>");

        return MostRecent;
    }
    /// gets all the episodes from a podcast xml file and returns them in an episode list.
    static PodcastEpisodeList getEpisodes(string& RSSFile){
    RSSFile = GetField(RSSFile,"<channel>","</channel>");
    // find episodes maybe use while loop
    PodcastEpisodeList episodes;
    int positionIndex=0;
    while(true)
    {
        string item = GetFieldAndReturnIndex(RSSFile,"<item>","</item>",positionIndex,positionIndex);//  find item starting from last item we indexed
        if(item == ""){break;}
        PodcastEpisode tmp = GetPodcastData(item);
        episodes.push_back(tmp);// add podcast to some kind of list
        episodes.back().index = (episodes.size() - 1);
    }
    return episodes;
}


    static bool hasSpaces(string& Value){
    for (char i:Value)
    if (isspace(i) != 0)
        {return true;}

    return false;
    }

    
    static string replaceSpaces(string& Value,char replaceWith){
        string cleanString;
        for (char i :Value)
        {
                if (i == ' ')
                cleanString += i;
                if (i != ' ')
                {
                    cleanString += replaceWith;
                }
        }
    return cleanString;
    }

    static string cleanString(string Value){
        string cleanString;
        for (char i :Value)
        {
            size_t code = int(i);
            bool isNumber = code > 48 && code < 58;
            bool ischaracter = (code > 64 && code < 91) || (code > 96 && code < 123);
            if (isNumber || ischaracter)
                cleanString += i;
        }
        return cleanString;
    }

    static PodcastDataList extractPodcastDataFromString(string& fileData){

      PodcastDataList list;
      int index = 0;
      for (size_t i = 0; i < fileData.length(); i++)
      {
        string Podcast = DataTools::GetFieldAndReturnIndex(fileData, "<Podcast>", "</Podcast>", index, index);
        if (Podcast == "")
          break;

        PodcastData tmpData;
        tmpData.artist = DataTools::GetField(Podcast, "<Artist=\"", "\">");
        tmpData.RssFeed = DataTools::GetField(Podcast, "<RssFeed=\"", "\">");
        tmpData.title = DataTools::GetField(Podcast, "<Title=\"", "\">");
        tmpData.image30 = DataTools::GetField(Podcast, "<Image30=\"", "\">");
        tmpData.image60 = DataTools::GetField(Podcast, "<Image60=\"", "\">");
        tmpData.image100 = DataTools::GetField(Podcast, "<Image100=\"", "\">");
        tmpData.image600 = DataTools::GetField(Podcast, "<Image600=\"", "\">"); 

        list.push_back(tmpData);
        list.back().index = (list.size() -1);// setting the index property of the element
      }
      return list;
    }
};



