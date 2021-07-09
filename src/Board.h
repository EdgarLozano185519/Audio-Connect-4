#ifndef _BOARD_H
#define _BOARD_H
#include "Timer.h"
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

class Board{
private:
    int **grid;
    int maxColumns;
    int maxRows;
public:
    Board(){
        grid=0;
    };
    Board(int numColumns, int numRows){
        //if(grid==0){
            grid = new int*[numColumns];
            for(int i = 0; i < numColumns; i++){
                grid[i] = new int[numRows];
            }
            maxColumns = numColumns;
            maxRows = numRows;
            for(int i = 0; i<maxColumns; i++){
                for(int j = 0; j < maxRows; j++){
                    grid[i][j] = -1;
                }
            }
        //}
    };
    void setCols(int numColumns){maxColumns=numColumns;};
    void setRows(int numRows){maxRows=numRows;};
    void setBoard(){
        if(grid==0){
            grid = new int*[maxColumns];
            for(int i = 0; i < maxColumns; i++){
                grid[i] = new int[maxRows];
            }
            for(int i = 0; i<maxColumns; i++){
                for(int j = 0; j < maxRows; j++){
                    grid[i][j] = -1;
                }
            }
        }
    };
    int dropPiece(int column, int playerId){
        int emptySpace = -1;
        for(int i = maxRows-1; i>=0; i--){
            if(grid[column][i]==-1){
                emptySpace = i;
                break;
            }
        }
        if(emptySpace!=-1){
            grid[column][emptySpace] = playerId;
        }
        return emptySpace;
    };
    bool checkHorizontal(int playerId, int column, int row){
        int fourRight = column+3;
        bool bFourRight = false;
        int fourLeft = column-3;
        bool bFourLeft = false;
        if(fourRight < maxColumns){
        int count = 0;
            for(int i = column+1; i <= fourRight; i++){
                if(grid[i][row]==playerId) count++;
            }
            if(count==3) bFourRight=true;
        }
        if(fourLeft>=0){
            int count=0;
            for(int i = column-1; i >= fourLeft; i--){
                if(grid[i][row]==playerId) count++;
            }
            if(count==3) bFourLeft=true;
        }
        return (bFourLeft || bFourRight);
    };
    bool checkVertical(int playerId, int column, int row){
        int fourDown = row-3;
        int fourUp = row+3;
        bool bFourDown = false;
        bool bFourUp = false;
        if(fourUp < maxRows){
            int count=0;
            for(int i = row+1; i <= fourUp; i++)
                if(grid[column][i]==playerId) count++;
            if(count==3) bFourUp = true;
        }
        if(fourDown >= 0){
            int count = 0;
            for(int i = row-1; i >= fourDown; i--)
                if(grid[column][i]==playerId) count++;
            if(count==3) bFourDown = true;
        }
        return (bFourDown || bFourUp);
    };
    bool checkDiagonals(int playerId, int column, int row){
        bool bUpRight = false;
        bool bDownRight = false;
        bool bUpLeft = false;
        bool bDownLeft = false;
        int **upRight = new int*[3];
        int **upLeft = new int*[3];
        int **downLeft = new int*[3];
        int **downRight = new int*[3];
        for(int i = 0; i < 3; i++){
            upRight[i] = new int[2];
            upLeft[i] = new int[2];
            downRight[i] = new int[2];
            downLeft[i] = new int[2];
        }
        for(int i = 0; i < 3; i++){
            int temp = (i+1);
            upRight[i][0] = column+temp; upRight[i][1]=row+temp;
            upLeft[i][0] = column-temp; upLeft[i][1] = row+temp;
            downLeft[i][0] = column-temp; downLeft[i][1] = row-temp;
            downRight[i][0] = column+temp; downRight[i][1] = row-temp;
        }
        if(upRight[2][0]<maxColumns && upRight[2][1]<maxRows){
            int count = 0;
            for(int i = 0; i < 3; i++){
                int rowCheck = upRight[i][1];
                int columnCheck = upRight[i][0];
                if(grid[columnCheck][rowCheck]==playerId) count++;
            }
            if(count==3) bUpRight = true;
        }
        if(upLeft[2][0]>=0 && upLeft[2][1]<maxRows){
            int count = 0;
            for(int i = 0; i < 3; i++){
                int rowCheck = upLeft[i][1];
                int columnCheck = upLeft[i][0];
                if(grid[columnCheck][rowCheck]==playerId) count++;
            }
            if(count==3) bUpLeft = true;
        }
        if(downRight[2][0]<maxColumns && downRight[2][1]>=0){
            int count = 0;
            for(int i = 0; i < 3; i++){
                int rowCheck = downRight[i][1];
                int columnCheck = downRight[i][0];
                if(grid[columnCheck][rowCheck]==playerId) count++;
            }
            if(count==3) bDownRight = true;
        }
        if(downLeft[2][0]>=0 && downLeft[2][1]>=0){
            int count = 0;
            for(int i = 0; i < 3; i++){
                int rowCheck = downLeft[i][1];
                int columnCheck = downLeft[i][0];
                if(grid[columnCheck][rowCheck]==playerId) count++;
            }
            if(count==3) bDownLeft = true;
        }
        for(int i = 0; i < 3; i++){
            delete [] upRight[i]; delete [] upLeft[i];
            delete [] downRight[i]; delete [] downLeft[i];
        }
        delete [] upRight; delete [] upLeft;
        delete [] downRight; delete [] downLeft;
        return (bUpRight || bUpLeft || bDownRight || bDownLeft);
    };
    bool checkAll(int playerId, int column, int row){
        return (checkHorizontal(playerId,column,row)||checkVertical(playerId,column,row)||checkDiagonals(playerId,column,row));
    };
    bool isFull(){
        for(int i = 0; i < maxColumns; i++)
            if(grid[i][0]==-1) return false;
        return true;
    };
    bool isColumnFull(int column){
        if(grid[column][0]!=-1) return true;
        return false;
    };
    bool rightIsEmpty(int column){
        if(column < maxColumns-1)
            if(grid[column+1][0]==-1) return true;
        return false;
    };
    bool leftIsEmpty(int column){
        if(column>0)
            if(grid[column-1][0] == -1) return true;
        return false;
    };
    int findEmpty(){
        for(int i = 0; i < maxColumns; i++)
            if(grid[i][0]==-1) return i;
        return -1;
    };
    void playerAi(int *column,int*row){
        if(isFull()) return;
        int temp = rand()%2+1;
        if(temp==1){
            if(isColumnFull(*column))
                *column = findEmpty();
            *row = dropPiece(*column,2);
        }
        else{
            temp = rand()%2+1;
            if(temp==1){
                if(rightIsEmpty(*column)){
                    *column += 1;
                    *row = dropPiece(*column,2);
                }
                else{
                    *column = *column-1;
                    *row = dropPiece(*column,2);
                }
            }
            else{
                if(leftIsEmpty(*column)){
                    *column = *column-1;
                    *row = dropPiece(*column,2);
                }
                else{
                    *column += 1;
                    *row = dropPiece(*column,2);
                }
            }
        }
    };
    void displayGrid(){
        for(int i = 0; i < maxColumns; i++){
            for(int j = 0; j < maxRows; j++){
                printf("Column %d row %d: %d\n", i,j,grid[i][j]);
            }
        }
    };
    void populateArrayElement(int *p, int num){
        *p = num;
    };
    void populateArray(int* a, int size, int column){
        for(int i = 0; i < size; i++){
            populateArrayElement(&a[i], grid[column][i]);
        }
    }
};

