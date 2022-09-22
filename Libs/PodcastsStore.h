#include "PodcastDataBundle.h"
#include "PodcastMetaDataLists.h"
#include "webTools.h"
#include <string>
#include <iostream>
using std::cout;
using std::endl;
class PodcastsStore
{
private:
    /* data */
public:
    PodcastsStore(/* args */);
    ~PodcastsStore();

    /// Searches Itunes Database of podcasts and parses the results into: Podcast name; artist; feedurl; image url.
    PodcastDataList itunesSearch(string search = ""){
      if(search == "" || search.c_str() == NULL){PodcastDataList test;return test;}// catch if search string is empty
      for (unsigned int i = 0; i < search.length(); i++)
      {
        if (search[i] == ' '){search[i] = '+';}
      }
      string itunesQuery = "https://itunes.apple.com/search?media=podcast&term=" + search;
      cout << itunesQuery <<"\n"<< endl;
      std::string data;
      data = webTools::getFileInMem(itunesQuery);
      // how to parse Json
      int items = std::stoi(DataTools::GetField(data,"\"resultCount\":",","));
      data = DataTools::GetField(data,"\"results\":","");
      PodcastDataList tmp;
      int tmp0 = 0;
      int tmp1 = 0;
      int tmp2 = 0;
      int tmp3 = 0;
      for (int i = 0; i != items; i++)
      {
        PodcastData tmpData;
        // get image url's
        tmpData.image30  = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl30\":\"","\"",tmp0,tmp0);
        tmpData.image60  = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl60\":\"","\"",tmp0,tmp0);
        tmpData.image100 = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl100\":\"","\"",tmp0,tmp0);
        tmpData.image600 = DataTools::GetFieldAndReturnIndex(data,"\"artworkUrl600\":\"","\"",tmp0,tmp0);

        // get feed url
        tmpData.RssFeed  = DataTools::GetFieldAndReturnIndex(data,"\"feedUrl\":\"","\"",tmp1,tmp1);

        // get collection Name
        tmpData.title    = DataTools::GetFieldAndReturnIndex(data,"\"collectionName\":\"","\"",tmp2,tmp2);

        // get artist
        tmpData.artist   = DataTools::GetFieldAndReturnIndex(data,"\"artistName\":\"","\"",tmp3,tmp3);
        
      tmp.push_back(tmpData);
      tmp.back().index = (tmp.size() - 1);
      }
      return tmp;
    }
    };

PodcastsStore::PodcastsStore(/* args */)
{
}

PodcastsStore::~PodcastsStore()
{
}
