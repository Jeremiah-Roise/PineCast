#include<gtk/gtk.h>
#include<iostream>
#include<stdio.h>
#include<future>
#include<stdlib.h> 
#include"Libs/DataTools.h"
#include"Libs/webTools.h"
#include"Libs/podcastDataTypes.h"
#include"Libs/PodcastMetaDataLists.h"
#include"Libs/UINAMES.h"
using namespace std;

extern "C"{
void DownloadAndPlayPodcast(string mp3Url,string name,GtkProgressBar* bar);
GtkWidget* CreateSearchEntry(PodcastMetaData);
void createSearchResults(GtkWidget *e,PodcastMetaDataList x);
void testPrint(GtkWidget* e,gpointer data);
void returnSelection(GtkWidget*,gpointer);
void PodcastSearchEntry(GtkEntry *e);
void getSelectedPodcastEpisode(GtkWidget* e);
void clearContainer(GtkContainer* e);
void loadLib(PodcastMetaDataList* list);
void removeFromLibrary();
GdkPixbuf* createImage(string imageUrl,int scaleX,int scaleY);
GtkWidget* listBox;
GtkWidget* window;
GtkWidget* stack;
GtkWidget* notebook;
GtkWidget* PodcastDetailsPage;
//  PV means Preview
GtkWidget* PVImage;
GtkWidget* PVTitle;
GtkWidget* PVAuthor;
GtkWidget* PVEpisodeList;
GtkWidget* LibraryUi;
GtkWidget* addToLibraryButton;
GtkBuilder* builder;
PodcastMetaData currentPodcast;
PodcastMetaDataList searchList;
PodcastMetaDataList Library;
podcastDataTypes::episodeList currentepisodes;
GtkWidget* stackPage = 0;
bool deleteMode = false;
//  GUI setup
int main(int argc,char** argv)
{
  //  TODO create thread with loadLib and mutexes for locking
  gtk_init(&argc,&argv);
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "PodcastWindow.glade", NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder,"MainWindow"));

  //  ? might be worth multithreading?
  listBox = GTK_WIDGET(gtk_builder_get_object(builder,"SearchListBox"));
  stack = GTK_WIDGET(gtk_builder_get_object(builder,"PageSelector"));
  notebook = GTK_WIDGET(gtk_builder_get_object(builder,"Lib/Search"));
  PodcastDetailsPage = GTK_WIDGET(gtk_builder_get_object(builder,"PodcastDetails"));
  LibraryUi = GTK_WIDGET(gtk_builder_get_object(builder,"Library"));
  PVImage = GTK_WIDGET(gtk_builder_get_object(builder,"PVimage"));
  PVTitle = GTK_WIDGET(gtk_builder_get_object(builder,"PVTitle"));
  PVAuthor = GTK_WIDGET(gtk_builder_get_object(builder,"PVAuthor"));
  PVEpisodeList = GTK_WIDGET(gtk_builder_get_object(builder,"PVEpisodeList"));
  addToLibraryButton = GTK_WIDGET(gtk_builder_get_object(builder,"addToLib"));

  gtk_builder_connect_signals(builder,NULL);
  g_object_unref(builder);
  loadLib(&Library);
  gtk_widget_show(window);
  createSearchResults(LibraryUi,Library);
  gtk_main();
  return 0;
}

//  update podcasts in library
void loadLib(PodcastMetaDataList* list){
    list->clear();
    cout << "LoadingLibrary" << endl;
    // open file To read
    string fileData  = DataTools::getFile("Podcasts/MyPodcasts.xml");
    int index = 0;
    for (size_t i = 0; i < fileData.length(); i++)
    {

        string Podcast = DataTools::GetFieldP(fileData,"<Podcast>","</Podcast>",index,index);
        if(Podcast == ""){break;}
        list->addPodcast(DataTools::GetField(Podcast,"<Artist=\"","\">"),
        DataTools::GetField(Podcast,"<RssFeed=\"","\">"),
        DataTools::GetField(Podcast,"<Title=\"","\">"),
        DataTools::GetField(Podcast,"<Image30=\"","\">"),
        DataTools::GetField(Podcast,"<Image60=\"","\">"),
        DataTools::GetField(Podcast,"<Image100=\"","\">"),
        DataTools::GetField(Podcast,"<Image600=\"","\">"));
    }
    cout << "finished" << endl;
}

void deleteModeON(){
  deleteMode = true;
}
void deleteModeOFF(){
  deleteMode = false;
}
void deleteModeSwitch(){
  deleteMode = !deleteMode;
}

