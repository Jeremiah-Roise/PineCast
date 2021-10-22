#include<string>
#pragma once
using std::string;
using std::cout;
using std::endl;
class filepaths
{
public:
    static string lclFiles(){
        string localStorage = getenv("HOME");
        localStorage += "/.Podcasts/";
        return localStorage;
    }
    static string tmpPath(){
        string tmpDir = "/tmp/PineCast/";
        return tmpDir;
    }
    /// a function for checking whether a folder exists
    static bool folderExists(string& folder){
        struct stat tmp;
        if (stat(folder.c_str(), &tmp) != 0 && S_ISDIR(tmp.st_mode) != 1)
        {
            return false;
        }
        return true;
    }
    /// a function for checking whether a file exists
    static bool fileExists(string& folder){
        struct stat tmp;
        if (stat(folder.c_str(), &tmp) == 0)
        {
            return true;
        }
        return false;
    }
};