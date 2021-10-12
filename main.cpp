#include </usr/include/gtk-3.0/gtk/gtk.h>
#include <iostream>
#include <stdio.h>
#include <future>
#include <stdlib.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include "Libs/Libs.h"
#include "UINAMES.h"
#define goto  //please don't.
using namespace std;
#define oneDayInSeconds 86400
extern "C"
{
  void streamPodcast(PodcastEpisode podcast, GtkWidget* e);
  void DownloadAndPlayPodcast(PodcastEpisode podcast, GtkWidget* e);
  void createSearchResults(GtkWidget* container, PodcastDataList x);
  void returnSelectionFromSearchResults(GtkWidget*, gpointer);
  void searchItunesWithText(GtkEntry* e);
  void getSelectedPodcastEpisodeButton(GtkWidget* e);
  void clearContainer(GtkContainer* e);
  
  GtkWidget* createResultWidget(PodcastData);
  GtkWidget* UIsearchListBox;
  GtkWidget* UIwindow;
  GtkWidget* UImainStack;
  GtkWidget* UInotebook;
  GtkWidget* UIPodcastDetailsPage;

  //  PV means Preview
  GtkWidget* UIPVImage;
  GtkWidget* UIPVTitle;
  GtkWidget* UIPVAuthor;
  GtkWidget* UIPVEpisodeList;
  GtkWidget* UILibraryUi;
  GtkWidget* UIaddToLibraryButton;
  GtkWidget* UIDownloadsList;
  GtkWidget* UIstackPage;
  GtkWidget* UIsearchEntry;
  GtkBuilder* builder;
  PodcastData currentPodcast;
  PodcastDataList searchList;
  PodcastDataList Library;
  PodcastEpisodeList Downloading;
  PodcastEpisodeList DownloadedEpisodes;
  PodcastEpisodeList currentepisodes;
  bool deleteMode = false;  /// whether the library is set to delete selected podcast.
  bool downloadPodcast = false; /// select whether to download or stream podcasts.



int Show_Splash_Screen(int time)
{
  GtkWidget *window;
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window, -1, -1);
  gtk_window_set_decorated(GTK_WINDOW (window), FALSE);
  gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
  gtk_widget_show_all (window);
  g_timeout_add (time, [](gpointer data){gtk_widget_destroy((GtkWidget*)data); gtk_main_quit (); return(FALSE);}, window); gtk_main ();
  return 0;
}





  ///  GUI setup.
  int main(int argc, char **argv)
  {

    gtk_init(&argc, &argv);
    
    Show_Splash_Screen(3000);




    string lcl = filepaths::lclFiles();
    if (filepaths::folderExists(lcl) == false)
    {
      mkdir(lcl.c_str(),ACCESSPERMS);
    }

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "PodcastWindow.glade", NULL);
    UIwindow = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
    //                                                            | these are macros|
    //                                                            | in UINAMES.h    |
    UIsearchListBox =      GTK_WIDGET(gtk_builder_get_object(builder, searchListBoxName));
    UIsearchEntry =        GTK_WIDGET(gtk_builder_get_object(builder, searchEntryName));
    UImainStack =          GTK_WIDGET(gtk_builder_get_object(builder, mainStackName));
    UInotebook =           GTK_WIDGET(gtk_builder_get_object(builder, notebookName));
    UIPodcastDetailsPage = GTK_WIDGET(gtk_builder_get_object(builder, podcastDetailsPageName));
    UILibraryUi =          GTK_WIDGET(gtk_builder_get_object(builder, LibraryUiName));
    UIPVImage =            GTK_WIDGET(gtk_builder_get_object(builder, PVImageName));
    UIPVTitle =            GTK_WIDGET(gtk_builder_get_object(builder, PVTitleName));
    UIPVAuthor =           GTK_WIDGET(gtk_builder_get_object(builder, PVAuthorName));
    UIPVEpisodeList =      GTK_WIDGET(gtk_builder_get_object(builder, PVEpisodeListName));
    UIaddToLibraryButton = GTK_WIDGET(gtk_builder_get_object(builder, addToLibraryButtonName));
    UIDownloadsList =      GTK_WIDGET(gtk_builder_get_object(builder, "DownloadsList"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    loadLib(Library);
    createSearchResults(UILibraryUi, Library);
    gtk_widget_show(UIwindow);
    gtk_main();
    return 0;
  }


  /// for listing search results in a GtkListBox or GtkFlowbox.
  void createSearchResults(GtkWidget *container, PodcastDataList x)
  {
    int size = x.size();
    if (size == 0)
    {
      return;
    } //  User Input Filtering
    //deleting old search results
    clearContainer(GTK_CONTAINER(container));

    for (int i = 0; i < size; i++)
    {
      PodcastData tmp = x.at(i);
      string name = tmp.title;
      GtkWidget *result = createResultWidget(tmp);

      gtk_container_add(GTK_CONTAINER(container), result);
      gtk_widget_show_all(result);
    }
  }
  
  /// minor UI builder function that creates the Podcast result widget in the library and search pages.
  ///
  /// the main reason this isn't in a lambda is because it could be used by many other systems.
  GtkWidget* createResultWidget(PodcastData podcast)
  {

    GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget* thumbImage = gtk_image_new_from_pixbuf(webTools::createImage(podcast.image600, 50, 50));
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


  /// get search text and give it to the itunes search function
  ///
  /// linked directly to the UI in glade
  void searchItunesWithText(GtkEntry* e)
  {
    searchList = webTools::itunesSearch(gtk_entry_get_text(e));
    createSearchResults(UIsearchListBox,searchList);
    return;
  }
  
  /// when this is called it initializes the preview page.
  ///
  /// when called it uses the global currentPodcast variable to get the Podcast title, image, artist, etc,
  /// and iterates through the episodes argument to create the episodes,
  /// should probably be updated to not use global variables.
  void setPreviewPage(PodcastEpisodeList episodes)
  {
    auto widgetBuilder = [](PodcastEpisodeList data, int i)
    { 
      string title = data.at(i).title.c_str();
      string duration = "<span size=\"medium\"><i>" + data.at(i).duration + "</i></span>";

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

      g_signal_connect(playButton, "released", (GCallback)streamfunc, (gpointer) "button");
      g_signal_connect(downloadButton, "released", (GCallback)downloadfunc, (gpointer) "button");

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
    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(UInotebook));
    if (page == 0)
    {
      gtk_widget_hide(UIaddToLibraryButton);
    }
    if (page == 1)
    {
      gtk_widget_show(UIaddToLibraryButton);
    }

    gtk_stack_set_visible_child(GTK_STACK(UImainStack), UIPodcastDetailsPage);
    gtk_label_set_text(GTK_LABEL(UIPVTitle), currentPodcast.title.c_str());
    gtk_label_set_text(GTK_LABEL(UIPVAuthor), currentPodcast.artist.c_str());
    gtk_image_set_from_pixbuf(GTK_IMAGE(UIPVImage), webTools::createImage(currentPodcast.image600, 200, 200));

    //  list episodes
    currentepisodes = episodes;

    clearContainer(GTK_CONTAINER(UIPVEpisodeList));

    for (size_t i = 0; i < episodes.size(); i++)
    {
      GtkWidget* singleEntry = widgetBuilder(episodes, i);
      gtk_container_add(GTK_CONTAINER(UIPVEpisodeList), singleEntry);
    }
  }

  ///  gets the returned selection from search results
  void returnSelectionFromSearchResults(GtkWidget* e, gpointer data)
  {

    int index = atoi(gtk_widget_get_name(e));

    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(UInotebook));

    if (page == 0)
    {
      currentPodcast = Library.at(index);
    }

    if (page == 1)
    {
      currentPodcast = searchList.at(index);
    }

    if (page == 0 && deleteMode == true)
    {
      removeFromLibrary(currentPodcast);
      clearContainer(GTK_CONTAINER(UILibraryUi));
      Library.clear();
      loadLib(Library);
      createSearchResults(UILibraryUi, Library);
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

 void playMp3(string name);
void streamPodcastMonitor(double prg, string file){
  cout << prg << endl;
  if (prg >= 0.0500 && prg <= 0.059)
  {
    playMp3(file);
  }
}

void downloadPodcastMonitor(double prg, string file){
  cout << prg << endl;
  if (prg >= 1)
  {
    playMp3(file);
  }
}


  /// function that gets called when an episode is clicked.
  ///
  /// takes in the selected episode by getting it's index from the name of the widget
  void getSelectedPodcastEpisodeButton(GtkWidget* e)
  {
    PodcastEpisode current = currentepisodes.at(atoi(gtk_widget_get_name(e)));
    //  check if the podcast is already being downloaded.
    for (PodcastEpisode download : Downloading)
    {
      if (download.mp3Link == current.mp3Link)
      {
        cout << "already downloading" << endl;
        return;
      }
    }

    if (downloadPodcast)
    {
      DownloadAndPlayPodcast(current,e);
    }
    else
    {
      streamPodcast(current,e);
    }
  }

 
  /// Downloads entirely and then plays a podcast.
  ///
  /// creates the download bar widget and updates it with the current progress,
  /// it's very jenky but it works.
  void DownloadAndPlayPodcast(PodcastEpisode podcast, GtkWidget* e)
  {
      Downloading.push_back(podcast);
      string filePath = filepaths::lclFiles();
      filePath += DataTools::cleanString(currentPodcast.title);
      filePath += "/"+DataTools::cleanString(Downloading.back().title)+".mp3";

      std::thread downThread = std::thread(webTools::DownloadPodcast, Downloading.back().mp3Link, filePath, downloadPodcastMonitor);
      downThread.detach();
      return;
  }



  /// streams a podcast by waiting until it is %0.05 finished and then opens the audioplayer.
  ///
  /// uses playMp3 to start the audio player checks download progress once per second.
  void streamPodcast(PodcastEpisode podcast, GtkWidget* e)
  {
      Downloading.push_back(podcast);
      string filePath = filepaths::lclFiles();
      filePath += DataTools::cleanString(currentPodcast.title);
      filePath += "/"+DataTools::cleanString(Downloading.back().title)+".mp3";

      std::thread downThread = std::thread(webTools::DownloadPodcast, Downloading.back().mp3Link, filePath, streamPodcastMonitor);
      downThread.detach();
  }



  ///  sets the library's delete mode to true.
  void deleteModeON()
  {
    deleteMode = true;
  }

  ///  sets the library's delete mode to false.
  void deleteModeOFF()
  {
    deleteMode = false;
  }

  ///  toggles the library's delete mode.
  void deleteModeSwitch()
  {
    deleteMode = !deleteMode;
  }

  /// uses system command to start podcast with default application should be able to tolerate spaces.
  void playMp3(string path)
  {
    string FName = "xdg-open \"" + path + "\" &";
    cout << "the command is: " << FName << endl;
    system(FName.data());
  }

  ///  simply goes to the main page.
  void goMainPage()
  {
    gtk_stack_set_visible_child_name(GTK_STACK(UImainStack), (const gchar *)mainPageName);
  }

  ///  monitors for when tabs change in the main UInotebook.
  void tabChanged(  GtkNotebook* self, GtkWidget* page, guint page_num, gpointer user_data){
    cout << page_num << endl;

    //  if the page is equal to the search page focus the search bar.
    if (page_num == 1)
    {
      gtk_widget_grab_focus(UIsearchEntry);
      cout << "setting focus to search bar" << endl;
    }
  }

  ///  adds the podcast to the library.
  void addPodcastToLibButton()
  {
    addToLibrary(currentPodcast);
    clearContainer(GTK_CONTAINER(UILibraryUi));
    Library.clear();
    loadLib(Library);
    createSearchResults(UILibraryUi, Library);

    auto cache = [](){
      string name = currentPodcast.title + ".rss";

      string data = webTools::getFileInMem(currentPodcast.RssFeed);
      caching::createCacheFile(name.c_str(),data.c_str(),data.size());
    };

    thread cacheThread(cache);
    cacheThread.detach();
  }

  ///  clears the given container of all children
  void clearContainer(GtkContainer* e)
  {
    gtk_container_foreach(e, (GtkCallback)gtk_widget_destroy, NULL);
  }

}