//  Destroy Function For GUI
void on_MainWindow_destroy(){gtk_main_quit();}

//  for listing search results in a GtkListBox
//  TODO figure out how to multithread this image download freezes window
void createSearchResults(GtkWidget *container,PodcastMetaDataList x){
  int size = x.GetIndex();
  if(size == 0){return;}//  User Input Filtering
  searchList = x;
  //deleting old search results
  clearContainer(GTK_CONTAINER(container));

  for (int i = 0; i < size; i++)
  {
    PodcastMetaData tmp = x.GetPodcastAtIndex(i);
    string name = tmp.title;
    GtkWidget *result = CreateSearchEntry(tmp);
    
    
    gtk_container_add(GTK_CONTAINER(container),result);
    gtk_widget_show_all(result);
  }
}


GtkWidget* CreateSearchEntry(PodcastMetaData podcast){


  //  Image stuff
  GtkWidget *image = gtk_image_new_from_pixbuf(createImage(podcast.image600,50,50 ));
  //  ###########
  
  //  Formatting and setting up signals
  GtkWidget *label = gtk_label_new(podcast.title.c_str());
  gtk_label_set_xalign (GTK_LABEL(label), 0.0);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
  GtkWidget *event = gtk_event_box_new();
  gtk_label_set_line_wrap(GTK_LABEL(label),true);

  g_signal_connect(event,"button-press-event",(GCallback)returnSelection,(gpointer)nullptr);
  gtk_widget_set_name(GTK_WIDGET(event),(const gchar*)to_string(podcast.index).c_str());  //  setting the name to the index of the podcast
  gtk_box_pack_start(GTK_BOX(box),image,false,true,0);
  gtk_box_pack_start(GTK_BOX(box),label,true,true,0);
  gtk_container_add(GTK_CONTAINER(event),GTK_WIDGET(box));
  gtk_widget_show_all(event);
  //  #################################
  return event;
}

//  clears the given container of all children
void clearContainer(GtkContainer* e){
  gtk_container_foreach (e, (GtkCallback) gtk_widget_destroy, NULL);
}

//  create's an image from a url
GdkPixbuf* createImage(string imageUrl,int scaleX,int scaleY){
  string imagedata = webTools::getFileInMem(imageUrl);//  getting image data from web

  GdkPixbufLoader *test = gdk_pixbuf_loader_new();
  gdk_pixbuf_loader_set_size(test,scaleX,scaleY);
  gdk_pixbuf_loader_write(test,(const guchar*)imagedata.c_str(),imagedata.size(),nullptr);
  

  GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(test);
  gdk_pixbuf_loader_close(test,nullptr);

  return pixbuf;
}






/*
  ## Signal Functions Go beyond this comment ##
*/

// get search text and give it to the itunes search function
void PodcastSearchEntry(GtkEntry *e){
  std::future<void> loadList = std::async(std::launch::async,&createSearchResults,listBox,webTools::itunesSearch(gtk_entry_get_text(e)));
  //createSearchResults(listBox,webTools::itunesSearch(gtk_entry_get_text(e)));
  return;
}


void setPreviewPage()
{
  gtk_stack_set_visible_child(GTK_STACK(stack),PodcastDetailsPage);
  gtk_label_set_text(GTK_LABEL(PVTitle),currentPodcast.title.c_str());
  gtk_label_set_text(GTK_LABEL(PVAuthor),currentPodcast.artist.c_str());
  gtk_image_set_from_pixbuf(GTK_IMAGE(PVImage),createImage(currentPodcast.image600,200,200));

  //  list episodes
  podcastDataTypes::episodeList episodes = DataTools::getEpisodes(webTools::getFileInMem(currentPodcast.RssFeed));
  currentepisodes = episodes;
  clearContainer(GTK_CONTAINER(PVEpisodeList));
    for (int i = 0; i < episodes.getIndexSize(); i++)
    {
      GtkWidget* eventBox = gtk_event_box_new();
      GtkWidget* label = gtk_label_new(episodes.getEpisodeAtIndex(i).title.c_str());
      GtkWidget* box = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL,0);
      gtk_label_set_line_wrap(GTK_LABEL(label),true);
      gtk_label_set_xalign (GTK_LABEL(label), 0.0);
      gtk_container_add(GTK_CONTAINER(box),label);
      gtk_container_add(GTK_CONTAINER(eventBox),box);
      gtk_widget_show_all(eventBox);
      gtk_widget_set_name(eventBox,(gchar*)to_string(i).c_str());
      g_signal_connect(eventBox,"button-press-event",(GCallback)getSelectedPodcastEpisode,(gpointer)"button");
      gtk_container_add(GTK_CONTAINER(PVEpisodeList),eventBox);
    }
}

