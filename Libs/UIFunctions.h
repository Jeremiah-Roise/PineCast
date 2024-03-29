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
#include <string>
#include "podcastEpisodeTypes.h"
#include "PodcastMetaDataLists.h"
#include "PodcastDataBundle.h"
#include "PlayPodcast.h"
#include "LibraryTools.h"
//void clearContainer(GtkContainer* e);
void deletePodcast(GtkWidget*, gpointer);
void buttonDownload(GtkWidget* e, gpointer data);
void buttonStream(GtkWidget* e, gpointer data);
void buttonPlay(GtkWidget* e, gpointer data);
 
extern void setCurrentPodcastEpisode(PodcastDataBundle* Podcast);

  /// handles all the 
  class episodeActionsUI : public PlayPodcast
  {
    public:
      GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
      PodcastDataBundle Podcast;
      GtkProgressBar* progressTracker = GTK_PROGRESS_BAR(gtk_progress_bar_new());
    private:
      enum eventActionType {stream,download_Noplay,download_Play};
      AudioPlayer* player;
    public:
      eventActionType actionToPerform;
      bool ranEvent = false;
    private:
      GtkButton* button1;
      GtkButton* button2;

      string title = Podcast.Episode.title;
      string duration = "<span size=\"medium\"><i>" + Podcast.Episode.duration + "</i></span>";
      bool started = false;
      double amount = 0;

      GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
      GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
      GtkWidget* titleLabel = gtk_label_new(title.c_str());
      GtkWidget* durationLabel = gtk_label_new(duration.c_str());

      void setButtonsDownloaded(){
        //  best way I've found to update the icons on the buttons
        gtk_container_remove(GTK_CONTAINER(buttonBox),GTK_WIDGET(button1));
        button1 = GTK_BUTTON (gtk_button_new_from_icon_name("gtk-delete",GTK_ICON_SIZE_BUTTON));
        gtk_box_pack_start(GTK_BOX(buttonBox), GTK_WIDGET(button1), false, false, 0);

        gtk_container_remove(GTK_CONTAINER(buttonBox),GTK_WIDGET(button2));
        button2 = GTK_BUTTON (gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON));
        gtk_box_pack_end(GTK_BOX(buttonBox), GTK_WIDGET(button2), false, false, 0);

        g_signal_connect(button1, "released", (GCallback)deletePodcast, (gpointer) this);
        g_signal_connect(button2, "released", (GCallback)buttonPlay, (gpointer) this);
      }

      void setButtonsRemote(){
        //  best way I've found to update the icons on the buttons
        gtk_container_remove(GTK_CONTAINER(buttonBox),GTK_WIDGET(button1));
        button1 = GTK_BUTTON (gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON));
        gtk_box_pack_start(GTK_BOX(buttonBox), GTK_WIDGET(button1), false, false, 0);

        gtk_container_remove(GTK_CONTAINER(buttonBox),GTK_WIDGET(button2));
        button2 = GTK_BUTTON (gtk_button_new_from_icon_name("emblem-downloads", GTK_ICON_SIZE_BUTTON));
        gtk_box_pack_end(GTK_BOX(buttonBox), GTK_WIDGET(button2), false, false, 0);

        g_signal_connect(button1, "released", (GCallback)buttonStream,(gpointer) this);
        g_signal_connect(button2, "released", (GCallback)buttonDownload, (gpointer) this);
      }

      void downloadFinished(){
        cout << "DOWNLOAD FINISHED" << endl;

        gtk_widget_hide(GTK_WIDGET(progressTracker));

        setButtonsDownloaded();

        gtk_widget_show(GTK_WIDGET(button1));
        gtk_widget_show(GTK_WIDGET(button2));
        gtk_progress_bar_set_fraction(progressTracker,0.0f);
        Downloads::addToDownloads(Podcast.Episode);
        reset();//	reset the PlayPodcast object
      }
      
      void updateBar(double amount){
        this->amount = amount;
        //  this jank is to prevent gtk from crashing
        gdk_threads_add_idle((GSourceFunc)&episodeActionsUI::updateBarHelper,(gpointer)this);
      }

      static gboolean updateBarHelper(gpointer callback){
        episodeActionsUI* buttonSource = reinterpret_cast<episodeActionsUI*>(callback);
        if (buttonSource->amount == -1)
        {
          return FALSE;
        }
        if (buttonSource->amount >= 1)
        {
          buttonSource->amount = -1;
          buttonSource->downloadFinished();
          gtk_progress_bar_set_fraction(buttonSource->progressTracker,1);
          //  this kills all scheduled events of this type eg: the download went much faster than the ui updated and the download is finished now
          return FALSE;
        }
        
        cout << "this is the update function: " << buttonSource->amount << "\n";
        gtk_progress_bar_set_fraction(buttonSource->progressTracker,buttonSource->amount);
        //	this is the section to set up events such as starting an audio player halfway through the download
        if(buttonSource->actionToPerform == stream && buttonSource->amount >= 0.2 && buttonSource->ranEvent == false){
          buttonSource->ranEvent = true;
          std::string filepath = DataTools::filePathFromEpisode(buttonSource->Podcast.Episode,buttonSource->Podcast.Podcast);
          buttonSource->player->load_file(filepath.c_str());
          buttonSource->player->setPlaying(true);
          setCurrentPodcastEpisode(&buttonSource->Podcast);
        }
        if(buttonSource->actionToPerform == download_Play && buttonSource->amount >= 1 && buttonSource->ranEvent == false){
          buttonSource->ranEvent = true;
          std::string filepath = DataTools::filePathFromEpisode(buttonSource->Podcast.Episode,buttonSource->Podcast.Podcast);
          buttonSource->player->load_file(filepath.c_str());
          buttonSource->player->setPlaying(true);
          setCurrentPodcastEpisode(&buttonSource->Podcast);
        }
        if(buttonSource->actionToPerform == download_Noplay && buttonSource->amount >= 1 && buttonSource->ranEvent == false){
          buttonSource->ranEvent = true;
          
        }
        
        return TRUE;
      }

    public:
      friend void buttonStream(GtkWidget* e, gpointer data){
        episodeActionsUI* buttonSource = reinterpret_cast<episodeActionsUI*>(data);
        //streamPodcast(buttonSource.Podcast,buttonSource.progressTracker);
        buttonSource->StartDownload();
        if (buttonSource->started == false)
        {
	  buttonSource->actionToPerform = episodeActionsUI::stream;
          buttonSource->started = true;
          gtk_widget_show(GTK_WIDGET(buttonSource->progressTracker));
          gtk_widget_hide(GTK_WIDGET(buttonSource->button1));
          gtk_widget_hide(GTK_WIDGET(buttonSource->button2));
        }
        
      }
      friend void buttonPlay(GtkWidget* e, gpointer data){
        episodeActionsUI* buttonSource = reinterpret_cast<episodeActionsUI*>(data);
        std::string filepath = DataTools::filePathFromEpisode(buttonSource->Podcast.Episode,buttonSource->Podcast.Podcast);
        if (filepaths::fileExists(filepath))
        {
          //playMp3(filepath);
          buttonSource->player->load_file(filepath.c_str());
          buttonSource->player->setPlaying(true);
          setCurrentPodcastEpisode(&buttonSource->Podcast);
          return;
        }
      } 
      friend void buttonDownload(GtkWidget* e, gpointer data){
        episodeActionsUI* buttonSource = reinterpret_cast<episodeActionsUI*>(data);
        buttonSource->StartDownload();
        if (buttonSource->started == false)
        {
          buttonSource->actionToPerform = episodeActionsUI::download_Noplay;
          buttonSource->started = true;
          gtk_widget_show(GTK_WIDGET(buttonSource->progressTracker));
          gtk_widget_hide(GTK_WIDGET(buttonSource->button1));
          gtk_widget_hide(GTK_WIDGET(buttonSource->button2));
        }
      }

      friend void deletePodcast(GtkWidget* e, gpointer data){
        episodeActionsUI* buttonSource = reinterpret_cast<episodeActionsUI*>(data);
        string path = DataTools::filePathFromEpisode(buttonSource->Podcast.Episode,buttonSource->Podcast.Podcast);
        cout << path << endl;
        Downloads::removeFromDownloads(buttonSource->Podcast.Episode,buttonSource->Podcast.Podcast);
        buttonSource->setButtonsRemote();
        gtk_widget_show(GTK_WIDGET(buttonSource->button1));
        gtk_widget_show(GTK_WIDGET(buttonSource->button2));
      } 
      episodeActionsUI(bool isDownloaded,PodcastDataBundle inputPodcast,AudioPlayer* playerToUse) : PlayPodcast(inputPodcast), Podcast(inputPodcast), player(playerToUse)
      {
        updateFunc = std::bind(&episodeActionsUI::updateBar,this,std::placeholders::_1);
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
        gtk_label_set_max_width_chars(GTK_LABEL(titleLabel),50);
        gtk_label_set_line_wrap_mode(GTK_LABEL(titleLabel),PANGO_WRAP_WORD);
        gtk_label_set_ellipsize(GTK_LABEL(titleLabel),PANGO_ELLIPSIZE_END);
        gtk_label_set_lines(GTK_LABEL(titleLabel),2);
        gtk_label_set_line_wrap(GTK_LABEL(titleLabel), true);// enables line wrap
        gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.0);// sets lables to left align
        gtk_widget_set_tooltip_text(titleLabel,title.c_str());

        gtk_label_set_line_wrap(GTK_LABEL(durationLabel), true);// enables line wrap
        gtk_label_set_xalign(GTK_LABEL(durationLabel), 0.0);// sets lables to left align
        gtk_label_set_use_markup(GTK_LABEL(durationLabel), true);// enables markup on label
        gtk_label_set_markup(GTK_LABEL(durationLabel), duration.c_str());// displays the duration of the podcast in italics

        gtk_box_pack_start(GTK_BOX(infoBox), titleLabel, false, false, 0);
        gtk_box_pack_start(GTK_BOX(infoBox), durationLabel, false, false, 0);
        gtk_box_pack_start(GTK_BOX(topBox), infoBox, false, false, 0);
        gtk_box_pack_end(GTK_BOX(topBox), buttonBox, false, false, 0);

        gtk_box_pack_start(GTK_BOX(buttonBox), GTK_WIDGET(progressTracker), false, false, 0);
        gtk_box_pack_start(GTK_BOX(buttonBox), GTK_WIDGET(button1), false, false, 0);
        gtk_box_pack_start(GTK_BOX(buttonBox), GTK_WIDGET(button2), false, false, 0);

        gtk_widget_show_all(topBox);
        gtk_widget_hide(GTK_WIDGET(progressTracker));
      }
  };

  

  ///  clears the given container of all children
  void clearContainer(GtkContainer* e)
  {
    gtk_container_foreach(e, (GtkCallback)gtk_widget_destroy, NULL);
  }
