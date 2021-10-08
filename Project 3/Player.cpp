// Player.cpp

#include "provided.h"
#include <string>
#include <iostream>
#include <list>
#include <vector>
using namespace std;

enum state { //State is used in SmartPlayer, and describes all the possibles states of the game at any time
    PLAYING, REDWIN, BLACKWIN, TIE
};

class HumanPlayerImpl
{
  public:
    int chooseMove(const Scaffold& s, int N, int color);
    bool isInteractive() const { return true; }
};

class BadPlayerImpl
{
  public:
    int chooseMove(const Scaffold& s, int N, int color);
    bool isInteractive() const { return false; }
};

class SmartPlayerImpl
{
  public:
    int chooseMove(const Scaffold& s, int N, int color);
    bool isInteractive() const { return false; }
    int findBestMove(const Scaffold& s, int N, int color, AlarmClock& ac);
    int evaluateState(const Scaffold& scaf, int N, int color, int depth, AlarmClock& ac);
    int miniMax(Scaffold& s, int N, int color, int depth, AlarmClock& ac);
    state checkState(const Scaffold& scaf, int N);
    
    
};




int HumanPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    if (s.numberEmpty() == 0) {
        return 0;
    }
    int avaliable = 0;
    string action = "";
    cout << "Select which column to play:" << endl; //Prompt for user input 
    std::getline(cin, action);
    cout << endl;

    int selectedCol = stoi(action);

    while (selectedCol < 1 || selectedCol > s.cols() || avaliable < 1) { //Invalid Col loop
        cout << "Invalid column" << endl;
        cout << "Select which column to play:" << endl; //Prompt for user input 
        std::getline(cin, action);
        cout << endl;

        selectedCol = stoi(action);


        avaliable = 0;
        if (selectedCol <= s.cols() && selectedCol > 0) { //Checking if col is full or not
            for (int j = 1; j <= s.levels(); j++) {
                if (s.checkerAt(selectedCol, j) == VACANT) {
                    avaliable++;
                }
            }
        }
    }

    

    

    return selectedCol;  
}

int BadPlayerImpl::chooseMove(const Scaffold& s, int N, int color) //BadPlayer always plays left-most avaliable column
{
    if (s.numberEmpty() == 0) {
        return 0;
    }

    for (int i = 1; i <= s.cols(); i++) { 
        for (int j = 1; j <= s.levels(); j++) {
            if (s.checkerAt(i, j) == VACANT) {
                return i; //Returns leftmost vacant column
            }
        }
    }


    return 0;  
}


int SmartPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    AlarmClock ac(8750); //AlarmClock is set for 8750ms as the algorithm as it was tested to be a safe value for not exceeding 10 seconds

    return findBestMove(s,  N,  color, ac);

}

int SmartPlayerImpl::findBestMove(const Scaffold& s, int N, int color, AlarmClock& ac){ // Finds the best move by calling the miniMax function to test all possible moves


    Scaffold scaf = s;

    int oppColor = RED;

    if (color == RED) {
        oppColor = BLACK;
    }

    int bestScore = -1000; //Set low at first so that a valid move is stored
    vector<int> bestMoves; //A vector of the best moves
    list<int> possibleMoves; //A list of possible moves, used if there is no best move, or time has ran out


    for (int i = 1; i <= scaf.cols(); i++) { //Loop through all columns

        if (ac.timedOut()) { //Timed Out Check, use a bestMove, if none, take the first move in possibleMoves
            
            if (bestMoves.empty()) {
                return possibleMoves.front();
            }

            return bestMoves.front();
        }
        
        if (scaf.makeMove(i, color) == true) {

            possibleMoves.push_back(i); //Adding move into possibleMoves as a possible move

            int score = -miniMax(scaf, N, oppColor, 0, ac); //Calculate the minMax of this move

            if (score > bestScore) { //Stores the score if it's highest so far
                bestScore = score;
                bestMoves.clear();
                bestMoves.push_back(i); //Clears out all other best moves, and adds current move
            }
            else if (score == bestScore) {
                bestMoves.push_back(i); //Add this equally good move into the bestMoves vector
            }

            scaf.undoMove(); //Return Scaffold back to the original state


        }
    }

    if (bestMoves.empty()) { //No best moves, therefore use first possible move
        return possibleMoves.front();
    }

    return bestMoves.front();

    
}


int SmartPlayerImpl::miniMax(Scaffold& s, int N, int color, int depth, AlarmClock& ac) { //miniMax algorithm with depth adjustment
    
    if (ac.timedOut()) {
        
        return 0;
    }

    int oppColor = RED;

    if (color == RED) {
        oppColor = BLACK;
    }

    int moveVal = evaluateState(s, N, color, depth, ac); //Evaluated the move by observing the state of the game
    if (moveVal != -1) {
        return moveVal; //Win, Loss, or Draw
    }
    
    //Not a terminal node, then continue:

    int bestScore = -1000;

    for (int i = 1; i <= s.cols(); i++) { //Loop through all columns

        if (ac.timedOut()) {
           
            return 0;
        }

        if (s.makeMove(i, color) == true) { //Recursively calls itself to evaluate opponents move

            int score = -miniMax(s, N, oppColor, depth + 1, ac); 
            if (score > bestScore) {
                bestScore = score;
            }
            s.undoMove();

        }
    }

    return bestScore;

}

