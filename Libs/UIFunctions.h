#include <gtk-3.0/gtk/gtk.h>
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

  auto standardWidgetBuilder(PodcastDataBundle& tmp,void(*playFunction)(GtkWidget*, gpointer), void(*downloadFunction)(GtkWidget*, gpointer)){ 

    PodcastDataBundle* SelectedPodcast = new PodcastDataBundle;
    *SelectedPodcast = tmp;

    string title = SelectedPodcast->Episode.title;
    string duration = "<span size=\"medium\"><i>" + SelectedPodcast->Episode.duration + "</i></span>";

    GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget* playButton = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
    GtkWidget* downloadButton = gtk_button_new_from_icon_name("emblem-downloads", GTK_ICON_SIZE_BUTTON);
    GtkWidget* titleLabel = gtk_label_new(title.c_str());
    GtkWidget* durationLabel = gtk_label_new(duration.c_str());

    gtk_label_set_line_wrap(GTK_LABEL(titleLabel), true);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.0);

    gtk_label_set_line_wrap(GTK_LABEL(durationLabel), true);
    gtk_label_set_xalign(GTK_LABEL(durationLabel), 0.0);
    gtk_label_set_use_markup(GTK_LABEL(durationLabel), true);
    gtk_label_set_markup(GTK_LABEL(durationLabel), duration.c_str());

    gtk_box_pack_start(GTK_BOX(infoBox), titleLabel, false, false, 0);
    gtk_box_pack_start(GTK_BOX(infoBox), durationLabel, false, false, 0);

    gtk_box_pack_start(GTK_BOX(buttonBox), playButton, false, false, 0);
    gtk_box_pack_end(GTK_BOX(buttonBox), downloadButton, false, false, 0);

    gtk_box_pack_start(GTK_BOX(topBox), infoBox, false, false, 0);
    gtk_box_pack_end(GTK_BOX(topBox), buttonBox, false, false, 0);
    gtk_widget_show_all(topBox);

    g_signal_connect(playButton, "released", (GCallback)playFunction, (gpointer) SelectedPodcast);
    g_signal_connect(downloadButton, "released", (GCallback)downloadFunction, (gpointer) SelectedPodcast);

    return topBox;
  }


  auto downloadedWidgetBuilder(PodcastDataBundle& tmp, void(*playFunction)(GtkWidget*, gpointer),void(*deleteFunc)(GtkWidget*, gpointer)){

    PodcastDataBundle* SelectedEpisode = new PodcastDataBundle;
    *SelectedEpisode = tmp;
    string title = SelectedEpisode->Episode.title;
    string duration = "<span size=\"medium\"><i>" + SelectedEpisode->Episode.duration + "</i></span>";

    GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget* playButton = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
    GtkWidget* deleteButton = gtk_button_new_from_icon_name("gtk-delete",GTK_ICON_SIZE_BUTTON);
    GtkWidget* titleLabel = gtk_label_new(title.c_str());
    GtkWidget* durationLabel = gtk_label_new(duration.c_str());

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

    gtk_box_pack_start(GTK_BOX(buttonBox), playButton, false, false, 0);
    gtk_box_pack_start(GTK_BOX(buttonBox), deleteButton, false, false, 0);

    gtk_widget_show_all(topBox);

    g_signal_connect(playButton, "released", (GCallback)playFunction, (gpointer) SelectedEpisode);
    g_signal_connect(deleteButton, "released", (GCallback)deleteFunc, (gpointer) SelectedEpisode);

    return topBox;
  };

  void playPodcastButtonFunc(GtkWidget* e, gpointer dataBundle){
    PodcastDataBundle Episode = *(PodcastDataBundle*)dataBundle;
    cout << Episode.Episode.title << endl;
    GtkWidget* bar = gtk_progress_bar_new();
    PlayPodcast<GtkWidget*>* download = new PlayPodcast<GtkWidget*>(1,Episode.Episode,Episode.Podcast,bar);
    download->updateEventFunc = [](double fraction,GtkWidget* bar){gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar),fraction);};
    download->play();
  } 

  void downloadPodcast(GtkWidget* e, gpointer dataBundle){
    cout << "start" << endl;
    PodcastDataBundle Episode = *(PodcastDataBundle*)dataBundle;
    cout << Episode.Episode.title << endl;
    GtkWidget* bar = gtk_progress_bar_new();
    PlayPodcast<GtkWidget*>* download = new PlayPodcast<GtkWidget*>(1,Episode.Episode,Episode.Podcast,bar);
    download->updateEventFunc = [](double fraction,GtkWidget* bar){gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar),fraction);};
    download->StartDownload();
    Downloads::addToDownloads(Episode.Episode);
  } 

  void streamPodcast(GtkWidget* e, gpointer dataBundle){
    PodcastDataBundle Episode = *(PodcastDataBundle*)dataBundle;
    cout << Episode.Episode.title << endl;
    GtkWidget* bar = gtk_progress_bar_new();
    PlayPodcast<GtkWidget*>* download = new PlayPodcast<GtkWidget*>(0.5,Episode.Episode,Episode.Podcast,bar);
    download->updateEventFunc = [](double fraction,GtkWidget* bar){gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar),fraction);};
    download->StartDownload();
    Downloads::addToDownloads(Episode.Episode);
  } 

  void deletePodcast(GtkWidget* e, gpointer dataBundle){
    PodcastDataBundle Episode = *(PodcastDataBundle*)dataBundle;
    cout << Episode.Episode.title << endl;
    string path = DataTools::filePathFromEpisode(Episode.Episode,Episode.Podcast);
    cout << path << endl;
    Downloads::removeFromDownloads(Episode.Episode,Episode.Podcast);
  } 