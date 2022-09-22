#include <gtk-3.0/gtk/gtk.h>
#include <iostream>
#include <stdio.h>
#include <future>
#include <stdlib.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include "Libs/AudioPlayer.h"
#include "Libs/Libs.h"
#include "UINAMES.h"
#include "Libs/PreviewPageClass.h"
#include "Libs/UIFunctions.h"
#include "Libs/PodcastsStore.h"
#define goto  //please don't.
using namespace std;
#define oneDayInSeconds 86400

extern "C"
{
  void streamPodcast(PodcastEpisode podcast, GtkWidget* e);
  void DownloadAndPlayPodcast(PodcastEpisode podcast, GtkWidget* e);
  void createSearchResults(GtkWidget* container, PodcastDataList& x);
  void returnSelection(GtkWidget*, gpointer);
  void searchItunesWithText(GtkEntry* e);
  void getSelectedPodcastEpisodeButton(GtkWidget* e);
  
  GtkWidget* UIsearchListBox;
  GtkWidget* UIwindow;
  GtkWidget* UImainStack;
  GtkWidget* UInotebook;
  GtkWidget* UIPVPodcastDetailsPage;

  //  PV means Preview
  GtkWidget* UIPVImage;
  GtkWidget* UIPVTitle;
  GtkWidget* UIPVAuthor;
  GtkWidget* UIPVEpisodeList;
  GtkWidget* UILibraryUi;
  GtkWidget* UIPVaddToLibraryButton;
  GtkWidget* UIDownloadsList;
  GtkWidget* UIstackPage;
  GtkWidget* UIsearchEntry;
  GtkBuilder* builder;
  AudioPlayer* player;
  PodcastData currentPodcast;
  PodcastDataList searchList;
  PodcastDataList Library;
  PodcastEpisodeList DownloadedEpisodes;
  PodcastEpisodeList currentepisodes;
  PreviewPageClass* PreviewPage;
  bool deleteMode = false;  /// whether the library is set to delete selected podcast.
  PodcastsStore store;

//  this function just sets up a bunch of global variables and checks that folders exist
void init(GtkBuilder* builder){
  string lcl = filepaths::lclFiles();
  if (filepaths::folderExists(lcl) == false)
  {
    mkdir(lcl.c_str(),ACCESSPERMS);
  }
  player = new AudioPlayer();
  PreviewPage = new PreviewPageClass(builder,player);

  //                                                              |    these are macros     |
  //                                                              |    in UINAMES.h         |
  UIPVImage =            GTK_WIDGET(gtk_builder_get_object(builder, PVImageName));
  UIPVTitle =            GTK_WIDGET(gtk_builder_get_object(builder, PVTitleName));
  UInotebook =           GTK_WIDGET(gtk_builder_get_object(builder, notebookName));
  UIPVAuthor =           GTK_WIDGET(gtk_builder_get_object(builder, PVAuthorName));
  UIDownloadsList =      GTK_WIDGET(gtk_builder_get_object(builder, downloadsList));
  UImainStack =          GTK_WIDGET(gtk_builder_get_object(builder, mainStackName));
  UILibraryUi =          GTK_WIDGET(gtk_builder_get_object(builder, LibraryUiName));
  UIsearchEntry =        GTK_WIDGET(gtk_builder_get_object(builder, searchEntryName));
  UIsearchListBox =      GTK_WIDGET(gtk_builder_get_object(builder, searchListBoxName));
  UIPVEpisodeList =      GTK_WIDGET(gtk_builder_get_object(builder, PVEpisodeListName));
  UIPVaddToLibraryButton = GTK_WIDGET(gtk_builder_get_object(builder, addToLibraryButtonName));
  UIPVPodcastDetailsPage = GTK_WIDGET(gtk_builder_get_object(builder, podcastDetailsPageName));
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
  store = PodcastsStore();

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
    g_signal_connect(previewButton, "released", (GCallback)returnSelection, (gpointer) &podcast.index);
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
  /// linked directly to the UI xml
  void searchItunesWithText(GtkEntry* e)
  {
    searchList = store.itunesSearch(gtk_entry_get_text(e));
    createSearchResults(UIsearchListBox,searchList);
    return;
  }
  void deletePodcastFromLibrary(PodcastData Podcast){
      Library::removeFromLibrary(currentPodcast);
      clearContainer(GTK_CONTAINER(UILibraryUi));
      Library.clear();
      Library::loadLib(Library);
      createSearchResults(UILibraryUi, Library);
      return;
  }

  void displayPodcastDetails(PodcastData Podcast)
  {
    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(UInotebook));

    string rss;
    string fileName = currentPodcast.title + ".rss";

    cout << "loading ";
    //  get RSS file if cache does not exist or is out of date.
    if (page == stackPage::LibraryPage)
    {
      if (PreviewPage->lastViewedPodcast.title != currentPodcast.title)
      {
        if (caching::isCacheFileValid(fileName.c_str(),oneDayInSeconds))
        {
          cout << "from cache" << endl;
          string filepath = caching::getCachePath(fileName.c_str());
          rss = DataTools::getFile(filepath);
        }
        else
        {
          cout << "from web" << endl;
          rss = webTools::getFileInMem(currentPodcast.RssFeed);
          caching::createCacheFile(fileName.c_str(),rss.c_str(),rss.size());
        }
        rss = webTools::getFileInMem(currentPodcast.RssFeed);
      }

      //  check the state of the cachefile
      PreviewPage->setPreviewPage(DataTools::getEpisodes(rss),currentPodcast,true);
      gtk_stack_set_visible_child(GTK_STACK(UImainStack),UIPVPodcastDetailsPage);
      return;
    }

    if (page == stackPage::SearchPage)
    {
      cout << "from store" << endl;
      //  check the state of the cachefile
      rss = webTools::getFileInMem(currentPodcast.RssFeed);
      PreviewPage->setPreviewPage(DataTools::getEpisodes(rss),currentPodcast,false);
      gtk_stack_set_visible_child(GTK_STACK(UImainStack),UIPVPodcastDetailsPage);
      return;
    }
  }

  ///  gets the returned selection from search results
  void returnSelection(GtkWidget* e, gpointer PodcastIndex)
  {
    int index = *(int*)PodcastIndex;
    int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(UInotebook));

    if (page == stackPage::LibraryPage)
      currentPodcast = Library.at(index);

    if (page == stackPage::SearchPage)
      currentPodcast = searchList.at(index);

    if (deleteMode == false)
    {
      displayPodcastDetails(currentPodcast);
    }
    if (deleteMode == true)
    {
      deletePodcastFromLibrary(currentPodcast);
    }
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
  //  begin audioPlayer UI to audio player class bindings
  void playButtonPressed(GtkWidget* button){
    player->pausePlay();
    // set button state to inversions of it's current state
  }
  void forwardButtonPressed(GtkWidget* button){
    player->seek(3);
    // perform seek logic for more intuitive seeking behaviour
    // perform seek
  }
  void rewindButtonPressed(GtkWidget* button){
    player->seek(-3);
    // perform seek logic for more intuitive seeking behaviour
    // perform seek
  }
}