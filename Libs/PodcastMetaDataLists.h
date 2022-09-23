#pragma once
#include<string>
#include<vector>
#include"podcastEpisodeTypes.h"
using namespace std;
//  conforms to apple podcast search api
struct PodcastData
{
string title = "";
string artist = "";
string image30 = "";
string image60 = "";
string image100 = "";
string image600 = "";
string RssFeed = "";
size_t index = 0;
};

typedef vector<PodcastData> PodcastDataList;