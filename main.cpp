#include <gtk/gtk.h>
#include <iostream>
#include <stdio.h>
#include <future>
#include <stdlib.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <dbus/dbus.h>
#include <stdlib.h>
#include "Libs/webTools.h"
#include "Libs/podcastDataTypes.h"
#include "Libs/PodcastMetaDataLists.h"
#include "Libs/DataTools.h"
#include "UINAMES.h"
#include "Libs/LibraryTools.h"
using namespace std;

extern "C"
{
  void DownloadAndPlayPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget *e);
  GtkWidget *CreateSearchEntry(PodcastMetaData);
  void createSearchResults(GtkWidget *e, PodcastMetaDataList x);
  void returnSelection(GtkWidget *, gpointer);
  void PodcastSearchEntry(GtkEntry *e);
  void getSelectedPodcastEpisodeButton(GtkWidget *e);
  void clearContainer(GtkContainer *e);
  void loadLib(PodcastMetaDataList &list);
  GdkPixbuf *createImage(string imageUrl, int scaleX, int scaleY);
  GtkWidget *searchListBox;
  GtkWidget *window;
  GtkWidget *mainStack;
  GtkWidget *notebook;
  GtkWidget *PodcastDetailsPage;
  //  PV means Preview
  GtkWidget *PVImage;
  GtkWidget *PVTitle;
  GtkWidget *PVAuthor;
  GtkWidget *PVEpisodeList;
  GtkWidget *LibraryUi;
  GtkWidget *addToLibraryButton;
  GtkWidget *DownloadsList;
  GtkBuilder *builder;
  PodcastMetaData currentPodcast;
  PodcastMetaDataList searchList;
  PodcastMetaDataList Library;
  vector<podcastDataTypes::PodcastEpisode> Downloading;
  podcastDataTypes::episodeList currentepisodes;
  GtkWidget *stackPage = 0;
  bool deleteMode = false;
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
    int size = x.GetIndex();
    if (size == 0)
    {
      return;
    } //  User Input Filtering
    searchList = x;
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

  GtkWidget *CreateSearchEntry(PodcastMetaData podcast)
  {

    //  Image stuff
    GtkWidget *image = gtk_image_new_from_pixbuf(createImage(podcast.image600, 50, 50));
    //  ###########

    //  Formatting and setting up signals
    GtkWidget *label = gtk_label_new(podcast.title.c_str());
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *event = gtk_event_box_new();
    gtk_label_set_line_wrap(GTK_LABEL(label), true);

    g_signal_connect(event, "button-press-event", (GCallback)returnSelection, (gpointer) nullptr);
    gtk_widget_set_name(GTK_WIDGET(event), (const gchar *)to_string(podcast.index).c_str()); //  setting the name to the index of the podcast
    gtk_box_pack_start(GTK_BOX(box), image, false, true, 0);
    gtk_box_pack_start(GTK_BOX(box), label, true, true, 0);
    gtk_container_add(GTK_CONTAINER(event), GTK_WIDGET(box));
    gtk_widget_show_all(event);
    //  #################################
    return event;
  }

  //  clears the given container of all children
  void clearContainer(GtkContainer *e)
  {
    gtk_container_foreach(e, (GtkCallback)gtk_widget_destroy, NULL);
  }

  //  create's an image from a url
  GdkPixbuf *createImage(string imageUrl, int scaleX, int scaleY)
  {
    string imagedata = webTools::getFileInMem(imageUrl); //  getting image data from web

    GdkPixbufLoader *test = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_set_size(test, scaleX, scaleY);
    gdk_pixbuf_loader_write(test, (const guchar *)imagedata.c_str(), imagedata.size(), nullptr);

    GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(test);
    gdk_pixbuf_loader_close(test, nullptr);

    return pixbuf;
  }

  /*
  ## Signal Functions Go beyond this comment ##
  */
