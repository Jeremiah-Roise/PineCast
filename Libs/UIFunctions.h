#pragma once
#include<gtk/gtk.h>
#include <iostream>
#include <stdio.h>
#include <future>
#include <stdlib.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include"podcastEpisodeTypes.h"
#include"PodcastMetaDataLists.h"
#include"PodcastDataBundle.h"
#include "../UINAMES.h"
void clearContainer(GtkContainer* e);
void deletePodcast(GtkWidget*, gpointer);
void buttonDownload(GtkWidget* e, gpointer data);
void buttonStream(GtkWidget* e, gpointer data);
void buttonPlay(GtkWidget* e, gpointer data);

  void streamPodcast(PodcastDataBundle& Episode, GtkProgressBar* bar){
    cout << Episode.Episode.title << endl;
    PlayPodcast<GtkProgressBar*>* download = new PlayPodcast<GtkProgressBar*>(0.5,Episode,bar);
    download->updateEventFunc = [](double fraction, PodcastDataBundle Podcast,GtkProgressBar* bar)
    {
      gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar),fraction);
    };

    download->eventPointFunc = [](double fraction, PodcastDataBundle Podcast,GtkProgressBar* bar)
    {
      GtkWidget* parent = gtk_widget_get_parent(GTK_WIDGET(bar));
      GtkWidget* playButton = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
      GtkWidget* deleteButton = gtk_button_new_from_icon_name("gtk-delete",GTK_ICON_SIZE_BUTTON);
      gtk_container_add(GTK_CONTAINER(parent),playButton);
      gtk_container_add(GTK_CONTAINER(parent),deleteButton);
      gtk_widget_show(playButton);
      gtk_widget_show(deleteButton);
    };

    download->StartDownload();
    Downloads::addToDownloads(Episode.Episode);
  } 


  void downloadPodcast(GtkProgressBar* progressBar, PodcastDataBundle Episode){
    cout << Episode.Episode.title << endl;
    PlayPodcast<GtkProgressBar*>* download = new PlayPodcast<GtkProgressBar*>(1,Episode,progressBar);

    download->updateEventFunc = [](double fraction, PodcastDataBundle Podcast,GtkProgressBar* bar)
    {
      gtk_progress_bar_set_fraction(bar,fraction);
    };

    download->eventPointFunc = [](double fraction, PodcastDataBundle Podcast,GtkProgressBar* bar)
    {
      GtkWidget* parent = gtk_widget_get_parent(GTK_WIDGET(bar));
      GtkWidget* playButton = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
      GtkWidget* deleteButton = gtk_button_new_from_icon_name("gtk-delete",GTK_ICON_SIZE_BUTTON);

      gtk_container_add(GTK_CONTAINER(parent),playButton);
      gtk_container_add(GTK_CONTAINER(parent),deleteButton);

      gtk_widget_show(playButton);
      gtk_widget_show(deleteButton);
      gtk_widget_destroy(GTK_WIDGET(bar));
    };

    download->StartDownload();
    Downloads::addToDownloads(Episode.Episode);
  } 


  class episodeActionsUI
  {
    public:
      GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
      PodcastDataBundle Podcast;
      GtkProgressBar* progressTracker = GTK_PROGRESS_BAR(gtk_progress_bar_new());
    private:
      GtkButton* button1;
      GtkButton* button2;

      string title = Podcast.Episode.title;
      string duration = "<span size=\"medium\"><i>" + Podcast.Episode.duration + "</i></span>";

      GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
      GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
      GtkWidget* titleLabel = gtk_label_new(title.c_str());
      GtkWidget* durationLabel = gtk_label_new(duration.c_str());

    public:
      episodeActionsUI(bool isDownloaded,PodcastDataBundle& Podcast) : Podcast(Podcast)
      {
        if (isDownloaded == true)// if true create the UI for a Podcast Episode that has been downloaded
        {
          button1 = GTK_BUTTON (gtk_button_new_from_icon_name("gtk-delete",GTK_ICON_SIZE_BUTTON));
          button2 = GTK_BUTTON (gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON));
          g_signal_connect(button1, "released", (GCallback)deletePodcast, (gpointer) this);
          g_signal_connect(button2, "released", (GCallback)buttonPlay, (gpointer) this);
        }
        if(isDownloaded == false)// if true create the UI for a podcast that has no been downloaded
        {
          button1 = GTK_BUTTON (gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON));
          button2 = GTK_BUTTON (gtk_button_new_from_icon_name("emblem-downloads", GTK_ICON_SIZE_BUTTON));
          g_signal_connect(button1, "released", (GCallback)buttonStream,(gpointer) this);
          g_signal_connect(button2, "released", (GCallback)buttonDownload, (gpointer) this);
        }
        gtk_label_set_line_wrap(GTK_LABEL(titleLabel), true);// enables line wrap
        gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.0);// sets lables to left align

        gtk_label_set_line_wrap(GTK_LABEL(durationLabel), true);// enables line wrap
        gtk_label_set_xalign(GTK_LABEL(durationLabel), 0.0);// sets lables to left align
        gtk_label_set_use_markup(GTK_LABEL(durationLabel), true);// enables markup on label
        gtk_label_set_markup(GTK_LABEL(durationLabel), duration.c_str());// displays the duration of the podcast in italics

        gtk_box_pack_start(GTK_BOX(infoBox), titleLabel, false, false, 0);
        gtk_box_pack_start(GTK_BOX(infoBox), durationLabel, false, false, 0);
        gtk_box_pack_start(GTK_BOX(topBox), infoBox, false, false, 0);
        gtk_box_pack_end(GTK_BOX(topBox), buttonBox, false, false, 0);

        gtk_box_pack_start(GTK_BOX(buttonBox), GTK_WIDGET(button1), false, false, 0);
        gtk_box_pack_start(GTK_BOX(buttonBox), GTK_WIDGET(button2), false, false, 0);

        gtk_widget_show_all(topBox);

      }
  };
  
  void buttonPlay(GtkWidget* e, gpointer data){
    episodeActionsUI buttonSource = *reinterpret_cast<episodeActionsUI*>(data);
    play(buttonSource.Podcast.Episode,buttonSource.Podcast.Podcast);
  } 
  void buttonStream(GtkWidget* e, gpointer data){
    episodeActionsUI buttonSource = *reinterpret_cast<episodeActionsUI*>(data);
    streamPodcast(buttonSource.Podcast,buttonSource.progressTracker);
  }
  void buttonDownload(GtkWidget* e, gpointer data){
    episodeActionsUI buttonSource = *reinterpret_cast<episodeActionsUI*>(data);
    downloadPodcast(buttonSource.progressTracker,buttonSource.Podcast);
  }

  void deletePodcast(GtkWidget* e, gpointer data){
    episodeActionsUI buttonSource = *reinterpret_cast<episodeActionsUI*>(data);
    string path = DataTools::filePathFromEpisode(buttonSource.Podcast.Episode,buttonSource.Podcast.Podcast);
    cout << path << endl;
    Downloads::removeFromDownloads(buttonSource.Podcast.Episode,buttonSource.Podcast.Podcast);
  } 

  ///  clears the given container of all children
  void clearContainer(GtkContainer* e)
  {
    gtk_container_foreach(e, (GtkCallback)gtk_widget_destroy, NULL);
  }