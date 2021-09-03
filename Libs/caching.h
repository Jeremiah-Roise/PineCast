#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;


class caching
{

public:
    //  creates a new cacheFile in the /tmp directory removes spaces from filename
    static void createCacheFile(const char* filename,const char* data,const int dataSize){
        fstream filehandle;
        string filepath = "/tmp/";
        filepath += filename;
        std::remove(filepath.begin(), filepath.end(), ' ');
        filehandle.open(filepath.c_str(),ios_base::out);
        filehandle.write(data,dataSize);
        filehandle.close();
        utime(filepath.c_str(),NULL);
    }
    //  get the time since epoch
    static double timeSinceEpoch(){
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    //  checks if a cachefile exists and if it's out of date if it's out of date or doesn't exist return false
    static bool isCacheFileValid(const char* filename,size_t refreshTime){
        
        string filepath = "/tmp/";
        filepath += filename;

        //  remove spaces from the path
        std::remove(filepath.begin(), filepath.end(), ' ');

        //  check the state of the cachefile
        struct stat tmp;
        int cacheFile = stat(filepath.c_str(), &tmp);

        // getting epoch time for comparison against the cache file creation date incremented by one day
        double now = caching::timeSinceEpoch();

        //  get RSS file if cache does not exist or is out of date.
        if (cacheFile == -1 || (cacheFile == 0 && tmp.st_atim.tv_sec + refreshTime <= now)){
            return false;
        }
        return true;
    }
};
