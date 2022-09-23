#pragma once
#include <gtk-3.0/gtk/gtk.h>
#include "AudioPlayer.h"
#include "podcastEpisodeTypes.h"
    void playButtonPressed(GtkWidget* button, gpointer data);
    void forwardButtonPressed(GtkWidget* button, gpointer data);
    void rewindButtonPressed(GtkWidget* button, gpointer data);
class Current_Episode_UI
{
private:
    PodcastDataBundle currentPodcast;
    GtkBuilder* builder;
    AudioPlayer* player;
    GtkWidget* Audio_Controls_Rewind =     GTK_WIDGET(gtk_builder_get_object(builder, "Audio_Controls_Reverse"));
    GtkWidget* Audio_Controls_forward =     GTK_WIDGET(gtk_builder_get_object(builder, "Audio_Controls_forward"));
    GtkWidget* Audio_Controls_Pause_Play =  GTK_WIDGET(gtk_builder_get_object(builder, "Audio_Controls_Pause_Play"));
    GtkWidget* Episode_Information_Image =  GTK_WIDGET(gtk_builder_get_object(builder, "Episode_Information_Image"));
    GtkWidget* Episode_Information_Title =  GTK_WIDGET(gtk_builder_get_object(builder, "Episode_Information_Title"));

public:
    Current_Episode_UI(GtkBuilder* builder,AudioPlayer* playerToUse) : builder(builder), player(playerToUse)
    {
        g_signal_connect(Audio_Controls_Pause_Play,"pressed",(GCallback)playButtonPressed,(gpointer)this);
        g_signal_connect(Audio_Controls_forward,"pressed",(GCallback)forwardButtonPressed,(gpointer)this);
        g_signal_connect(Audio_Controls_Rewind,"pressed",(GCallback)rewindButtonPressed,(gpointer)this);
    }

    void setEpisodeInfoUI(PodcastDataBundle* podcastEpisode)
    {
        gtk_label_set_label(GTK_LABEL(Episode_Information_Title),podcastEpisode->Episode.title.c_str());
    }

    //  begin audioPlayer UI to audio player class bindings
    friend void playButtonPressed(GtkWidget* button, gpointer data){
        Current_Episode_UI* buttonSource = reinterpret_cast<Current_Episode_UI*>(data);
        buttonSource->player->pausePlay();
        // set button state to inversions of it's current state
    }

    friend void forwardButtonPressed(GtkWidget* button, gpointer data){
        Current_Episode_UI* buttonSource = reinterpret_cast<Current_Episode_UI*>(data);
        buttonSource->player->seek(3);
        // perform seek logic for more intuitive seeking behaviour
        // perform seek
    }

    friend void rewindButtonPressed(GtkWidget* button, gpointer data){
        Current_Episode_UI* buttonSource = reinterpret_cast<Current_Episode_UI*>(data);
        buttonSource->player->seek(-3);
        // perform seek logic for more intuitive seeking behaviour
        // perform seek
    }
};