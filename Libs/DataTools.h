#include<string.h>
#include<fstream>
#include<iostream>
#include"podcastDataTypes.h"
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
    static string GetFieldP(string PodcastRSS,string startAtChars,string endAtChars,int &index,int startAtIndex = 0){
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
    static podcastDataTypes::PodcastEpisode GetPodcastData(const string& aRSSFile){

        podcastDataTypes::PodcastEpisode MostRecent;
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
    static podcastDataTypes::podcastDataTypes::episodeList getEpisodes(string RSSFile){
    RSSFile = GetField(RSSFile,"<channel>","</channel>");
    // find episodes maybe use while loop
    podcastDataTypes::episodeList episodes;
    int positionIndex=0;
    while(true)
    {
        string item = GetFieldP(RSSFile,"<item>","</item>",positionIndex,positionIndex);//  find item starting from last item we indexed
        if(item == ""){break;}
        episodes.addEpisode(GetPodcastData(item));// add podcast to some kind of list
    }
    return episodes;
}
};



