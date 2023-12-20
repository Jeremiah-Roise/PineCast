# PineCast

A podcast app for linux phones
Made with c++ has very basic functionality such as itunes search, and a rudimentry Library system.
[small demo](https://www.youtube.com/watch?v=qU5-zqU4DCA)
this is my first serious application built for Linux so if you see some odd design choices (bad code) that's my inexperience at work.
 #### things to do
 - [x] add built in audio player.
 - [ ] __maybe__ replace custom xml parser.
#### cmake and make should now be working but when you run the binary it must be in the same folder as the podcastWindow.glade file or it will not work.
*when compiling for actual usage use ./compile.sh 1 for an optimized build*

![Pinecast Podcast Client](/Images/Library.png)

Note the reason I use int\* varname not int \*varname because the type is int pointer not normal int which is confusing.

when you clone the repo the docs folder should only have a doxygenConfig file in it produce the docs by running doxygen on the config
there are many files in this project, so 
I'll give a basic rundown of each file.

 - ### User Interface.
     - PreviewPageClass.h holds a class which will manage a preview page for a podcast episode.
        - the PreviewPageClass initializer takes a gtk builder and an AudioPlayer to use.
        - the addEpisodesToList function takes a number of episodes to add to a UI List of episodes.
        - the callbackLoadMoreEpisodes function helps an asynchronus function add more episodes to the list dynamically.
        - the setPreviewPage function is called when the preview page is loaded with a new podcast.

     - UIFunctions.h this manages the UI and downloads for an individual episode.
        - the setButtonsDownloaded function is used to replace the download and stream buttons on an episode with delete and play buttons when the episode is in the local filesystem.
        - the setButtonsRemote function is is used to set the episode action buttons to the stream and download options when the episode is not available locally.
        - the downloadFinished function is called when an episode is successfully downloaded.
        - the updateBarHelper function it's called to update the progress bar in the episode UI.
        - the updateBar function calls the updateBarHelper function is sync with the gtk UI update loop.
        - the buttonStream function is a helper function to call class member functions.
        - the buttonPlay function is a helper function to call class member functions.
        - the buttonDownload function is a helper function to call class member functions.
        - the deletePodcast function is a helper function to call class member functions.
        - the episodeActionsUI initializer creates the UI for displaying episodes with some Podcast Data.
        - the clearContainer function just clears everthing in a gtk container.

     - Current_Episode_UI.h connects the buttons in the UI to the media controls. It is also a test of the future architecture of the application.
        - the Current_Episode_UI initializer takes a reference to a gtk builder and an AudioPlayer for the class to control.
        - the setEpisodeInfoUI function takes a PodcastDataBundle and sets the info for currently playing episode.
        - the playButtonPressed function links the UIButton to to the AudioPlayer pausePlay function.
        - the forwardButtonPressed function links the UIButton to the associated AudioPlayer function.
        - the rewindButtonPressed function links the UIButton to the associated AudioPlayer function.

 - ### Getting Data
     - webTools.h holds functions for getting info from the web.
        - the internetAccess function checks if there is a working network connection by connecting to google.com
        - the getFileInMem function gets all the data in a file online and returns it in a string.
        - the createImage function returns a GdkPixbuf from an image online
        - the getWebFile function downloads a file to the disk

     - DataTools.h holds lots of helper functions mostly for dealing with strings of data either from the apple podcast API or the filesystem. Most of the functions contained are quite self explanatory.
        - the filePathFromEpisode function takes a PodcastData and PodcastEpisode, and generates the path to the library for this podcast and episode.
        - the getFile function takes the path to a file and returns a string containing the data in the file.
        - the getFieldAndReturnIndex function gets particular values from podcast xml files and returns the index it was found at from the index reference. It takes the values,
            - PodcastRSS: a string representing the podcast RSS feed
            - startAtChars: a string representing the starting substring of the substring to be searched for.
            - endAtChars: a string representing the ending substring of the substring to be searched for.
            - startAtIndex: an integer representing the index to start searching from. (0 by default)
        - the GetField function is a subset of the previous function which does not return the index.
        - the getPodcastData function gets a podcastEpisode from a String of RSS data.
        - the getEpisodes function gets all the episodes from a string of RSS data.
        - the hasSpaces function checks if there are spaces in a string that is passed to it.
        - the replaceSpaces function replaces all the spaces in a string with a the char passed to it.
        - the cleanString function removes all characters that are not letters or numbers.
        - the extractPodcastDataFromString function gets all PodcastMetaData from an RSS string.

     - LibraryTools.h has some functions for adding and removing podcasts from the users library, as well as some functions for dealing with tracking downloaded episodes.
        - the addToLibrary function saves all the important podcast metaData to the filesystem.
        - the removeFromLibrary function deletes all of a Podcasts metaData from the filesystem.
        - the loadLib function takes a reference to a variable to load with all the Podcasts in the Library.
        - the Downloads class holds functions similar to the functions above but for downloads.

     - PodcastsStore.h is for interacting with the apple API.
        - the itunesSearch function takes a string to search the apple API with, and returns a list of Podcasts.

     - caching.h it is exactly what it says it is, it manages the caching of various things.
        - the createCacheFile function creates a cache file in /tmp folder for caching some random info.
        - the timeSinceEpoch function just returns the time since the unix epoch.
        - the isCacheFileValid function simply checks if the cache file is valid.
        - the getCachePath function takes a filename and returns the path to the cache file.

     - filepaths.h holds some functions for dealing with filepaths.
        - the lclFiles function returns the path to the directory where user data is stored.
        - the tmpPath function returns the path to the /tmp directory.
        - the folderExists function checks if a folder exists.
        - the fileExists function checks if a file exists.

 - ### Audio
     - miniaudio.h is an outside library because theres no way I could write my own audio library right now. Its a single header library that plays audio through the pc speakers.

     - AudioPlayer.h handles the audio playing in the app instantiate a class load A file and your off to the races.
        - the loadfile function takes a filepath to an mp3 and loads it for playing.
        - setPlaying takes a boolean which when true will set the audio playing, and when false will pause the audio.
        - the pausePlay function alternates the pause state of the audio.
        - the seek function takes an integer a negative number will seek the audio in reverse, and a positive number will seek forward.

     - PlayPodcast.h manages the downloading of the podcast mp3's I'm quite proud of this one it's you simply instantiate an object and store it it downloads the file and deletes itself from memory.
        - the StartDownload function creates a thread which handles the downloading of the podcast mp3.
        - the progressUpdate function is a callback for the download process which calls the update func callback.
        - the playPodcast initializer takes a PodcastDataBundle to download.

 - Libs.h Is already outdated and was a hairbrained Idea to begin with.

 - ### Data Types.
     - PodcastDataBundle.h is a composite of episode data and the podcast meta data.

     - PodcastMetaDataLists.h is a struct that holds the meta data of a podcast, the file also holds a typedef of a vector of podcast meta data stucts.

     - podcastEpisodeTypes.h is a struct for holding individual podcast episodes.



 

As you can see there are quite a few files and lots of detail to be had in each some are done much better than others.
If more details are needed they should be relatively well commented.