//  gets the returned selection from search results
void returnSelection(GtkWidget* e,gpointer data){
  
  int index = atoi(gtk_widget_get_name(e));

  searchList.GetPodcastAtIndex(index,currentPodcast);
  int page = (int)gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
  if (page == 0)
  {
    Library.GetPodcastAtIndex(index,currentPodcast);
  }
  if (page == 1)
  {
    searchList.GetPodcastAtIndex(index,currentPodcast);
  }

  if(page == 0 && deleteMode == true){
    removeFromLibrary();
  }
  if(deleteMode == false){
    setPreviewPage();
  }
}


//  simply goes to the main page
void goMainPage(){
  gtk_stack_set_visible_child_name(GTK_STACK(stack),(const gchar*)mainPageName);
}


//  Download Selected Episode
//  TODO use more descriptive name
void getSelectedPodcastEpisode(GtkWidget* e){
  podcastDataTypes::PodcastEpisode current = currentepisodes.getEpisodeAtIndex(atoi(gtk_widget_get_name(e)));
  
  e = gtk_widget_get_parent(e);
  clearContainer(GTK_CONTAINER(e));
  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  gtk_container_add(GTK_CONTAINER(box),gtk_label_new(current.title.data()));
  GtkWidget* progressBar = gtk_progress_bar_new();
  gtk_container_add(GTK_CONTAINER(box),progressBar);
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),0.0f);
  gtk_container_add(GTK_CONTAINER(e),box);
  gtk_widget_show_all(e);
  cout << current.title << endl;
  std::thread play(DownloadAndPlayPodcast,current.mp3Link,current.title,GTK_PROGRESS_BAR(progressBar));
  play.detach();
}


void DownloadAndPlayPodcast(string mp3Url,string name,GtkProgressBar* bar){

 name+=".mp3";

  double progress = 0;
  const std::future<void> thread = std::async(std::launch::async ,DownloadPodcast,mp3Url,name,&progress);
  cout << "created thread" << endl;

  while (thread.wait_for(0ms) != std::future_status::ready)// wait for download to finish
  {
    sleep(1);
    cout << "Download progress: " << progress << endl;
    gtk_progress_bar_set_fraction(bar,progress);
  }
  thread.wait();

  cout << progress << endl;
  gtk_widget_destroy(GTK_WIDGET(bar));

  string FName = "xdg-open \""+ name+"\"";
  FName+=" &";
  cout << "the name is: "<< FName<<endl;
  system(FName.data());
}


// TODO make this actually work
void streamPodcast(string mp3Url,string name){
  //  TODO make this a thread
  getWebFile(mp3Url,name);//  use DownloadPodcast here
  //  wait for a second or wait until theres a certain amount of progress
} 

void addToLibrary(){
  cout << "adding to library" << endl;
  string XML;
  XML = "\n<Podcast>";
  XML += "\n<Title=\""+currentPodcast.title+"\">";
  XML += "\n<RssFeed=\""+currentPodcast.RssFeed+"\">";
  XML += "\n<Image600=\""+currentPodcast.image600+"\">";
  XML += "\n<Image100=\""+currentPodcast.image100+"\">";
  XML += "\n<Image60=\""+currentPodcast.image60+"\">";
  XML += "\n<Image30=\""+currentPodcast.image30+"\">";
  XML += "\n</Podcast>";
  ofstream file;
  file.open("Podcasts/MyPodcasts.xml",std::ios::app);
  cout << "created file" << endl;
  file.write(XML.data(),XML.size());
  cout << "wrote to file" << endl;
  file.close();
  loadLib(&Library);
  createSearchResults(LibraryUi,Library);
}
}

void removeFromLibrary(){
  string XML;
  XML = DataTools::getFile("Podcasts/MyPodcasts.xml");
  int index;
  int end;
  index = XML.find("<Title=\""+ currentPodcast.title +"\">");
  index -= sizeof("<Podcast>");
  end = XML.find("</Podcast>");
  end += sizeof("</Podcast>");
  XML.erase(index,end);
  cout << XML.substr(index,end - index) << endl;

  fstream file;
  file.open("Podcasts/MyPodcasts.xml",fstream::out);
  file.write(XML.c_str(),XML.size());
  
  cout << "removed from lib" << endl;
}