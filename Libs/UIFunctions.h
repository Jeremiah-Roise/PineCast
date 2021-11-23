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
#include "../UINAMES.h"

    auto standardWidgetBuilder(PodcastEpisode& SelectedEpisode,void(*playFunction)){ 
      string title = SelectedEpisode.title;
      string duration = "<span size=\"medium\"><i>" + SelectedEpisode.duration + "</i></span>";

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
      gtk_widget_set_name(GTK_WIDGET(playButton),to_string(SelectedEpisode.index).c_str());
      gtk_widget_set_name(GTK_WIDGET(downloadButton),to_string(SelectedEpisode.index).c_str());

      gtk_box_pack_start(GTK_BOX(topBox), infoBox, false, false, 0);
      gtk_box_pack_end(GTK_BOX(topBox), buttonBox, false, false, 0);
      gtk_widget_show_all(topBox);
      return topBox;
    }


    auto downloadedWidgetBuilder(PodcastEpisode& SelectedEpisode, void(*playFunction)(PodcastEpisode),void(*deleteFunc)(PodcastEpisode)){

      string title = SelectedEpisode.title;
      string duration = "<span size=\"medium\"><i>" + SelectedEpisode.duration + "</i></span>";

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

      g_signal_connect(playButton, "released", (GCallback)playFunction, (gpointer) &SelectedEpisode);
      g_signal_connect(deleteButton, "released", (GCallback)deleteFunc, (gpointer) &SelectedEpisode);

      return topBox;
    };