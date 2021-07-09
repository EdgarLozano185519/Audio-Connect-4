#ifndef _CONNECT_FOUR_MENU_H
#define _CONNECT_FOUR_MENU_H
#include <irrKlang.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"
using namespace irrklang;

class ConnectFourMenu {
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
    void playSound();
    void adjust();
    void playDown();
    void playUp();
public:
    // Constructor takes two args: the ISoundEngine pointer being used for the game, and number of
    // menu items to add.
    ConnectFourMenu(ISoundEngine *s, int count);
    void startTime(){t.start();};
    void loadMusic(const char *c);
    void addMessage(const char *);
    void addItem(char *c);
    void addClick(const char *c);
    void enablePan(){pan=1;}
    void disablePan(){pan=0;}
    void displayMenu(int*);
    int getChoice(){return choice;}
    void clean();    // Function to clean up.
    ~ConnectFourMenu();
};

ConnectFourMenu::ConnectFourMenu(ISoundEngine *s, int count)
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

void ConnectFourMenu::addClick(const char *c){
    mc = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) mc[i]='\0';
    strcpy(mc, c);
    engine->addSoundSourceFromFile(c);
}

void ConnectFourMenu::playSound(){
    // Control playing of menu message
    if(message) if(!message->isFinished()) {message->stop(); mesPlayed=1;message=0;}
    // Play menu click if one exists
    if(mc) menuClick=engine->play2D(mc,false);

    // Play a menu item
    if(!mi[choice].sound) mi[choice].sound=engine->play2D(mi[choice].name,false);
}

void ConnectFourMenu::adjust(){
    if(choice<0) choice=0;
    if(choice>=size) choice=size-1;
}

void ConnectFourMenu::playDown(){
    if(choice<size&&choice>=0){
        choice++;
        adjust();
        playSound();
    }
}

void ConnectFourMenu::playUp(){
    if(choice>=0&&choice<size){
        choice--;
        adjust();
        playSound();
    }
}

void ConnectFourMenu::loadMusic(const char *c){
    m = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) m[i]='\0';
    strcpy(m,c);
    engine->addSoundSourceFromFile(m);
}

void ConnectFourMenu::addMessage(const char *c){
    mes = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) mes[i]='\0';
    strcpy(mes, c);
    engine->addSoundSourceFromFile(c);
}

void ConnectFourMenu::addItem(char *c){
    mi[current].name = new char[strlen(c)+1];
    for(int i=0; i<strlen(c)+1; i++) mi[current].name[i]='\0';
    strcpy(mi[current].name, c);
    engine->addSoundSourceFromFile(c);
    current++;
}

void ConnectFourMenu::displayMenu(int *enter){
    SDL_Event e;
    if(m) music=engine->play2D(m, true,false,true,ESM_AUTO_DETECT,true);
    if(mes)
        if(mesPlayed==0) message=engine->play2D(mes,false,false,true);
    //while(true){
        // Keep this code at the beginning of the loop to check for engine changes
        //engine->update();

        // Get time per every iteration for cooperative key presses.
        int mt = t.getTime();
        if(SDL_PollEvent(&e)==1&&e.type==SDL_KEYDOWN){
            if((e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE)&&mt>200){
                t.start();
                choice=-1;
            }

            // Putting code to turn down or up the volume
            if(music){
                if(e.key.keysym.sym==SDLK_F1&&mt>200){
                    t.start();
                    music->setVolume(music->getVolume()-0.1);
                    if(music->getVolume()<0) music->setVolume(0);
                }
            }
            if(music){
                if(e.key.keysym.sym==SDLK_F2&&mt>200){
                    t.start();
                    music->setVolume(music->getVolume()+0.1);
                    if(music->getVolume()>1) music->setVolume(1);
                }
            }

            // Insert code here for moving through menu items.
            // Switch for this instance of class for sake of ConnectFour game
            // The keys being used are right and left to move
        if(e.key.keysym.sym==SDLK_RIGHT&&mt>200) {t.start();playDown();}
            // Then, code for up arrow
        else if(e.key.keysym.sym==SDLK_LEFT&&mt>200) {t.start();playUp();}
            // Code for enter key
            else if(e.key.keysym.sym==SDLK_RETURN&&mt>200){
                t.start();
                *enter=1;
            }
            else if(e.key.keysym.sym==SDLK_SPACE&&mt>200){
                t.start();
                printf("Space was pressed\n");
                *enter=2;
            }
        }
    //}
}

void ConnectFourMenu::clean(){
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

ConnectFourMenu::~ConnectFourMenu()
{
    clean();
}
#endif
