/* Logan Seaburg
 * I used:
 * Lecture Notes
 * Stanford C++ Lib and references on cs106b.stanford.edu
 * The Stanford C++ "random.h" library and references
 *
 * --Extras--
 * this program is capable of generating random worlds
 *
 * this program will output the board on the console window, but it can also  output via the gui
 *   by typing 'show GUI.' Because the GUI takes longer to process than the plaintext, the built in
 *   pauses in the animation have been removed
 *
 * by typing "wrap-around" on the main menu screen, where it asks you to
 *   animate, tick, or quit, you can enable the board to act as if it can wrap-around
 *   typing "wrap-around" again will disable this feature
 *
 * both the 'show GUI' and 'wrap-around' commands are case-insensitive
*/

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "grid.h"
#include "gwindow.h"
#include "simpio.h"
#include "lifegui.h"
#include "random.h"
using namespace std;

void intro();
void getFile(Grid<int>& board);
void readFile(Grid<int>& board, string fileName);
void displayBoard(Grid<int>& board, LifeGUI& gui, bool showGUI);
void newGeneration(Grid<int>& board, bool wrapAround);
int isAlive(int r, int c, Grid<int>& board, bool wrapAround);
void menu(Grid<int>& board);

int main() {
    Grid<int> board;
    intro();
    getFile(board);
    menu(board);
    cout << "Have a nice Life!" << endl;
    return 0;
}

//prints the heading on the top of the page with the rules of the game
void intro() {
   cout << "Welcome to the CS 106B Game of Life," << endl
          << "a simulation of the lifecycle of a bacteria colony."  << endl
          << "Cells (X) live and die by the following rules:" << endl
          << "- A cell with 1 or fewer neighbors dies." << endl
          << "- Locations with 2 neighbors remain stable." << endl
          << "- Locations with 3 neighbors will create life." << endl
          << "- A cell with 4 or more neighbors dies." << endl
          << "Additionally, typing 'wrap-around' makes it as if the cells can go around the board," << endl
          << "typing 'show GUI' will turn on the nicer display of the GUI," << endl
          << "and typing 'random' instead of a file name generates a random world." << endl
          << endl;
   return;
}

//reads the file and stores the input into the grid that is passed to it
void getFile(Grid<int>& board) {
    string fileName;

    //gets the name of the file
    fileName = getLine ("Grid input file name? ");
    while (! (fileExists(fileName) || fileName == "random") ) {
        cout << "Unable to open that file.  Try again." << endl;
        fileName = getLine ("Grid input file name? ");
    }
    if (fileName == "random") {
        board.resize(randomInteger(5, 25), randomInteger(5, 25));
        for(int r = 0; r < board.height(); r++) {
            for(int c = 0; c < board.width(); c++) {
                    board[r][c] = randomInteger(0, 1);
                }
            }
    } else {
        readFile(board, fileName);
    }
    return;
}

//stores the contents of the file into the board that is passed to it
void readFile(Grid<int>& board, string fileName) {
    int totalRows, totalColumns;
    string tempRow, tempColumn, tempLine;
    ifstream input;

    openFile(input, fileName);

    getline(input, tempRow);
    totalRows = stringToInteger(tempRow);

    getline(input, tempColumn);
    totalColumns = stringToInteger(tempColumn);
    board.resize(totalRows, totalColumns);

    //fills the grid with the values
    for(int r = 0; r < totalRows; r++) {
        getline(input, tempLine);
        for(int c = 0; c < totalColumns; c++) {
            if (tempLine[c] == '-') {
                board[r][c] = 0;
            } else {
                board[r][c] = 1;
            }
        }
    }
    input.close();
}

//outputs the board that is passed to it to the console window
//also will display on the GUI that is passed to it depending on the value
//      of the third parameter, if the user has already selected 'show GUI'
void displayBoard(Grid<int>& board, LifeGUI& gui, bool showGUI) {
    for(int r = 0; r < board.height(); r++) {
        for(int c = 0; c < board.width(); c++) {
            if (board[r] [c] == 0) {
                if (showGUI) {
                  gui.drawCell(r, c, false);
                }
                cout << "-";
            } else {
                if (showGUI) {
                  gui.drawCell(r, c, true);
                }
                cout << "X";
            }
        }
        cout << endl;
    }
}

//will update board to it's next phase after one generation
//the first parameter is the board it is working on
//the second parameter is the setting if top cells affect bottom and so on
void newGeneration(Grid<int>& board, bool wrapAround) {
    Grid<int> temp = board;
    for(int r = 0; r < board.height(); r++) {
        for(int c = 0; c < board.width(); c++) {
            board[r] [c] = isAlive(r, c, temp, wrapAround);
        }
    }
}

//takes in the row position, column position, and the board
//returns a 1 if the cell will be alive and a 0 if the cell will die
int isAlive(int r, int c, Grid<int>&board, bool wrapAround) {
    //TODO - incorporate the wrap around feature
    int neighbors = 0;
    //loops through all the neighboring cells and counts how many are alive
    for(int i = r - 1; i <= r + 1; i++) {
        for(int j = c - 1; j <= c + 1; j++) {
            if (board.inBounds(i, j) ) {
                neighbors += board[i] [j];
            } else if (wrapAround) {
                int tempI = i;
                int tempJ = j;
                if (i < 0) {
                    tempI = board.height() - 1;
                } else if (i >= board.height()) {
                    tempI = 0;
                }
                if (j < 0) {
                    tempJ = board.width() - 1;
                } else if (j >= board.width()){
                    tempJ = 0;
                }
                neighbors += board[tempI] [tempJ];
            }
        }
    }
    neighbors -= board[r] [c];
    //rules of the game that decide how many neighbors you need to live
    if (neighbors <= 1 || neighbors >= 4) {
        return 0;
    } else if (neighbors == 2) {
        return board[r] [c];
    } else {
        return 1;
    }
}

//operates as the interactive part of the program and handels the animations
void menu(Grid<int>& board) {
    LifeGUI gui;
    string answer;
    int frames;
    bool wrapAround = false, showGUI = false;
    gui.resize(board.height(), board.width());
    displayBoard(board, gui, showGUI);
    do {
        answer = getLine("a)nimate, t)ick, q)uit? ");
        if (toLowerCase(answer) == "a") {
            frames = getInteger("How many frames? ");
            for(int i = 0; i < frames; i++) {
                clearConsole();
                newGeneration(board, wrapAround);
                displayBoard(board, gui, showGUI);
                if (!showGUI) {
                   pause(50);
                }
            }
        } else if (toLowerCase(answer) == "t") {
            newGeneration(board, wrapAround);
            displayBoard(board, gui, showGUI);
        } else if (toLowerCase(answer) == "wrap-around") {
            wrapAround = ! wrapAround;
            if (wrapAround) {
                cout << "The wrap-around function has been enabled." << endl;
            } else {
                cout << "The wrap-around function has been disabled." << endl;
            }
        } else if (toLowerCase(answer) == "show gui")  {
            showGUI = !showGUI;
            if (showGUI) {
                cout << "The GUI display has been enabled." << endl;
            } else {
                cout << "The GUI display has been disabled." <<endl;
            }
        } else if (toLowerCase(answer) != "q") {
            cout << "Invalid choice; please try again." << endl;
        }
    } while (toLowerCase(answer) != "q");
}
