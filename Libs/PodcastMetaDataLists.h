#include<string>
#include<vector>
using namespace std;
#pragma once
//  conforms to apple podcast search api
struct PodcastMetaData
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




class PodcastMetaDataList
{
public:
    int addPodcast(string author,string RssFeed,string title,string thumbnail30,string thumbnail60,string thumbnail100,string thumbnail600){
        PodcastMetaData tmp;
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
    PodcastMetaData GetPodcastAtIndex(int index){
        return podcasts[index];
    }
    void GetPodcastAtIndex(int index,PodcastMetaData& ret){
        ret = podcasts[index];
    }
    int GetIndex(){return index;}
    void clear(){
        podcasts.clear();
        index = 0;
    }
protected:
    int index = 0;
private:
    std::vector<PodcastMetaData> podcasts;
};





