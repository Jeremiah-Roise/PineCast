#include<string>
using std::string;
#pragma once



class podcastDataTypes
{
    public:
    struct PodcastEpisode
    {
        string title;
        string artist;
        string mp3Link;
        string thumbnailLink;
        string summary;
        int episode;
        string duration;
        string RssFeed;
        int index=0;
    };

    class episodeList
    {
    private:
        unsigned int index=0;
        podcastDataTypes::PodcastEpisode episodeList[1000];
    public:
        void addEpisode(podcastDataTypes::PodcastEpisode episode){
            episodeList[index] = episode;
            episode.index = index;
            index++;
        }
        podcastDataTypes::PodcastEpisode getEpisodeAtIndex(int index){
            return episodeList[index];
        }
        int getIndexSize(){return index;}
    };
};