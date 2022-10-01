# PineCast

A podcast app for linux phones
Made with c++ has very basic functionality such as itunes search, and a rudimentry Library system.
this is my first serious application built for Linux so if you see some odd design choices (bad code) that's my inexperience at work.
 #### things to do
 - [x] add built in audio player.
 - [ ] __maybe__ replace custom xml parser.
#### cmake and make should now be working but when you run the binary it must be in the same folder as the podcastWindow.glade file or it will not work.
*when compiling for actual usage use ./compile.sh 1 for an optimized build*

![Pinecast Podcast Client](/Images/Library.png)

Note the reason I use int\* varname not int \*varname because the type is int pointer not normal int which is confusing.

when you clone the repo the docs folder should only have a doxygenConfig file in it produce the docs by running doxygen on the config
