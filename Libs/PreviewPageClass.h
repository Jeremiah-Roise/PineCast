#pragma once
#include"UIFunctions.h"
#include"PodcastDataBundle.h"
#include"podcastEpisodeTypes.h"
#include"PodcastMetaDataLists.h"
#include<vector>

//  singleton class to track everything in the preview Page
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
        PodcastData lastViewedPodcast;//    to prevent reloading the current page.
        PodcastData currentPodcast;
        
        vector<episodeActionsUI*> episodeActions;// to keep track of episodes currently on the page

    public:
        PreviewPageClass(GtkBuilder* builder) : builder(builder){}


        /// when this is called it initializes the preview page.
        ///
        /// when called it uses the global currentPodcast variable to get the Podcast title, image, artist, etc,
        /// and iterates through the episodes argument to create the episodes,
        /// should probably be updated to not use global variables.
        void setPreviewPage(PodcastEpisodeList episodes,PodcastData Podcast)
        {
            currentPodcast = Podcast;

            //  hide the add to library button if in the library
            //if (page == 0)
                //gtk_widget_hide(UIPVaddToLibraryButton);

            //if (page == 1)
                //gtk_widget_show(UIPVaddToLibraryButton);

            gtk_label_set_text(GTK_LABEL(UIPVTitle), currentPodcast.title.c_str());
            gtk_label_set_text(GTK_LABEL(UIPVAuthor), currentPodcast.artist.c_str());
            gtk_image_set_from_pixbuf(GTK_IMAGE(UIPVImage), webTools::createImage(currentPodcast.image600, 200, 200));


            clearContainer(GTK_CONTAINER(UIPVEpisodeList));
            GtkWidget* singleEntry;
            for (PodcastEpisode& episode:episodes)
            {
              PodcastDataBundle dataBundle;
              dataBundle.Episode = episode;
              dataBundle.Podcast = currentPodcast;

              if (Downloads::isEpisodeDownloaded(episode))
              {
                  episodeActionsUI* tmp = new episodeActionsUI(true,dataBundle);
                  singleEntry = tmp->topBox;
                  gtk_container_add(GTK_CONTAINER(UIPVEpisodeList), singleEntry);
              }

              else
              {
                  episodeActionsUI* tmp = new episodeActionsUI(false,dataBundle);
                  singleEntry = tmp->topBox;
                  gtk_container_add(GTK_CONTAINER(UIPVEpisodeList), singleEntry);
              }
            }
        }
};