// Putting a few helper functions because I can
void playWait(ISoundEngine *engine, char *c){
    ISound *s = engine->play2D(c, false, false, true);
    while(!s->isFinished()) SDL_Delay(10);
    s->drop();
}

void speakNumbersWait(ISoundEngine *engine, int num){
    if(num==1) playWait(engine,"../sounds/numbers/1.ogg");
    else if(num==2) playWait(engine,"../sounds/numbers/2.ogg");
    else if(num==3) playWait(engine,"../sounds/numbers/3.ogg");
    else if(num==4) playWait(engine,"../sounds/numbers/4.ogg");
    else if(num==5) playWait(engine,"../sounds/numbers/5.ogg");
    else if(num==6) playWait(engine,"../sounds/numbers/6.ogg");
    else if(num==7) playWait(engine,"../sounds/numbers/7.ogg");
    else if(num==8) playWait(engine,"../sounds/numbers/8.ogg");
    else if(num==9) playWait(engine,"../sounds/numbers/9.ogg");
}

void speakPlayerRows(int column, ISoundEngine *engine, int playerId, Board b){
    int array[6];
    for(int i = 0; i < 6; i++) array[i]=-1;
    b.populateArray(array,6,column);
    if(playerId==1) playWait(engine,"../sounds/yourRows.ogg");
    else playWait(engine,"../sounds/opponentRows.ogg");
    for(int i = 0; i < 6; i++){
        int temp = array[i];
        if(temp<10&&temp>0&&temp==playerId){
            speakNumbersWait(engine,i);
        }
    }
}

void playDropSound(ISoundEngine *engine){
    int rn = rand()%2+1;
    if(rn==1) playWait(engine,"../sounds/drop.ogg");
    else playWait(engine,"../sounds/drop1.ogg");
}

void playInterupt(ISoundEngine *engine, char *c){
    SDL_Event e;
    Timer t;
    t.start();
    ISound *s = engine->play2D(c, false, false, true);
    while(!s->isFinished()){
        if(SDL_PollEvent(&e)!=0){
            if(t.getTime()>300&&e.key.keysym.sym==SDLK_SPACE){
                s->stop();
            }
        }
    }
    s->drop();
}
#endif
