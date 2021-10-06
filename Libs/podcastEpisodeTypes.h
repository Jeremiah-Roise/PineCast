#include<string>
using std::string;
#pragma once



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
        int index = 0;
        float Download = 0;
    };
typedef vector<PodcastEpisode> PodcastEpisodeList;