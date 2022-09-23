#pragma once
#include<string>
using std::string;



    struct PodcastEpisode
    {
        string title = "";
        string artist = "";
        string mp3Link = "";
        string thumbnailLink = "";
        string summary = "";
        int episode = 0;
        string duration = "";
        string RssFeed = "";
        int index = 0;
        float Download = 0;
    };
typedef vector<PodcastEpisode> PodcastEpisodeList;