static void check_and_abort(DBusError *error) {
    if (!dbus_error_is_set(error)) return;
    puts(error->message);
    abort();
}
  // get search text and give it to the itunes search function
  void PodcastSearchEntry(GtkEntry *e)
  {
    DBusConnection *connection = NULL;
    DBusError error;
    DBusMessage *msgQuery = NULL;
    DBusMessage *msgReply = NULL;
    const char *interfaceName = NULL;
    const char *versionValue = NULL;
 
    dbus_error_init(&error);
    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    check_and_abort(&error);
 
    interfaceName = "sm.puri.OSK0";
 
    msgQuery = dbus_message_new_method_call(
        interfaceName,
        "/sm/puri/OSK0",
        "sm.puri.OSK0",
        "SetVisible");
        DBusMessageIter iter,subIter;
        dbus_message_iter_init_append(msgQuery, &iter);

        //HOW TO CALL VARIANT?:
        dbus_message_iter_open_container(&iter, DBUS_TYPE_BOOLEAN, DBUS_TYPE_BOOLEAN_AS_STRING, &subIter);
        dbus_message_iter_append_basic(&subIter, DBUS_TYPE_BOOLEAN,(void*)true);
        dbus_message_iter_close_container(&iter, &subIter);

    msgReply = dbus_connection_send_with_reply_and_block(connection, msgQuery, 1000, &error);
    check_and_abort(&error);
    dbus_message_unref(msgQuery);
 
    dbus_message_get_args(msgReply, &error, DBUS_TYPE_STRING, &versionValue, DBUS_TYPE_INVALID);
 
    printf("The critical action is: %s\n", versionValue);
     
    dbus_message_unref(msgReply);
     

    std::future<void> loadList = std::async(std::launch::async, &createSearchResults, searchListBox, webTools::itunesSearch(gtk_entry_get_text(e)));
    //createSearchResults(listBox,webTools::itunesSearch(gtk_entry_get_text(e)));
    return;
  }

  void setPreviewPage(podcastDataTypes::episodeList episodes)
  {
    auto tmpFunc = [] (podcastDataTypes::episodeList data,int i) {
      GtkWidget *eventBox = gtk_event_box_new();
        GtkWidget *label = gtk_label_new(data.getEpisodeAtIndex(i).title.c_str());
        gtk_widget_set_margin_top(GTK_WIDGET(label), 10);
        GtkWidget *box = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0);
        gtk_label_set_line_wrap(GTK_LABEL(label), true);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0);
        gtk_container_add(GTK_CONTAINER(box), label);
        gtk_container_add(GTK_CONTAINER(eventBox), box);
        gtk_widget_set_name(eventBox, (gchar *)to_string(i).c_str());
        gtk_widget_show_all(eventBox);
        return eventBox;
    };


    
    gtk_stack_set_visible_child(GTK_STACK(mainStack), PodcastDetailsPage);
    gtk_label_set_text(GTK_LABEL(PVTitle), currentPodcast.title.c_str());
    gtk_label_set_text(GTK_LABEL(PVAuthor), currentPodcast.artist.c_str());
    gtk_image_set_from_pixbuf(GTK_IMAGE(PVImage), createImage(currentPodcast.image600, 200, 200));

    //  list episodes
    currentepisodes = episodes;
    clearContainer(GTK_CONTAINER(PVEpisodeList));
    for (int i = 0; i < episodes.getIndexSize(); i++)
    {
      bool download = true;
      if (Downloading.size() > 0)
      {
      for (auto tmpPod : Downloading)
      {
        if (tmpPod.mp3Link == episodes.getEpisodeAtIndex(i).mp3Link)
        {
          download = false;
        }
      }
      }
      if (download == true)
      {
        GtkWidget* eventBox = tmpFunc(episodes,i);
        g_signal_connect(eventBox, "button-press-event", (GCallback)getSelectedPodcastEpisodeButton, (gpointer) "button");
        gtk_container_add(GTK_CONTAINER(PVEpisodeList), eventBox);
      }
      if (download == false)
      {
        GtkWidget* eventBox = tmpFunc(episodes,i);
        g_signal_connect(eventBox, "button-press-event", (GCallback)[](){cout<<"already downloading"<<endl;}, (gpointer) "button");
        gtk_container_add(GTK_CONTAINER(PVEpisodeList), eventBox);
      }
    }
}

  //  gets the returned selection from search results
  void returnSelection(GtkWidget *e, gpointer data)
  {

    int index = atoi(gtk_widget_get_name(e));

    searchList.GetPodcastAtIndex(index, currentPodcast);
    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page == 0 || page == 1)
    {
      Library.GetPodcastAtIndex(index, currentPodcast);
    }
    if (page == 1)
    {
      searchList.GetPodcastAtIndex(index, currentPodcast);
    }

    if (page == 0 && deleteMode == true)
    {
      removeFromLibrary(currentPodcast);
      clearContainer(GTK_CONTAINER(LibraryUi));
      Library.clear();
      loadLib(Library);
      createSearchResults(LibraryUi, Library);
    }

    if (deleteMode == false && page == 0)
    {
      string rss;
      struct stat tmp;
      string path = "/tmp/" + currentPodcast.title + ".rss";
      //  remove spaces from the path
      std::remove(path.begin(), path.end(), ' ');
      //  check the state of the cachefile
      int cacheFile = stat(path.c_str(), &tmp);
      cout << tmp.st_atim.tv_sec + 86400 << endl;

      // getting epoch time for comparison against the cache file creation date incremented by one day
      cout << cacheFile << endl;
      double now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      if (cacheFile == -1 || (cacheFile == 0 && tmp.st_atim.tv_sec + 86400 <= now))
      {
        rss = webTools::getFileInMem(currentPodcast.RssFeed);
        cout << "from web" << endl;
        fstream file;
        file.open(path.c_str(), ios_base::out);
        file.write(rss.c_str(), rss.size());
        file.close();
        utime(path.c_str(),NULL);
      }
      else
      {
        rss = DataTools::getFile(path);
        cout << "from cache" << endl;
      }

      setPreviewPage(DataTools::getEpisodes(rss));
    }
    if (deleteMode == false && page == 1)
    {
      string rss;
      //  check the state of the cachefile
      rss = webTools::getFileInMem(currentPodcast.RssFeed);

      setPreviewPage(DataTools::getEpisodes(rss));
    }
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

  /// function that gets called when an episode is clicked.
  void streamPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget *e);
  void getSelectedPodcastEpisodeButton(GtkWidget *e)
  {
    podcastDataTypes::PodcastEpisode current = currentepisodes.getEpisodeAtIndex(atoi(gtk_widget_get_name(e)));
    if (/*some nonexistant variable*/ false)
    {
      
      Downloading.push_back(current);
      thread th = thread(DownloadAndPlayPodcast,Downloading.back(), e);
      th.detach();
      return;
    }
    else
    {
      
      Downloading.push_back(current);
      thread th = thread(streamPodcast,Downloading.back(), e);
      th.detach();
      return;
    }
  }

  void playMp3(string name);
  /// Downloads entirely and then plays a podcast
  void DownloadAndPlayPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget *e)
  {

    
    e = gtk_widget_get_parent(e);
    gtk_widget_hide(e);
    clearContainer(GTK_CONTAINER(e));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(box), gtk_label_new(podcast.title.data()));
    GtkWidget *progressBar = gtk_progress_bar_new();
    gtk_container_add(GTK_CONTAINER(box), progressBar);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0f);
    gtk_container_add(GTK_CONTAINER(e), box);
    g_signal_connect(e, "button-press-event", (GCallback)[](){cout<<"already downloading"<<endl;}, (gpointer) "button");
    gtk_widget_show_all(e);
    
    podcast.title += ".mp3";
    double progress = 0;
    const std::future<void> thread = std::async(std::launch::async, DownloadPodcast, podcast.mp3Link, podcast.title, &progress);

    while (thread.wait_for(0ms) != std::future_status::ready) // wait for download to finish
    {
      sleep(1);
      if (GTK_IS_PROGRESS_BAR(progressBar))
      {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
      }
      podcast.Download = progress;
      cout << "Download progress: " << progress << endl;

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
  void streamPodcast(podcastDataTypes::PodcastEpisode podcast, GtkWidget *e)
  {
    //e = gtk_widget_get_parent(e);
    gtk_widget_hide(e);
    clearContainer(GTK_CONTAINER(e));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(box), gtk_label_new(podcast.title.data()));
    GtkWidget *progressBar = gtk_progress_bar_new();
    gtk_container_add(GTK_CONTAINER(box), progressBar);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0f);
    gtk_container_add(GTK_CONTAINER(e), box);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),0);
    
    gtk_widget_show_all(e);
    g_signal_handlers_destroy(e);
    g_signal_connect(e, "button-press-event", (GCallback)[](){cout<<"already downloading"<<endl;}, (gpointer) "button");
    

    double progress;
    const std::future<void> thread = std::async(std::launch::async, DownloadPodcast, podcast.mp3Link, podcast.title + ".mp3", &progress);
    while (!(progress >= 0.05)) // wait for download to finish
    {
      sleep(1);
      if (GTK_IS_PROGRESS_BAR(progressBar))
      {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
      }
      
      podcast.Download = progress;
      cout << "Download progress: " << progress << endl;
    }
    playMp3(podcast.title + ".mp3");
    while (thread.wait_for(0ms) != std::future_status::ready) // wait for download to finish
    {
      sleep(1);
      if (GTK_IS_PROGRESS_BAR(progressBar))
      {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
      }
      podcast.Download = progress;
      cout << "Download progress: " << progress << endl;
    }

    thread.wait();
  }
}
