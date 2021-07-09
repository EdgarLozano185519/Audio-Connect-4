#include <SDL.h>
#include "Numbers.h"
#include "Network.h"
#include <irrKlang.h>
#include "Menu.h"
#include "ConnectFourMenu.h"
#include "Board.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace irrklang;

#ifdef main
#undef main
#endif

SDL_Window* myWindow = NULL;    // Get a window pointer for later use
SDL_Surface* mySurface = NULL;    // Pointer for surface; required for rendering to window
void mainMenu(ISoundEngine *);
void testSpeakers(ISoundEngine *);
void game(ISoundEngine *engine);
void online(ISoundEngine *engine);
void offline(ISoundEngine*);
int main(int argc, char* argv[]) {
    // Define constants for screen
    const int WIDTH = 640;
    const int HEIGHT = 480;
    int menuSelection;    // Stores menu selection

    // Initialize SDL systems
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDLNet_Init();
    srand(time(NULL));    // Init random numbers with time

    // Create the window
    myWindow = SDL_CreateWindow("Audio ConnectFour", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    // Get the window surface
    mySurface = SDL_GetWindowSurface(myWindow);

    //Update the surface
    SDL_UpdateWindowSurface( myWindow );

    ISoundEngine *engine = createIrrKlangDevice();

    // Create the main menu
    mainMenu(engine);

    // Keep this at the bottom
    // This is to close all used subsystems
    SDL_DestroyWindow(myWindow);    // Destroy the window
    SDL_Quit();    // Quit the main SDL subsystem
    SDLNet_Quit();
    myWindow = NULL;
    mySurface = NULL;
    engine->drop();
}

void mainMenu(ISoundEngine *engine){
    Menu myMenu(engine,3);
    myMenu.loadMusic("../sounds/music.ogg");
    myMenu.addItem("../sounds/startGame.ogg");
    myMenu.addItem("../sounds/testSpeakers.ogg");
    myMenu.addItem("../sounds/exit.ogg");
    myMenu.addClick("../sounds/click.ogg");
    myMenu.addMessage("../sounds/welcome.ogg");
    myMenu.displayMenu();
    int choice=myMenu.getChoice();

    // Code to handle menu options here
    if(choice==1) testSpeakers(engine);
    else if(choice==0) game(engine);
}

void testSpeakers(ISoundEngine *engine){
    SDL_Event e;
    ISound *s;
    s=engine->play2D("../sounds/speaker.ogg",false,false,true);
    while(!s->isFinished()){
        if(SDL_PollEvent(&e)==0&&e.type==SDL_KEYDOWN)
            if(e.key.keysym.sym==SDLK_ESCAPE){ s->stop(); break;}
    }
    mainMenu(engine);
}

void game(ISoundEngine *engine){
    Menu myMenu(engine,2);
    myMenu.addItem("../sounds/online.ogg");
    myMenu.addItem("../sounds/offline.ogg");
    myMenu.addClick("../sounds/click.ogg");
    myMenu.addMessage("../sounds/gameMode.ogg");
    myMenu.displayMenu();
    if(myMenu.getChoice()==-1) mainMenu(engine);
    if(myMenu.getChoice()==0) online(engine);
    if(myMenu.getChoice()==1) offline(engine);
}

void online(ISoundEngine *engine){
    Network n("localhost",3000);
    if(n.connect()){
        n.sendNum(-2);
        ISound *s = engine->play2D("../sounds/success.ogg",false,false,true);
        while(!s->isFinished()) SDL_Delay(100);
        int playerId = n.receiveWait();
        engine->play2D("../sounds/numberPrompt.ogg");
        int playerFound = inputNumber(engine, &n, playerId);
        if(playerFound==-3){
            n.sendNum(-5);
            playerFound = n.receiveWait();
            n.sendNum(-6);
            int opponentFound = n.receiveWait();
            multiPlayer(engine, playerFound, opponentFound, &n);
        }
    }
    else{
        ISound *s = engine->play2D("../sounds//unsuccessful.ogg",false,false,true);
    while(!s->isFinished()) SDL_Delay(100);
    }
    if(n.isConnected()){ n.sendNum(-200); n.close();}
    mainMenu(engine);
}

void offline(ISoundEngine *engine){
    // I think this has to go through different states
    // Pick a random state to determine who goes first
    int state;
    int randomNumber=rand();
    printf("Random number is %d", randomNumber);
    randomNumber = randomNumber%2+1;
    if(randomNumber==1) state=1;
    else state=4;

    // Description of different states
    // State 2: Player has to make a move by selecting menu option
    // State 2 also allows player to press space to determine num of filled slots
    // State 3: Drop token into board and check status.

    // This part will set up the board
    Board board(7,6);

    // Set up the menu and its items.
    ConnectFourMenu columnMenu(engine,7);
    columnMenu.addItem("../sounds/numbers/1.ogg");
    columnMenu.addItem("../sounds/numbers/2.ogg");
    columnMenu.addItem("../sounds/numbers/3.ogg");
    columnMenu.addItem("../sounds/numbers/4.ogg");
    columnMenu.addItem("../sounds/numbers/5.ogg");
    columnMenu.addItem("../sounds/numbers/6.ogg");
    columnMenu.addItem("../sounds/numbers/7.ogg");
    columnMenu.addClick("../sounds/click.ogg");

    // Set up helper vars
    int enterPressed = 0;    // Detect whether player pressed enter or space on menu
    columnMenu.startTime();    // Start the in-menu timer
    int temp;    // Temp var for storing player column
    // Game loop starts here
    playInterupt(engine,"../sounds/inGameIntro.ogg");
    while(true){
        if(state==1){
            playWait(engine, "../sounds/yourTurn.ogg");
            state=2;
        }
        else if(state==2){
            columnMenu.displayMenu(&enterPressed);
            if(enterPressed==1){ enterPressed=0; state++;}
            else if(enterPressed==2){
                enterPressed=0;
                printf("Menu choice is %d\n", columnMenu.getChoice());
                speakPlayerRows(columnMenu.getChoice(),engine,1,board);
                speakPlayerRows(columnMenu.getChoice(),engine,2,board);
            }
            if(columnMenu.getChoice()==-1){ engine->removeAllSoundSources(); break;}
        }
        else if(state==3){
            temp = board.dropPiece(columnMenu.getChoice(),1);
            printf("empty space found at %d\n", temp+1);
            if(temp==-1){
                playWait(engine, "../sounds/fullTry.ogg");
                state=1;
            }
            else{
                if(board.checkAll(1,columnMenu.getChoice(),temp)){
                    engine->removeAllSoundSources();
                    break;
                }
                else{ playDropSound(engine); state=4;}
            }
        }
        else if(state==4){
            playWait(engine, "../sounds/opponentTurn.ogg");
            int col = columnMenu.getChoice();
            board.playerAi(&col,&temp);
            printf("Player AI got %d column and %d row\n",col,temp);
            if(board.checkAll(2,col,temp))
                break;
            else{
                //board.displayGrid();
                playDropSound(engine);
                state=1;
            }
        }
    }
    mainMenu(engine);
}

// To determine who goes first, the server should randomly select and send a playerId that to both of the participating players
// 
/*
 * Note: this function is being used for actual multiplayer implementation
 @param engine: The irrklang isound engine instance
 @param playerId: The player id found in an earlier online function
 @param opponent: Opponent player
 @param n: The network passed in as a pointer
*/
void multiPlayer(ISoundEngine *engine, int playerId, int opponent, Network *n){
    int turn = -200;    // A var that contains whose turn it will be
    // This part will set up the board
    Board board(7,6);

    // Set up the menu and its items.
    ConnectFourMenu columnMenu(engine,7);
    columnMenu.addItem("../sounds/numbers/1.ogg");
    columnMenu.addItem("../sounds/numbers/2.ogg");
    columnMenu.addItem("../sounds/numbers/3.ogg");
    columnMenu.addItem("../sounds/numbers/4.ogg");
    columnMenu.addItem("../sounds/numbers/5.ogg");
    columnMenu.addItem("../sounds/numbers/6.ogg");
    columnMenu.addItem("../sounds/numbers/7.ogg");
    columnMenu.addClick("../sounds/click.ogg");

    // Set up helper vars
    int enterPressed = 0;    // Detect whether player pressed enter or space on menu
    columnMenu.startTime();    // Start the in-menu timer
    int temp;    // Temp var for storing player column
    playInterupt(engine,"../sounds/inGameIntro.ogg");
    // Game loop starts here
    while(true){
        if(turn==-200){
            n->send(
            