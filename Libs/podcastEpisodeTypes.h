#pragma once
#include<string>
using std::string;



    struct PodcastEpisode
    {
        string title = "";
        string artist = "";
        string RssFeed = "";
        string mp3Link = "";
        string summary = "";
        string duration = "";
        int index = 0;
        int episode = 0;
        float Download = 0;
    };
typedef vector<PodcastEpisode> PodcastEpisodeList;