int SmartPlayerImpl::evaluateState(const Scaffold& scaf, int N, int color, int depth, AlarmClock& ac) { //Evaluates the state of the game, assigning interger values to who it favors

    if (ac.timedOut()) {
        
        return 0;
    }

    state currentState = checkState(scaf, N); //Checks what the state of the game is
    

    if (currentState != PLAYING) {

        if ((color == RED && currentState == REDWIN) || (color == BLACK && currentState == BLACKWIN))
            return 100000 - depth; //Player won the game, returns large number adjusted with depth

        else if ((color == BLACK && currentState == REDWIN) || (color == RED && currentState == BLACKWIN))
            return -100000 + depth; //Player lost the game, returns large number adjusted with depth

        else if (currentState == TIE)
            return 0;
    }
    return -1; //Game is ongoing

}

state SmartPlayerImpl::checkState(const Scaffold& scaf, int N) { 
    //Almost identical to Game's completed fucntion, except it assigns a "state" enum for each possbile outcome
   

    for (int i = 1; i <= scaf.levels(); i++) {
        for (int j = 1; j <= scaf.cols(); j++) {


            //Horizontal Checks

            if (scaf.checkerAt(j, i) == RED) {
                int horizCheck = 0;
                for (int k = j; k <= (scaf.cols()); k++) {
                    if (scaf.checkerAt(k, i) == RED) {
                        horizCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (horizCheck >= N) {
                    return REDWIN;
                }
            }
            if (scaf.checkerAt(j, i) == BLACK) {
                int horizCheck = 0;
                for (int k = j; k <= (scaf.cols()); k++) {
                    if (scaf.checkerAt(k, i) == BLACK) {
                        horizCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (horizCheck >= N) {
                    return BLACKWIN;
                }
            }

            //Vertical Checks
            if (scaf.checkerAt(j, i) == RED) {
                int vertCheck = 0;
                for (int k = i; k <= (scaf.levels()); k++) {
                    if (scaf.checkerAt(j, k) == RED) {
                        vertCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (vertCheck >= N) {
                    return REDWIN;
                }
            }
            if (scaf.checkerAt(j, i) == BLACK) {
                int vertCheck = 0;
                for (int k = i; k <= (scaf.levels()); k++) {
                    if (scaf.checkerAt(j, k) == BLACK) {
                        vertCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (vertCheck >= N) {
                    return BLACKWIN;
                }
            }

            //Up-Right Diagonal Check

            if (scaf.checkerAt(j, i) == RED) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf.cols()) && l <= (scaf.levels()); k++, l++) {
                    if (scaf.checkerAt(k, l) == RED) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }

                }
                if (diagCheck >= N) {
                    return REDWIN;
                }
            }

            if (scaf.checkerAt(j, i) == BLACK) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf.cols()) && l <= (scaf.levels()); k++, l++) {
                    if (scaf.checkerAt(k, l) == BLACK) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }

                }
                if (diagCheck >= N) {
                    return BLACKWIN;
                }
            }

            //Down-Right Checks

            if (scaf.checkerAt(j, i) == RED) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf.cols()) && l >= 1; k++, l--) {
                    if (scaf.checkerAt(k, l) == RED) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }

                }
                if (diagCheck >= N) {
                    return REDWIN;
                }
            }

            if (scaf.checkerAt(j, i) == BLACK) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf.cols()) && l >= 1; k++, l--) {
                    if (scaf.checkerAt(k, l) == BLACK) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }

                }
                if (diagCheck >= N) {
                    return BLACKWIN;
                }
            };
        };
    }


    if (scaf.numberEmpty() == 0) { //Full scaf, tie game
        return TIE;
    }

   
   
    return PLAYING;  
}

//******************** Player derived class functions *************************

//  These functions simply delegate to the Impl classes' functions.
//  You probably don't want to change any of this code. 

HumanPlayer::HumanPlayer(string nm)
 : Player(nm)
{
    m_impl = new HumanPlayerImpl;
}
 
HumanPlayer::~HumanPlayer()
{
    delete m_impl;
}
 
int HumanPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}

BadPlayer::BadPlayer(string nm)
 : Player(nm)
{
    m_impl = new BadPlayerImpl;
}
 
BadPlayer::~BadPlayer()
{
    delete m_impl;
}
 
int BadPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}

SmartPlayer::SmartPlayer(string nm)
 : Player(nm)
{
    m_impl = new SmartPlayerImpl;
}
 
SmartPlayer::~SmartPlayer()
{
    delete m_impl;
}
 
int SmartPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}
