#include<string>
#include<vector>
#include"podcastEpisodeTypes.h"
using namespace std;
#pragma once
//  conforms to apple podcast search api
struct PodcastData
{
string title;
string artist;
string image30;
string image60;
string image100;
string image600;
string RssFeed;
int index=0;
};




class PodcastDataList
{
public:
    int createAndAddPodcast(string author,string RssFeed,string title,string thumbnail30,string thumbnail60,string thumbnail100,string thumbnail600){
        PodcastData tmp;
        tmp.artist = author;
        tmp.RssFeed = RssFeed;
        tmp.image30 = thumbnail30;
        tmp.image60 = thumbnail60;
        tmp.image100 = thumbnail100;
        tmp.image600 = thumbnail600;
        tmp.title = title;
        tmp.index = index;
        podcasts.push_back(tmp);
        index ++;
        return index-1;
    }
    PodcastData GetPodcastAtIndex(int index){
        if (podcasts.empty())
        {
            cout << "list is empty" << endl;
        }
        
        return podcasts[index];
    }
    int GetIndexSize(){return index;}
    void clear(){
        podcasts.clear();
        index = 0;
    }
protected:
    int index = 0;
private:
    std::vector<PodcastData> podcasts;
    PodcastEpisodeList myEpisodes;
};





