#ifndef _MENU_H
#define _MENU_H
#include <irrKlang.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"
using namespace irrklang;

class Menu {
private:
    typedef struct {
        char *name;
        ISound *sound;
    } menuItem;
    menuItem *mi;
    ISoundEngine *engine;    // Engine for entire application
    ISound *music;    // Reserved ISound object for music manipulation
    char *m;    // Character array to store music file or memory
    int size;    // Size of menu, size does not start at 0
    int current;    // Keeps track of menu choice, starts at 0
    char *mc;    // Stores name of menu click.
    ISound *menuClick;    // Stores an optional single menu click item
    int pan;    // Determines whether pan or not. 0=no, 1=yes
    int choice;
    ISound *message; char *mes;    // Message to play before navigating to menu
    int mesPlayed=0;    // Whether messaged has been played
    Timer t;    // A timer object
    void fadeAndStop();
    void playSound();
    void adjust();
    void playDown();
    void playUp();
public:
    // Constructor takes two args: the ISoundEngine pointer being used for the game, and number of
    // menu items to add.
    Menu(ISoundEngine *s, int count);
    void loadMusic(const char *c);
    void addMessage(const char *);
    void addItem(char *c);
    void addClick(const char *c);
    void enablePan(){pan=1;}
    void disablePan(){pan=0;}
    void displayMenu();
    int getChoice(){return choice;}
    void clean();    // Function to clean up.
    ~Menu();
};

Menu::Menu(ISoundEngine *s, int count)
{
    engine=s;
    size=count; mi = new menuItem[size];

    // Initialize pointers to 0
    for(int i = 0; i < size; i++){
        mi[i].sound = 0;
        mi[i].name=0;
    }
    music = 0; choice=0;
    m=0; current=0; menuClick=0; mc=0; pan=0;
    mes=0; message=0;
}

void Menu::addClick(const char *c){
    mc = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) mc[i]='\0';
    strcpy(mc, c);
    engine->addSoundSourceFromFile(c);
}

void Menu::fadeAndStop(){
    if(music){
        for(int i = 0; i < 10; i++){
            music->setVolume(music->getVolume()-0.1);
            SDL_Delay(100);
        }
        if(music->getVolume()!=0) music->setVolume(0);
        if(music->getVolume()==0) {music->stop();music=0;}
    }
    engine->removeAllSoundSources();
}

void Menu::playSound(){
    // Control playing of menu message
    if(message) if(!message->isFinished()) {message->stop(); mesPlayed=1;message=0;}
    // Play menu click if one exists
    if(mc) menuClick=engine->play2D(mc,false);

    // Play a menu item
    if(!mi[choice].sound) mi[choice].sound=engine->play2D(mi[choice].name,false);
}

void Menu::adjust(){
    if(choice<0) choice=current-1;
    if(choice>=size) choice=0;
}

void Menu::playDown(){
    if(choice<size&&choice>=0){
        choice++;
        adjust();
        playSound();
        //SDL_Delay(300);
    }
}

void Menu::playUp(){
    if(choice>=0&&choice<size){
        choice--;
        adjust();
        playSound();
        //SDL_Delay(300);
    }
}

void Menu::loadMusic(const char *c){
    m = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) m[i]='\0';
    strcpy(m,c);
    engine->addSoundSourceFromFile(m);
}

void Menu::addMessage(const char *c){
    mes = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) mes[i]='\0';
    strcpy(mes, c);
    engine->addSoundSourceFromFile(c);
}

void Menu::addItem(char *c){
    mi[current].name = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) mi[current].name[i]='\0';
    strcpy(mi[current].name, c);
    engine->addSoundSourceFromFile(c);
    current++;
}

void Menu::displayMenu(){
    SDL_Event e;
    if(m) music=engine->play2D(m, true,false,true,ESM_AUTO_DETECT,true);
    if(mes)
        if(mesPlayed==0) message=engine->play2D(mes,false,false,true);
    t.start();
    while(true){
        // Keep this code at the beginning of the loop to check for engine changes
        //engine->update();

        // Get time per every iteration for cooperative key presses.
        int mt = t.getTime();
        if(SDL_PollEvent(&e)==0&&e.type==SDL_KEYDOWN){
            if((e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE)&&mt>200){ t.start(); choice=-1;break;}

            // Putting code to turn down or up the volume
            if(music){
                if(e.key.keysym.sym==SDLK_F1&&mt>200){
                    t.start();
                    music->setVolume(music->getVolume()-0.1);
                    if(music->getVolume()<0) music->setVolume(0);
                    //SDL_Delay(100);
                }
            }
            if(music){
                if(e.key.keysym.sym==SDLK_F2&&mt>200){
                    t.start();
                    music->setVolume(music->getVolume()+0.1);
                    if(music->getVolume()>1) music->setVolume(1);
                    //SDL_Delay(100);
                }
            }

            // Insert code here for moving through menu items.
            // Begin with code for down arrow
        if(e.key.keysym.sym==SDLK_DOWN&&mt>200) {t.start();playDown();}
            // Then, code for up arrow
        if(e.key.keysym.sym==SDLK_UP&&mt>200) {t.start();playUp();}
            // Code for enter key
            if(e.key.keysym.sym==SDLK_RETURN&&mt>200){t.start();fadeAndStop();break;}
        }
    }
}

void Menu::clean(){
        if(mi)
        for(int i = 0; i < size; i++){
            if(mi[i].sound) mi[i].sound->drop();
            if(mi[i].name) delete [] mi[i].name;
        }
    if(music){
        music->drop();
        engine->removeSoundSource(m);
    }
    if(mc) delete [] mc;
    if(menuClick) menuClick->drop();
    if(m) delete [] m;
    if(message) message->drop();
    if(mes) delete [] mes;
    m = 0; music = 0;
    mc=0; menuClick=0; delete [] mi; mi=0;
}

Menu::~Menu()
{
    clean();
}
#endif
