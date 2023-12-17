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
 - AudioPlayer.h handles the audio playing in the app instantiate a class load A file and your off to the races.
 - Current_Episode_UI.h connects the buttons in the UI to the media controls.
 - DataTools.h holds lots of helper functions mostly for dealing with strings of data either from the apple podcast API or the filesystem. Most of the functions contained are quite self explanatory.
 - LibraryTools.h has some functions for adding and removing podcasts from the users library, as well as some functions for dealing with tracking downloaded episodes.
 - Libs.h Is already outdated and was a hairbrained Idea to begin with.
 - PlayPodcast.h manages the downloading of the podcast mp3's I'm quite proud of this one it's you simply instantiate an object and store it it downloads the file and deletes itself from memory.
 - PodcastDataBundle.h is a composite of episode data and the podcast meta data.
 - PodcastMetaDataLists.h is a struct that holds the meta data of a podcast, the file also holds a typedef of a vector of podcast meta data stucts.
 - PodcastsLibrary.h is an experimental version of libraryTool.h
 - PodcastsStore.h is for interacting with the apple API.
 - PreviewPageClass.h holds a class which will manage a preview page for a podcast episode.
 - UIFunctions.h this manages the UI and downloads for an individual episode.
 - caching.h it is exactly what it says it is, it manages the caching of various things.
 - filepaths.h holds some functions for dealing with filepaths.
 - miniaudio.h is an outside library because theres no way I could write my own audio library right now. Its a single header library that plays audio through the pc speakers.
 - podcastEpisodeTypes.h is a struct for holding individual podcast episodes.
 - webTools.h holds functions for getting info from the web.

As you can see there are quite a few files and lots of detail to be had in each some are done much better than others.
If more details are needed they should be relatively well commented.
