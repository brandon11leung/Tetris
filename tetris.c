/*******************************************************************************
 * Name        : tetris.c
 * Author      : Brandon Leung
 * Version     : 0.0.0
 * Date        : February 25, 2023
 * Description : An ASCII Art port of Tetris in C.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>

#define SPAWNCURSORX 5
#define SPAWNCURSORY 0
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

int rowFlash0[13] = {1,1,0,0,0,0,0,0,0,0,0,0,1};
int rowFlash1[13] = {1,1,1,1,1,1,1,1,1,1,1,1,1};
int rowFlash2[13] = {1,1,2,2,2,2,2,2,2,2,2,2,1};
int previewPiece
int fixedLinesRequired = 10;
int score = STARTINGSCORE;
int linesCleared = STARTINGLINES;
int debugNumber1 = 0;
int debugNumber2 = 0;
int spawnCordX = 3;
int spawnCordY = 0;
int cursorX = 3;
int cursorY = 0;
int currNumOrientations;
int currPiece[4][4][4];
int tryPiece[4][4];
int currOrient;
int startLevel = 0;
int level;
int mode;
int preview;
int rng = 1;
int goal;
bool gameOver;
double elapsedTime= 0.0;
char basePieceArr[7] = {'I', 'O', 'T', 'S', 'Z', 'L', 'J'};
char tempPieceArr[7];
int boardArr[21][13];
int emptyBoardArr[21][13];
int tempBoardArr[21][13];
int testBoardArr[21][13];
int gravityArr[30] = {799, 715, 632, 549, 466, 383, 300, 216, 133, 100,     // Levels 0-9
                       83, 83, 83,                                          // Levels 10-12
                       67, 67, 67,                                          // Levels 13-15
                       50, 50, 50,                                          // Levels 16-18
                       33, 33, 33, 33, 33, 33, 33, 33, 33, 33,              // Levels 19-28
                       17};                                                 // Levels 29+

int IPiece[2][4][4] = {{{0, 0, 0, 0},
                        {1, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0}}};

int OPiece[1][4][4] = {{{0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}}}; 

int TPiece[4][4][4] = {{{0, 0, 0, 0},
                        {0, 1, 1, 1},
                        {0, 0, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},
                        {0, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},
                        {0, 0, 1, 1},
                        {0, 0, 1, 0},
                        {0, 0, 0, 0}}};

int SPiece[2][4][4] = {{{0, 0, 0, 0},
                        {0, 0, 1, 1},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},
                        {0, 0, 1, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 0}}};

int ZPiece[2][4][4] = {{{0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {0, 0, 1, 1},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 1},
                        {0, 0, 1, 1},
                        {0, 0, 1, 0},
                        {0, 0, 0, 0}}};

int LPiece[4][4][4] = {{{0, 0, 0, 0},
                        {0, 1, 1, 1},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}},
                       {{0, 1, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 1},
                        {0, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 1},
                        {0, 0, 0, 0}}};
                      
int JPiece[4][4][4] = {{{0, 0, 0, 0},
                        {0, 1, 1, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 1, 0, 0},
                        {0, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 1},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 0, 0}}};


void initBoard() {
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 13; j++) {
            if (j < 2 || j == 12 || i == 20) {
                boardArr[i][j] = 1;
            } else {
                boardArr[i][j] = 0;
            }
        }

    }
    memcpy(emptyBoardArr, boardArr, sizeof(boardArr));
    cursorX = 3;
    cursorY = 0;
}

char pieceGen() {
    char nextPiece;
    if (rng == 2) {
        if (tempPieceArr[6] == 'X') {
            memcpy(tempPieceArr, basePieceArr, sizeof(basePieceArr));
            for (int i = 0; i < 7; i++) {
                int j = rand() % 7;
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
        int randNum = rand() % 7;
        nextPiece = basePieceArr[randNum];
    }
    return nextPiece;
}

// void startingScreen() {
//     printf("Please select what level you would like to start on (0-29): ");
//     scanf("%d", &startLevel);
//     printf("\n1: Normal\n2: Zen\n3: Randomized\nPlease select what mode you would like to play: ");
//     scanf("%d", &mode);
//     printf("\nPlease select how many pieces you would like to be able to preview (0-5): ");
//     scanf("%d", &preview);
//     printf("\n1: Random\n2: 7 Bag\nPlease select how many how you would like your pieces to be randomized: ");
//     scanf("%d", &rng);
//     printf("\n1: Fixed Goal\n2: Variable Goal\nPlease select what leveling system you would like to play: ");
//     scanf("%d", &goal);
// }

void boardMapper() {
    memcpy(tempBoardArr, boardArr, sizeof(boardArr));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currPiece[currOrient][i][j] == 1) {
                tempBoardArr[cursorY + i][cursorX + j] = currPiece[currOrient][i][j];
            }
        }
    }
}

void displayBoardNoMap() {
    clear();
    for (int i = 0; i < 20; i++) {
        printw("<!");
        for (int j = 2; j < 12; j++) {
            if (tempBoardArr[i][j] == 0) {
                printw(" .");
            } else if (tempBoardArr[i][j] == 2) {
                printw("==");
            } else {
                printw("[]");
            }
        }
        printw("!>\n");
    }
    printw("<!====================!>\n%2d\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/%d|%d\n", debugNumber2, debugNumber1, level);
    
}

void displayBoard() {
    clear();
    boardMapper();
    for (int i = 0; i < 20; i++) {
        printw("<!");
        for (int j = 2; j < 12; j++) {
            if (tempBoardArr[i][j] == 0) {
                printw(" .");
            } else if (tempBoardArr[i][j] == 2) {
                printw("==");
            } else {
                printw("[]");
            }
        }
        printw("!>\n");
    }
    printw("<!====================!>\n%2d\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/%d|%d\n", debugNumber2, debugNumber1, level);
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
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (boardArr[cursorY + i][cursorX + j] == 1 && currPiece[currOrient][i][j] == 1) {
                gameOver = true;
            }
        }
    }
}

void resetTimer() {
    time_t startTime = time(NULL);
    elapsedTime = 0.0;
}

void checkLines() {
    int linesComplete[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bool isAnyComplete = false;
    bool isLineComplete;
    int currLinesCleared = 0;
    for (int i = 0; i < 20; i++) {
        isLineComplete = true;
        for (int j = 2; j < 12; j++) {
            if (boardArr[i][j] == 0) {
                isLineComplete = false;
                break;
            }
        }
        if (isLineComplete) {isAnyComplete = true;}
        if (isLineComplete) {
            linesCleared++;
            currLinesCleared++;
            //debugNumber2++;
            linesComplete[i] = 1;
        }
    }
    if (isAnyComplete) {
        // Commented code below is to provide a flashing effect on completed rows
        // cant get the darn thing to work though. It pauses but the display never
        // updates.
        // for (int i = 0; i < 20; i++) {
        //     if (linesComplete[i]) {
        //         memcpy(tempBoardArr[i], rowFlash2, sizeof(rowFlash2));
                
        //         debugNumber2 = boardArr[i][5];
        //     }
        // }
        // endwin();
        // initscr();
        
        // displayBoardNoMap();
        // usleep(1000000); // Wait for 1 second
        
        // for (int i = 0; i < 20; i++) {
        //     if (linesComplete[i]) {
        //         memcpy(boardArr[i], rowFlash1, sizeof(rowFlash1));
        //     }
        // }
        // displayBoardNoMap(); 
        // usleep(1000000);
        //         for (int i = 0; i < 20; i++) {
        //     if (linesComplete[i]) {
        //         memcpy(boardArr[i], rowFlash0, sizeof(rowFlash0));
        //     }
        // }     
        // displayBoardNoMap(); 
        usleep(1000000 * currLinesCleared);
        
        
        int j = 19;
        for (int i = 20; i >= 0; i--) {
            if (!linesComplete[i]) {
                memcpy(tempBoardArr[j], boardArr[i], sizeof(boardArr[i]));
                j--;
            }
        }
        memcpy(boardArr, tempBoardArr, sizeof(tempBoardArr));
        score = score + (currLinesCleared*(level + 1) * 100);
        debugNumber1 = score;
        resetTimer();
        displayBoard();

    }
}

bool canMoveDown() {
    memcpy(testBoardArr, boardArr, sizeof(boardArr));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (boardArr[cursorY + i][cursorX + j] == 1 && currPiece[currOrient][i][j] == 1) {
                return false;
            }
        }
    }
    return true;
}

bool canMoveLR(int direction) {
    // +1 -> Right
    // -1 -> Left
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (boardArr[cursorY + i][cursorX + j + direction] == 1 && currPiece[currOrient][i][j] == 1) {
                return false;
            }
        }
    }
    return true;
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

void movePiece(int key) {
    switch (key) {
        case 'a':
            if (canMoveLR(-1)) {
                cursorX--; 
            }
            break;
        case 'd':
            if (canMoveLR(1)) {
                cursorX++;
            }
            break;
         
        case 'w' :
            if (canRotate()) {
                currOrient = (currOrient + 1) % currNumOrientations;
            }
            break;
        case 's':
            if (canMoveDown()) {
                elapsedTime = 0.0;
                cursorY++;
            }
            if (!canMoveDown()) {
                memcpy(boardArr, tempBoardArr, sizeof(tempBoardArr));
                checkLines();
                levelChecker();
                spawnPiece(pieceGen());
            }
            break;
     }   
    displayBoard(); 
}



void playGame() {
    srand(time(NULL));
    initscr();
    refresh();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    gameOver = false;
    int level = startLevel;
    int c;
    struct timeval before, after;
    nodelay(stdscr, TRUE);
    time_t startTime = time(NULL);
    spawnPiece(pieceGen());
    displayBoard();
    while (gameOver != true) {
        if ((c = getch()) != ERR) {
            if (c  == 's') {
                startTime = time(NULL);
                elapsedTime = 0.0;
                score++;
                debugNumber1++;
            }
            movePiece(c);
        }
        elapsedTime = difftime(time(NULL),startTime);
        if (elapsedTime >= gravityArr[level] * .001) {
            movePiece('s');
            startTime = time(NULL);
            elapsedTime = 0.0;
        }
    }
}

int main(int argc, char *argv[]) {
    //startingScreen();
    initBoard();
    playGame();
    endwin();
    printf("Your Score: %d\n", score);
    return 0;
}
 