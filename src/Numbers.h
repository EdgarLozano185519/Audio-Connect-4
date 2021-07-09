#ifndef NUMBERS_H
#define NUMBERS_H
#include "Network.h"
#include "Timer.h"
#include <SDL.h>
#include <irrKlang.h>
#include <string.h>
#include <stdio.h>
using namespace irrklang;

/*
These are a collection of functions that might help with managing numbers.
A class representing a number is here to help with its announcement.
Functions are here to facilitate number input and a few helper functions.
*/

/*
This class is for helping with number announcement.
*/
class Number{
private:
    typedef struct{
        char *c;
    } soundStrings;
    soundStrings s[10];
    int states;
    int state;
    Timer t;
    int numArray[10];
    ISoundEngine *engine;
        int isSpeaking;
    void init(){
    for(int i = 0; i < 10; i++) numArray[i]=-1;
        states=0;
        state=-1;
        isSpeaking=-1;
    };
public:
    Number(ISoundEngine *e){init(); engine=e; };
    void setNumber(int);
    void addNumSound(char *cc, int index){
        s[index].c = new char[strlen(cc)+1];
        for(int i=0; i<strlen(cc)+1; i++) s[index].c[i]='\0';
        strcpy(s[index].c,cc);
    };
    Number(int num, ISoundEngine *e){setNumber(num);engine=e;};
    ~Number();
    void sayNumber();
    void isSpeakingToggle(){isSpeaking=1;};
};

void Number::setNumber(int n){
    init();
    while(n!=0){
        int temp = n%10;
        n /= 10;
        numArray[states] = temp;
        states++;
    }
    for(int i = states-1; i>=0; i--){
        printf("%d ", numArray[i]);
    }
    printf("\n");
    state = states-1;
    t.start();
}
    
void Number::sayNumber(){
    if(state!=-1&&isSpeaking==1){
        if(t.getTime()>350){
            t.start();
            engine->play2D(s[numArray[state]].c);
            state--;
        }
    }
    else{
        isSpeaking=-1;
    state = states-1;
}
}

Number::~Number(){
    for(int i = 0; i < 10; i++)
        if(s[i].c) delete [] s[i].c;
    init();
}

void addNumberSounds(Number *n){
    n->addNumSound("../sounds/numbers/0.ogg",0);
    n->addNumSound("../sounds/numbers/1.ogg",1);
    n->addNumSound("../sounds/numbers/2.ogg",2);
    n->addNumSound("../sounds/numbers/3.ogg",3);
    n->addNumSound("../sounds/numbers/4.ogg",4);
    n->addNumSound("../sounds/numbers/5.ogg",5);
    n->addNumSound("../sounds/numbers/6.ogg",6);
    n->addNumSound("../sounds/numbers/7.ogg",7);
    n->addNumSound("../sounds/numbers/8.ogg",8);
    n->addNumSound("../sounds/numbers/9.ogg",9);
}

int inputNumber(ISoundEngine *engine, Network *n, int playerId){
    Timer t;
    int result = 0;
    int check = -1;
    Number num(engine);
    addNumberSounds(&num);
    num.setNumber(playerId);
    SDL_Event e;
    t.start();
    while(true){
        int tt = t.getTime();
        num.sayNumber();
        n->send(-4);
        int tempReceive = n->receiveWait();
        if(SDL_PollEvent(&e)!=0 && e.type==SDL_KEYDOWN){
            if(e.key.keysym.sym==SDLK_0 && tt>250){
                t.start();
                engine->play2D("../sounds/numbers/0.ogg");
                result+=0;
            }
            else if(e.key.keysym.sym==SDLK_1&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/1.ogg");
                result+=1;
            }
            else if(e.key.keysym.sym==SDLK_2&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/2.ogg");
                result+=2;
            }
            else if(e.key.keysym.sym==SDLK_3&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/3.ogg");
                result+=3;
            }
            else if(e.key.keysym.sym==SDLK_4&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/4.ogg");
                result+=4;
            }
            else if(e.key.keysym.sym==SDLK_5&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/5.ogg");
                result+=5;
            }
            else if(e.key.keysym.sym==SDLK_6&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/6.ogg");
                result+=6;
            }
            else if(e.key.keysym.sym==SDLK_7&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/7.ogg");
                result+=7;
            }
            else if(e.key.keysym.sym==SDLK_8&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/8.ogg");
                result+=8;
            }
            else if(e.key.keysym.sym==SDLK_9&&tt>250){
                t.start();
                engine->play2D("../sounds/numbers/9.ogg");
                result+=9;
            }
            else if(e.key.keysym.sym==SDLK_ESCAPE){ check=-2; break;}
            else if(e.key.keysym.sym==SDLK_RETURN){check=-3;break;}
            else if(e.key.keysym.sym==SDLK_SPACE) num.isSpeakingToggle();
            if(tempReceive==-3){
                result = tempReceive;
                check=-4;
                break;
            }
        }
    }
    if(check==-2) return -1;
    if(check==-3){
        if(result==0){
            ISound *s = 0; s = engine->play2D("../sounds/invalid.ogg",false,false,true);
            while(!s->isFinished()) SDL_Delay(100);
            if(s){s->drop(); s=0;}
            return inputNumber(engine, n, playerId);
        }
        else{
            n->sendNum(-3);
            n->sendNum(playerId);
            return n->receiveWait();
        }
    }
    else{
        return result;
    }
}

#endif