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
#include"Libs/UIFunctions.h"
#define goto  //please don't.
using namespace std;
#define oneDayInSeconds 86400

extern "C"
{
  void streamPodcast(PodcastEpisode podcast, GtkWidget* e);
  void DownloadAndPlayPodcast(PodcastEpisode podcast, GtkWidget* e);
  void createSearchResults(GtkWidget* container, PodcastDataList& x);
  void returnSelectionFromSearchResults(GtkWidget*, gpointer);
  void searchItunesWithText(GtkEntry* e);
  void getSelectedPodcastEpisodeButton(GtkWidget* e);
  void clearContainer(GtkContainer* e);
  
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
  PodcastEpisodeList DownloadedEpisodes;
  PodcastEpisodeList currentepisodes;
  bool deleteMode = false;  /// whether the library is set to delete selected podcast.



//  this function just sets up a bunch of global variables and checks that folders exist
void init(GtkBuilder* builder){
  string lcl = filepaths::lclFiles();
  if (filepaths::folderExists(lcl) == false)
  {
    mkdir(lcl.c_str(),ACCESSPERMS);
  }

  //                                                              | these are macros|
  //                                                              | in UINAMES.h    |
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
  Library::loadLib(Library);
  DownloadedEpisodes = Downloads::getDownloads();
  createSearchResults(UILibraryUi, Library);
}

///  GUI setup.
int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "PodcastWindow.glade", NULL);
  UIwindow = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));

  auto T1 = async(init, builder);

  gtk_widget_show(UIwindow);
  T1.wait();
  gtk_main();
  return 0;
}

  /// minor UI builder function that creates the Podcast result widget in the library and search pages.
  ///
  /// the main reason this isn't in a lambda is because it could be used by many other systems.
  GtkWidget* createResultWidget(PodcastData& podcast, size_t& index)
  {

    GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget* thumbImage = gtk_image_new_from_pixbuf(webTools::createImage(podcast.image600, 50, 50));
    GtkWidget* titleLabel = gtk_label_new(podcast.title.c_str());
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(titleLabel), true);
    GtkWidget* previewButton = gtk_button_new_from_icon_name("open-menu-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(topBox), thumbImage, false, false, 0);
    gtk_box_pack_start(GTK_BOX(topBox), titleLabel, false, false, 0);
    gtk_box_pack_end(GTK_BOX(topBox), previewButton, false, false, 0);
    podcast.index = index;
    g_signal_connect(previewButton, "released", (GCallback)returnSelectionFromSearchResults, (gpointer) &podcast.index);
    return topBox;
  }

  /// for listing search results in a GtkListBox or GtkFlowbox.
  void createSearchResults(GtkWidget *container, PodcastDataList& podcastWidgetsToCreate)
  {
    int size = podcastWidgetsToCreate.size();
    if (size == 0)
    {
      return;
    } //  User Input Filtering
    //deleting old search results
    clearContainer(GTK_CONTAINER(container));

    for (PodcastData& i:podcastWidgetsToCreate)
    {
      GtkWidget *result = createResultWidget(i,i.index);

      gtk_container_add(GTK_CONTAINER(container), result);
      gtk_widget_show_all(result);
    }
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
    //  hide the add to library button if in the library
    stackPage page = (stackPage)gtk_notebook_get_current_page(GTK_NOTEBOOK(UInotebook));

    if (page == 0)
      gtk_widget_hide(UIaddToLibraryButton);

    if (page == 1)
      gtk_widget_show(UIaddToLibraryButton);

    gtk_stack_set_visible_child(GTK_STACK(UImainStack), UIPodcastDetailsPage);
    gtk_label_set_text(GTK_LABEL(UIPVTitle), currentPodcast.title.c_str());
    gtk_label_set_text(GTK_LABEL(UIPVAuthor), currentPodcast.artist.c_str());
    gtk_image_set_from_pixbuf(GTK_IMAGE(UIPVImage), webTools::createImage(currentPodcast.image600, 200, 200));

    //  list episodes
    currentepisodes = episodes;

    clearContainer(GTK_CONTAINER(UIPVEpisodeList));
    GtkWidget* singleEntry;
    for (PodcastEpisode& episode:currentepisodes)
    {
      PodcastDataBundle dataBundle;
      dataBundle.Episode = episode;
      dataBundle.Podcast = currentPodcast;
      if (Downloads::isEpisodeDownloaded(episode))
      {
      singleEntry = downloadedWidgetBuilder(dataBundle,playPodcastButtonFunc,deletePodcast);
      gtk_container_add(GTK_CONTAINER(UIPVEpisodeList), singleEntry);
      }
      else
      {
      singleEntry = standardWidgetBuilder(dataBundle,streamPodcast,downloadPodcast);
      gtk_container_add(GTK_CONTAINER(UIPVEpisodeList), singleEntry);
      }
    }
  }

  ///  gets the returned selection from search results
  void returnSelectionFromSearchResults(GtkWidget* e, gpointer PodcastIndex)
  {
    int index = *(int*)PodcastIndex;
    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(UInotebook));

    if (page == 0)
      currentPodcast = Library.at(index);

    if (page == 1)
      currentPodcast = searchList.at(index);

    if (page == 0 && deleteMode == true)
    {
      Library::removeFromLibrary(currentPodcast);
      clearContainer(GTK_CONTAINER(UILibraryUi));
      Library.clear();
      Library::loadLib(Library);
      createSearchResults(UILibraryUi, Library);
      return;
    }

    if (page == 0 && deleteMode == false)
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

    if (page == stackPage::LibraryPage)
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


  ///  simply goes to the main page.
  void goMainPage()
  {
    gtk_stack_set_visible_child_name(GTK_STACK(UImainStack), (const gchar *)mainPageName);
  }

  ///  monitors for when tabs change in the main UInotebook.
  void tabChanged(  GtkNotebook* self, GtkWidget* page, guint page_num, gpointer user_data){
    cout << page_num << endl;

    //  if the page is equal to the search page focus the search bar.
    if (page_num == stackPage::LibraryPage)
    {
      gtk_widget_grab_focus(UIsearchEntry);
      cout << "setting focus to search bar" << endl;
    }
  }

  ///  adds the podcast to the library.
  void addPodcastToLibButton()
  {
    Library::addToLibrary(currentPodcast);
    clearContainer(GTK_CONTAINER(UILibraryUi));
    Library.clear();
    Library::loadLib(Library);
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
