/*******************************************************************************
 * Name        : tetris.c
 * Author      : Brandon Leung
 * Version     : 1.1.1
 * Date        : January 30, 2025
 * Description : An ASCII Art port of Tetris in C.
 * TODO        : Preview, Settings, Hold, SRS, Leaderboards, Restructure
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include "tetris.h"

#define SPAWNCURSORX 5
#define SPAWNCURSORY 0
#define NUMOFPIECES 7
#define PIECEDIMENSIONS 4
#define SPAWNORIENTATION 0
#define IMAXORIENTATIONS 2
#define OMAXORIENTATIONS 1
#define TMAXORIENTATIONS 4
#define SMAXORIENTATIONS 2
#define ZMAXORIENTATIONS 2
#define LMAXORIENTATIONS 4
#define JMAXORIENTATIONS 4
#define STARTINGSCORE 0
#define STARTINGLINES 0
#define BLOCKEXISTS 1
#define BLOCKABSENT 0
#define TOTALROWS 21        // 20 + 1 (border row)
#define ROWS 20
#define TOTALCOLUMNS 13     // 10 + 2 (border row) + 1 (cursor row)
#define COLUMNS 10

bool gameOver;
bool hardDropLock = false;
int previewPiece;
int fixedLinesRequired = 10;
int score = STARTINGSCORE;
int linesCleared = STARTINGLINES;
int debugNumber1 = 0;
int debugNumber2 = 0;
int cursorX;
int cursorY;
int currNumOrientations;
int currPiece[PIECEDIMENSIONS][PIECEDIMENSIONS][PIECEDIMENSIONS];
int tryPiece[PIECEDIMENSIONS][PIECEDIMENSIONS];
int currOrient;
int startLevel = 0;
int level;
int mode;
int preview;
int rng = 1;
int goal;
char basePieceArr[NUMOFPIECES] = {'I', 'O', 'T', 'S', 'Z', 'L', 'J'};
char tempPieceArr[NUMOFPIECES];
int boardArr[TOTALROWS][TOTALCOLUMNS];
int emptyBoardArr[TOTALROWS][TOTALCOLUMNS];
int tempBoardArr[TOTALROWS][TOTALCOLUMNS];

char pieceGen() {
    char nextPiece;
    if (rng == 2) {
        if (tempPieceArr[6] == 'X') {
            memcpy(tempPieceArr, basePieceArr, sizeof(basePieceArr));
            for (int i = 0; i < NUMOFPIECES; i++) {
                int j = rand() % NUMOFPIECES;
                char temp = tempPieceArr[j];
                tempPieceArr[j] = tempPieceArr[i];
                tempPieceArr[i] = temp;
            }
        }
        int i = 0;
        while (tempPieceArr[i] != 'X') {
            i++;
        }
        nextPiece = tempPieceArr[i];
        tempPieceArr[i] = 'X';
    } else {
        int randNum = rand() % NUMOFPIECES;
        nextPiece = basePieceArr[randNum];
    }
    hardDropLock = false;
    return nextPiece;
}

int spawnPiece(int piece) {
    if (piece == 'I') {
        memcpy(currPiece, IPiece, sizeof(IPiece));
        currNumOrientations = IMAXORIENTATIONS;
    } else if (piece == 'O') {
        memcpy(currPiece, OPiece, sizeof(OPiece));
        currNumOrientations = OMAXORIENTATIONS;
    } else if (piece == 'T') {
        memcpy(currPiece, TPiece, sizeof(TPiece));
        currNumOrientations = TMAXORIENTATIONS;
    } else if (piece == 'S') {
        memcpy(currPiece, SPiece, sizeof(SPiece));
        currNumOrientations = SMAXORIENTATIONS;
    } else if (piece == 'Z') {
        memcpy(currPiece, ZPiece, sizeof(ZPiece));
        currNumOrientations = ZMAXORIENTATIONS;
    } else if (piece == 'L') {
        memcpy(currPiece, LPiece, sizeof(LPiece));
        currNumOrientations = LMAXORIENTATIONS;
    } else if (piece == 'J') {
        memcpy(currPiece, JPiece, sizeof(JPiece));
        currNumOrientations = JMAXORIENTATIONS;
    }
    currOrient = SPAWNORIENTATION;
    cursorX = SPAWNCURSORX;
    cursorY = SPAWNCURSORY;
    for (int i = 0; i < PIECEDIMENSIONS; i++) {
        for (int j = 0; j < PIECEDIMENSIONS; j++) {
            if (boardArr[cursorY + i][cursorX + j] == 1 && currPiece[currOrient][i][j] == 1) {
                gameOver = true;
            }
        }
    }
}

void boardMapper() {
    memcpy(tempBoardArr, boardArr, sizeof(boardArr));
    for (int i = 0; i < PIECEDIMENSIONS; i++) {
        for (int j = 0; j < PIECEDIMENSIONS; j++) {
            if (currPiece[currOrient][i][j] == 1) {
                tempBoardArr[cursorY + i][cursorX + j] = currPiece[currOrient][i][j];
            }
        }
    }
}

void displayBoard(int modifier) {
    clear();
    if (modifier == 0) {
        boardMapper();
    }
    for (int i = 0; i < ROWS; i++) {
        printw("<!");
        for (int j = 2; j < TOTALCOLUMNS - 1; j++) { // 2 is to account for the border buffer
            if (tempBoardArr[i][j] == 0) {
                printw(" .");
            } else if (tempBoardArr[i][j] == 2) { // legacy feature for now
                printw("==");
            } else if (tempBoardArr[i][j] == 3) { // legacy feature for now
                printw("++");
            } else if (tempBoardArr[i][j] == 4) { // legacy feature for now
                printw("--");
            } else {
                printw("[]");
            }
        }
        printw("!>\n");
    }
    debugNumber1 = score;
    debugNumber2 = level;
    printw("<!====================!>\n%2d\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/%d|%d\n", debugNumber2, debugNumber1, level);
}

void completionAnimation(int linesComplete[], int currLinesCleared) {
    for (int i = 0; i < 5; i++) { 
        for (int j = 0; j < ROWS; j++) {
            if (linesComplete[j]) {
                memcpy(tempBoardArr[j], rowFlash[i], sizeof(rowFlash[i]));
            }
        }
        displayBoard(1);
        clock_t flashDelay = clock() + .12 * CLOCKS_PER_SEC * 1.1;// currLinesCleared;  // half a second per block
        while (1) {
            if (clock() >= flashDelay) {
                refresh();
                break;
            }
        }
    }
}

void checkLines() {
    int linesComplete[ROWS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bool isAnyComplete = false;
    bool isLineComplete;
    int currLinesCleared = 0;
    for (int i = 0; i < ROWS; i++) {
        isLineComplete = true;
        for (int j = 2; j < TOTALCOLUMNS - 1; j++) {
            if (boardArr[i][j] == 0) {
                isLineComplete = false;
                break;
            }
        }
        if (isLineComplete) {
            linesComplete[i] = 1;
            isAnyComplete = true;
            linesCleared++;
            currLinesCleared++;
        }
    }
    if (isAnyComplete) {
        completionAnimation(linesComplete, currLinesCleared);
        memcpy(tempBoardArr, emptyBoardArr, sizeof(emptyBoardArr));
        int j = 19;
        for (int i = ROWS; i >= 0; i--) {
            if (!linesComplete[i]) {
                memcpy(tempBoardArr[j], boardArr[i], sizeof(boardArr[i]));
                j--;
            }
        }
        memcpy(boardArr, tempBoardArr, sizeof(tempBoardArr));
        score = score + (currLinesCleared*(level + 1) * 100);
        debugNumber1 = score;
        displayBoard(0);
    }
}

void levelChecker() {
    if (goal == 2) {
        if (linesCleared >= (level * 5) || level != 29) {
            level++;
        }
    } else {
        if (linesCleared >= fixedLinesRequired) {
            level++;
            fixedLinesRequired = fixedLinesRequired + 10;
        }
    }
}

bool canRotate() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (boardArr[cursorY + i][cursorX + j] == 1 && currPiece[(currOrient + 1) % currNumOrientations][i][j] == 1) {
                return false;
            }
        }
    }
    return true;
}

bool canMoveLR(int direction) {
    // -1 -> Left
    // +1 -> Right
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (boardArr[cursorY + i][cursorX + j + direction] == 1 && currPiece[currOrient][i][j] == 1) {
                return false;
            }
        }
    }
    return true;
}

bool canMoveDown(int extra) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (boardArr[cursorY + i + extra][cursorX + j] == 1 && currPiece[currOrient][i][j] == 1) {
                return false;
            }
        }
    }
    return true;
}

void movePiece(int key) {
    switch (key) {
        case 'a':
            if (canMoveLR(-1) && !hardDropLock) {
                cursorX--; 
            }
            break;
        case 'd':
            if (canMoveLR(1) && !hardDropLock) {
                cursorX++;
            }
            break;
         
        case 'w':
            if (canRotate()) {
                currOrient = (currOrient + 1) % currNumOrientations;
            }
            break;
        case 's':
            if (canMoveDown(0)) {
                cursorY++;
            }
            if (!canMoveDown(0)) {
                memcpy(boardArr, tempBoardArr, sizeof(tempBoardArr));
                checkLines();
                levelChecker();
                spawnPiece(pieceGen());
            }
            break;
        case ' ':
            hardDropLock = true;
            while (true) {
                if (canMoveDown(1)) {
                    movePiece('s');
                }
                if (!canMoveDown(1)) {
                    break;
                }
                score = score + 2;
            }
            break;
        case KEY_LEFT:
            if (canMoveLR(-1) && !hardDropLock) {
                cursorX--; 
            }
            break;
        case KEY_RIGHT:
            if (canMoveLR(1) && !hardDropLock) {
                cursorX++;
            }
            break;
         
        case KEY_UP :
            if (canRotate()) {
                currOrient = (currOrient + 1) % currNumOrientations;
            }
            break;
        case KEY_DOWN:
            if (canMoveDown(0)) {
                cursorY++;
            }
            if (!canMoveDown(0)) {
                memcpy(boardArr, tempBoardArr, sizeof(tempBoardArr));
                checkLines();
                levelChecker();
                spawnPiece(pieceGen());
            }
            break;
    }
    displayBoard(0);
}

void playGame() {
    srand(time(NULL));
    initscr();
    refresh();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    gameOver = false;
    int c;
    spawnPiece(pieceGen());
    clock_t target = clock() + gravityArr[level] * CLOCKS_PER_SEC;
    displayBoard(0);
    while (gameOver != true) {
        if ((c = getch()) != ERR) {
            if (level > 7) {
                target = clock() + .15 * CLOCKS_PER_SEC;
            }
            if (c  == 's') {
                score++;
            }
            movePiece(c);
        }
        if (clock() >= target) {
            movePiece('s');
            refresh();
            target = clock() + gravityArr[level] * CLOCKS_PER_SEC;
        }
    }
}

void initBoard() {
    for (int i = 0; i < TOTALROWS; i++) {
        for (int j = 0; j < TOTALCOLUMNS; j++) {
            if (j < 2 || j == TOTALCOLUMNS - 1 || i == ROWS) {    // if row apart of the buffer (2 leftmost + 1 rightmost) or if its the bottom buffer row
                boardArr[i][j] = 1;
            } else {
                boardArr[i][j] = 0;
            }
        }
    }
    memcpy(emptyBoardArr, boardArr, sizeof(boardArr));
}

void startingScreen() {
    printf("Please select what level you would like to start on (0-29): ");
    scanf("%d", &startLevel);
    level = startLevel;
    printf("\n1: Normal\n2: Zen\n3: Randomized\nPlease select what mode you would like to play: ");
    scanf("%d", &mode);
    printf("\nPlease select how many pieces you would like to be able to preview (0-5): ");
    scanf("%d", &preview);
    printf("\n1: Random\n2: 7 Bag\nPlease select how many how you would like your pieces to be randomized: ");
    scanf("%d", &rng);
    printf("\n1: Fixed Goal\n2: Variable Goal\nPlease select what leveling system you would like to play: ");
    scanf("%d", &goal);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        startingScreen();
    }
    initBoard();
    playGame();
    endwin();
    printf("Your Score: %d\n", score);
    return 0;
}
 