#pragma once
#include"UIFunctions.h"
#include"PodcastDataBundle.h"
#include"podcastEpisodeTypes.h"
#include"PodcastMetaDataLists.h"
#include<vector>
#include<gtk-3.0/gtk/gtk.h>
void callbackLoadMoreEpisodes(GtkWidget* e,GtkPositionType, gpointer data);

/// class to track everything in the preview Page
///
/// there should only be one pointer to this class type
class PreviewPageClass
{
    private:
        GtkBuilder* builder;
        GtkWidget* UIPVaddToLibraryButton = GTK_WIDGET(gtk_builder_get_object(builder, addToLibraryButtonName));
        GtkWidget* UIPVImage =            GTK_WIDGET(gtk_builder_get_object(builder, PVImageName));
        GtkWidget* UIPVTitle =            GTK_WIDGET(gtk_builder_get_object(builder, PVTitleName));
        GtkWidget* UIPVAuthor =           GTK_WIDGET(gtk_builder_get_object(builder, PVAuthorName));
        GtkWidget* UIPVEpisodeList =      GTK_WIDGET(gtk_builder_get_object(builder, PVEpisodeListName));
        GtkWidget* UIPVPodcastDetailsPage = GTK_WIDGET(gtk_builder_get_object(builder, podcastDetailsPageName));
        GtkWidget* UIPVScrollWindow =       GTK_WIDGET(gtk_builder_get_object(builder, "LibraryScrolledWindow"));
        AudioPlayer* player;
        PodcastData currentPodcast;
        vector<episodeActionsUI*> episodeActions;// to keep track of episodes currently on the page
        PodcastEpisodeList currentEpisodes;
        void addEpisodesToList(unsigned short int numberOfEpisodesToLoad){
            for (int i = 0;(i < numberOfEpisodesToLoad && numberLoadedEpisodes < currentEpisodes.size()); i++)
            {
              PodcastEpisode episode = currentEpisodes.at(numberLoadedEpisodes);
              PodcastDataBundle dataBundle;
              dataBundle.Episode = episode;
              dataBundle.Podcast = currentPodcast;

              if (Downloads::isEpisodeDownloaded(episode))
              {
                  episodeActionsUI* tmp = new episodeActionsUI(true,dataBundle,player);
                  gtk_container_add(GTK_CONTAINER(UIPVEpisodeList), tmp->topBox);
              }

              else
              {
                  episodeActionsUI* tmp = new episodeActionsUI(false,dataBundle,player);
                  gtk_container_add(GTK_CONTAINER(UIPVEpisodeList), tmp->topBox);
              }
                numberLoadedEpisodes++;
            }
        }

    public:
        friend void callbackLoadMoreEpisodes(GtkWidget* e,GtkPositionType, gpointer data){
            PreviewPageClass* source = reinterpret_cast<PreviewPageClass*>(data);
            source->addEpisodesToList(10);
            cout << "callback subfunction called" << endl;
        }
        unsigned short int numberLoadedEpisodes = 0;
        PodcastData lastViewedPodcast;//    to prevent reloading the current page.
        PreviewPageClass(GtkBuilder* builder,AudioPlayer* playerToUse) : builder(builder), player(playerToUse)
        {
          g_signal_connect(UIPVScrollWindow, "edge-reached", (GCallback)callbackLoadMoreEpisodes, (gpointer)this);
          g_signal_connect(UIPVScrollWindow, "edge-overshot",(GCallback)callbackLoadMoreEpisodes, (gpointer)this);
        }


        /// when this is called it initializes the preview page.
        ///
        /// it uses the global currentPodcast variable to get the Podcast title, image, artist, etc,
        /// and iterates through the episodes argument to create the episodes,
        /// should probably be updated to not use global variables.
        void setPreviewPage(PodcastEpisodeList episodes,PodcastData Podcast,bool libraryMode)
        {
            currentPodcast = Podcast;
            currentEpisodes = episodes;

            //  hide the add to library button if in the library
            if (libraryMode == true)
                gtk_widget_hide(UIPVaddToLibraryButton);

            if (libraryMode == false)
                gtk_widget_show(UIPVaddToLibraryButton);

            gtk_label_set_text(GTK_LABEL(UIPVTitle), currentPodcast.title.c_str());
            gtk_label_set_text(GTK_LABEL(UIPVAuthor), currentPodcast.artist.c_str());
            gtk_image_set_from_pixbuf(GTK_IMAGE(UIPVImage), webTools::createImage(currentPodcast.image600, 200, 200));


            if (lastViewedPodcast.title == Podcast.title)
            {
                return;
            }
            numberLoadedEpisodes = 0;
            
            clearContainer(GTK_CONTAINER(UIPVEpisodeList));
            addEpisodesToList(10);
            lastViewedPodcast = Podcast;
        }
};

