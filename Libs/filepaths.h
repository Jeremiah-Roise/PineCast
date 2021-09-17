#include<string>
using std::string;
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