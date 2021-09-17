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
};