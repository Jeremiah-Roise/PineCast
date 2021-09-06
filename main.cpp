#include <gtk/gtk.h>
#include <iostream>
#include <stdio.h>
#include <future>
#include <stdlib.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include "Libs/webTools.h"
#include "Libs/podcastDataTypes.h"
#include "Libs/PodcastMetaDataLists.h"
#include "Libs/DataTools.h"
#include "UINAMES.h"
#include "Libs/LibraryTools.h"
#include "Libs/caching.h"
#define goto  //please don't.
using namespace std;
#define oneDayInSeconds 86400
extern "C"
{
  void streamPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget* e);
  void DownloadAndPlayPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget* e);
  void createSearchResults(GtkWidget* container, PodcastMetaDataList x);
  void returnSelectionFromSearchResults(GtkWidget*, gpointer);
  void searchItunesWithText(GtkEntry* e);
  void getSelectedPodcastEpisodeButton(GtkWidget* e);
  void clearContainer(GtkContainer* e);
  void loadLib(PodcastMetaDataList& list);

  GtkWidget* CreateSearchEntry(PodcastMetaData);
  GdkPixbuf* createImage(string imageUrl, int scaleX, int scaleY);

  GtkWidget* searchListBox;
  GtkWidget* window;
  GtkWidget* mainStack;
  GtkWidget* notebook;
  GtkWidget* PodcastDetailsPage;

  //  PV means Preview
  GtkWidget* PVImage;
  GtkWidget* PVTitle;
  GtkWidget* PVAuthor;
  GtkWidget* PVEpisodeList;
  GtkWidget* LibraryUi;
  GtkWidget* addToLibraryButton;
  GtkWidget* DownloadsList;
  GtkBuilder* builder;
  GtkWidget* stackPage;
  GtkWidget* searchEntry;
  PodcastMetaData currentPodcast;
  PodcastMetaDataList searchList;
  PodcastMetaDataList Library;
  vector<podcastDataTypes::PodcastEpisode> Downloading;
  podcastDataTypes::episodeList currentepisodes;

  bool deleteMode = false;
  bool downloadPodcast = false;

  //  GUI setup
  int main(int argc, char **argv)
  {

    struct stat tmp;
    if (stat("Podcasts", &tmp) != 0 && S_ISDIR(tmp.st_mode) != 1)
    {
      mkdir("Podcasts", ACCESSPERMS);
    }

    //  TODO create thread with loadLib and mutexes for locking
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "PodcastWindow.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));

    //                                                            | these are macros|
    //                                                            | in UINAMES.h    |
    searchListBox =      GTK_WIDGET(gtk_builder_get_object(builder, searchListBoxName));
    searchEntry =        GTK_WIDGET(gtk_builder_get_object(builder, searchEntryName));
    mainStack =          GTK_WIDGET(gtk_builder_get_object(builder, mainStackName));
    notebook =           GTK_WIDGET(gtk_builder_get_object(builder, notebookName));
    PodcastDetailsPage = GTK_WIDGET(gtk_builder_get_object(builder, podcastDetailsPageName));
    LibraryUi =          GTK_WIDGET(gtk_builder_get_object(builder, LibraryUiName));
    PVImage =            GTK_WIDGET(gtk_builder_get_object(builder, PVImageName));
    PVTitle =            GTK_WIDGET(gtk_builder_get_object(builder, PVTitleName));
    PVAuthor =           GTK_WIDGET(gtk_builder_get_object(builder, PVAuthorName));
    PVEpisodeList =      GTK_WIDGET(gtk_builder_get_object(builder, PVEpisodeListName));
    addToLibraryButton = GTK_WIDGET(gtk_builder_get_object(builder, addToLibraryButtonName));
    DownloadsList =      GTK_WIDGET(gtk_builder_get_object(builder, "DownloadsList"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    loadLib(Library);
    gtk_widget_show(window);
    createSearchResults(LibraryUi, Library);
    gtk_main();
    return 0;
  }

  //  update podcasts in library
  void loadLib(PodcastMetaDataList &list)
  {
    cout << "LoadingLibrary" << endl;
    // open file To read
    string fileData = DataTools::getFile("Podcasts/MyPodcasts.xml");
    int index = 0;
    for (size_t i = 0; i < fileData.length(); i++)
    {

      string Podcast = DataTools::GetFieldP(fileData, "<Podcast>", "</Podcast>", index, index);
      if (Podcast == "")
      {
        break;
      }
      list.createAndAddPodcast(DataTools::GetField(Podcast, "<Artist=\"", "\">"),
                               DataTools::GetField(Podcast, "<RssFeed=\"", "\">"),
                               DataTools::GetField(Podcast, "<Title=\"", "\">"),
                               DataTools::GetField(Podcast, "<Image30=\"", "\">"),
                               DataTools::GetField(Podcast, "<Image60=\"", "\">"),
                               DataTools::GetField(Podcast, "<Image100=\"", "\">"),
                               DataTools::GetField(Podcast, "<Image600=\"", "\">"));
    }
    cout << "finished" << endl;
  }

  void deleteModeON()
  {
    deleteMode = true;
  }

  void deleteModeOFF()
  {
    deleteMode = false;
  }

  void deleteModeSwitch()
  {
    deleteMode = !deleteMode;
  }

  //  Destroy Function For GUI
  void on_MainWindow_destroy() { gtk_main_quit(); }

  //  for listing search results in a GtkListBox
  //  TODO figure out how to multithread this image download freezes window
  void createSearchResults(GtkWidget *container, PodcastMetaDataList x)
  {
    int size = x.GetIndexSize();
    if (size == 0)
    {
      return;
    } //  User Input Filtering
    //deleting old search results
    clearContainer(GTK_CONTAINER(container));

    for (int i = 0; i < size; i++)
    {
      PodcastMetaData tmp = x.GetPodcastAtIndex(i);
      string name = tmp.title;
      GtkWidget *result = CreateSearchEntry(tmp);

      gtk_container_add(GTK_CONTAINER(container), result);
      gtk_widget_show_all(result);
    }
  }

  GtkWidget* CreateSearchEntry(PodcastMetaData podcast)
  {

    GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget* thumbImage = gtk_image_new_from_pixbuf(createImage(podcast.image600, 50, 50));
    GtkWidget* titleLabel = gtk_label_new(podcast.title.c_str());
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(titleLabel), true);
    GtkWidget* previewButton = gtk_button_new_from_icon_name("open-menu-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_name(GTK_WIDGET(previewButton), (const gchar*)to_string(podcast.index).c_str());
    gtk_box_pack_start(GTK_BOX(topBox), thumbImage, false, false, 0);
    gtk_box_pack_start(GTK_BOX(topBox), titleLabel, false, false, 0);
    gtk_box_pack_end(GTK_BOX(topBox), previewButton, false, false, 0);
    g_signal_connect(previewButton, "released", (GCallback)returnSelectionFromSearchResults, (gpointer) nullptr);
    return topBox;
  }

  //  clears the given container of all children
  void clearContainer(GtkContainer* e)
  {
    gtk_container_foreach(e, (GtkCallback)gtk_widget_destroy, NULL);
  }

  //  create's an image from a url
  GdkPixbuf* createImage(string imageUrl, int scaleX, int scaleY)
  {
    string imagedata = webTools::getFileInMem(imageUrl); //  getting image data from web

    GdkPixbufLoader *test = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_set_size(test, scaleX, scaleY);
    gdk_pixbuf_loader_write(test, (const guchar*)imagedata.c_str(), imagedata.size(), nullptr);

    GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(test);
    gdk_pixbuf_loader_close(test, nullptr);

    return pixbuf;
  }

  /*
  ## Signal Functions Go beyond this comment ##
  */
  // get search text and give it to the itunes search function
  void searchItunesWithText(GtkEntry* e)
  {
    searchList = webTools::itunesSearch(gtk_entry_get_text(e));
    createSearchResults(searchListBox,searchList);
    cout << "after create search results" << endl;
    return;
  }

  void setPreviewPage(podcastDataTypes::episodeList episodes)
  {
    auto widgetBuilder = [](podcastDataTypes::episodeList data, int i)
    {
      string title = data.getEpisodeAtIndex(i).title.c_str();
      string duration = "<span size=\"medium\"><i>" + data.getEpisodeAtIndex(i).duration + "</i></span>";

      GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
      GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
      GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
      GtkWidget* playButton = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
      GtkWidget* downloadButton = gtk_button_new_from_icon_name("emblem-downloads", GTK_ICON_SIZE_BUTTON);
      GtkWidget* titleLabel = gtk_label_new(title.c_str());
      GtkWidget* durationLabel = gtk_label_new(duration.c_str());

      void (*streamfunc)(GtkWidget*) = [](GtkWidget* e)
      {
        downloadPodcast = false;
        getSelectedPodcastEpisodeButton(e);
      };
      void (*downloadfunc)(GtkWidget*) = [](GtkWidget* e)
      {
        downloadPodcast = true;
        getSelectedPodcastEpisodeButton(e);
      };

      g_signal_connect(playButton, "pressed", (GCallback)streamfunc, (gpointer) "button");
      g_signal_connect(downloadButton, "pressed", (GCallback)downloadfunc, (gpointer) "button");

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
      gtk_widget_set_name(GTK_WIDGET(playButton),to_string(i).c_str());
      gtk_widget_set_name(GTK_WIDGET(downloadButton),to_string(i).c_str());

      gtk_box_pack_start(GTK_BOX(topBox), infoBox, false, false, 0);
      gtk_box_pack_end(GTK_BOX(topBox), buttonBox, false, false, 0);
      gtk_widget_show_all(topBox);
      return topBox;
    };

    //  hide the add to library button if in the library
    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == 0)
    {
      gtk_widget_hide(addToLibraryButton);
    }
    if (page == 1)
    {
      gtk_widget_show(addToLibraryButton);
    }

    gtk_stack_set_visible_child(GTK_STACK(mainStack), PodcastDetailsPage);
    gtk_label_set_text(GTK_LABEL(PVTitle), currentPodcast.title.c_str());
    gtk_label_set_text(GTK_LABEL(PVAuthor), currentPodcast.artist.c_str());
    gtk_image_set_from_pixbuf(GTK_IMAGE(PVImage), createImage(currentPodcast.image600, 200, 200));

    //  list episodes
    currentepisodes = episodes;

    clearContainer(GTK_CONTAINER(PVEpisodeList));

    for (int i = 0; i < episodes.getIndexSize(); i++)
    {
      GtkWidget* eventBox = widgetBuilder(episodes, i);
      gtk_container_add(GTK_CONTAINER(PVEpisodeList), eventBox);
    }
    cout << "finished set previewPage function" << endl;
  }

  //  gets the returned selection from search results
  void returnSelectionFromSearchResults(GtkWidget* e, gpointer data)
  {

    int index = atoi(gtk_widget_get_name(e));

    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

    if (page == 0)
    {
      currentPodcast = Library.GetPodcastAtIndex(index);
    }

    if (page == 1)
    {
      currentPodcast = searchList.GetPodcastAtIndex(index);
    }

    if (page == 0 && deleteMode == true)
    {
      removeFromLibrary(currentPodcast);
      clearContainer(GTK_CONTAINER(LibraryUi));
      Library.clear();
      loadLib(Library);
      createSearchResults(LibraryUi, Library);
      return;
    }

    if (deleteMode == false && page == 0)
    {
      string rss;
      string fileName = currentPodcast.title + ".rss";

      //  get RSS file if cache does not exist or is out of date.
      bool isCacheOutOfDate = not (caching::isCacheFileValid(fileName.c_str(),oneDayInSeconds));
      if (isCacheOutOfDate)
      {
        rss = webTools::getFileInMem(currentPodcast.RssFeed);
        cout << "from web" << endl;
        caching::createCacheFile(fileName.c_str(),rss.c_str(),rss.size());
        setPreviewPage(DataTools::getEpisodes(rss));
        return;
      }
      else
      {
        cout << "from cache" << endl;
        string filepath = caching::getCachePath(fileName.c_str());
        rss = DataTools::getFile(filepath);
        setPreviewPage(DataTools::getEpisodes(rss));
        return;
      }
    }

    if (page == 1)
    {
      cout << "loading" << endl;
      string rss;

      //  check the state of the cachefile
      rss = webTools::getFileInMem(currentPodcast.RssFeed);
      setPreviewPage(DataTools::getEpisodes(rss));
      return;
    }
    cout << "after set preview" << endl;
  }

  void addPodcastToLibButton()
  {
    addToLibrary(currentPodcast);
    clearContainer(GTK_CONTAINER(LibraryUi));
    Library.clear();
    loadLib(Library);
    createSearchResults(LibraryUi, Library);
  }

  //  simply goes to the main page
  void goMainPage()
  {
    gtk_stack_set_visible_child_name(GTK_STACK(mainStack), (const gchar *)mainPageName);
  }

  void tabChanged(  GtkNotebook* self, GtkWidget* page, guint page_num, gpointer user_data){
    cout << page_num << endl;
    if (page_num == 1)
    {
      gtk_widget_grab_focus(searchEntry);
      cout << "run" << endl;
    }
  }



  /// function that gets called when an episode is clicked.
  void getSelectedPodcastEpisodeButton(GtkWidget* e)
  {
    podcastDataTypes::PodcastEpisode current = currentepisodes.getEpisodeAtIndex(atoi(gtk_widget_get_name(e)));
    //  check if the podcast is already being downloaded
    for (podcastDataTypes::PodcastEpisode download : Downloading)
    {
      if (download.mp3Link == current.mp3Link)
      {
        cout << "already downloading" << endl;
        return;
      }
    }

    if (downloadPodcast)
    {

      Downloading.push_back(current);
      thread th = thread(DownloadAndPlayPodcast, Downloading.back(), e);
      th.detach();
      return;
    }
    else
    {

      Downloading.push_back(current);
      thread th = thread(streamPodcast, Downloading.back(), e);
      th.detach();
      return;
    }
  }

  void playMp3(string name);
  /// Downloads entirely and then plays a podcast
  void DownloadAndPlayPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget* e)
  {

    g_signal_connect(e, "pressed", (GCallback)[]() { cout << "already downloading" << endl; }, (gpointer) "button");
    e = gtk_widget_get_parent(e);
    gtk_widget_hide(e);
    clearContainer(GTK_CONTAINER(e));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(box), gtk_label_new(podcast.title.data()));
    GtkWidget *progressBar = gtk_progress_bar_new();
    gtk_container_add(GTK_CONTAINER(box), progressBar);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0f);
    gtk_container_add(GTK_CONTAINER(e), box);
    gtk_widget_show_all(e);

    podcast.title += ".mp3";
    double progress = 0;
    const std::future<void> thread = std::async(std::launch::async, DownloadPodcast, podcast.mp3Link, podcast.title, &progress);

    while (thread.wait_for(0ms) != std::future_status::ready) // wait for download to finish
    {
      sleep(1);
      //if (GTK_IS_PROGRESS_BAR(progressBar)) // for some reason this was crashing the program when the download button was pressed in a particular podcast called "pinetalk" and then you switched to a different podcast preview
      //{
      //  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
      //}
      podcast.Download = progress;
      cout << "Download progress: " << podcast.Download << endl;
    }
    thread.wait();

    playMp3(podcast.title);
  }

  /// uses system command to start podcast with default application should be able to tolerate spaces.
  void playMp3(string name)
  {
    string FName = "xdg-open \"" + name + "\"";
    FName += " &";
    cout << "the name is: " << FName << endl;
    system(FName.data());
  }

  /// streams a podcast by waiting until it is %0.05 finished and then opens the audioplayer.
  ///
  /// uses playMp3 to start the audio player checks download progress once per second.
  void streamPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget* e)
  {
    //e = gtk_widget_get_parent(e);
    gtk_widget_hide(e);
    clearContainer(GTK_CONTAINER(e));

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(box), gtk_label_new(podcast.title.data()));
    GtkWidget* progressBar = gtk_progress_bar_new();
    gtk_container_add(GTK_CONTAINER(box), progressBar);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0f);
    gtk_container_add(GTK_CONTAINER(e), box);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0);

    gtk_widget_show_all(e);
    g_signal_handlers_destroy(e);
    g_signal_connect(e, "pressed", (GCallback)[]() { cout << "already downloading" << endl; }, (gpointer) "button");

    double progress;
    const std::future<void> thread = std::async(std::launch::async, DownloadPodcast, podcast.mp3Link, podcast.title + ".mp3", &progress);
    while (!(progress >= 0.05)) // wait for download to finish
    {
      sleep(1);
      //if (GTK_IS_PROGRESS_BAR(progressBar))
      //{
      //  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
      //}

      podcast.Download = progress;
      cout << "Download progress: " << podcast.Download << endl;
    }
    playMp3(podcast.title + ".mp3");
    while (thread.wait_for(0ms) != std::future_status::ready) // wait for download to finish
    {
      sleep(1);
      //if (GTK_IS_PROGRESS_BAR(progressBar)) // for some reason this was crashing the program when the stream button was pressed in a particular podcast called "pinetalk" and then you switched to a different podcast preview
      //{
      //  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
      //}
      podcast.Download = progress;
      cout << "Download progress: " << progress << endl;
    }

    thread.wait();
  }
}
