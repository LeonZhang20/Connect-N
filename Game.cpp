// Game.cpp

#include "GameCore.h"
#include <stack>
#include <iostream>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nColumns, int nLevels, int N, Player* red, Player* black);
    ~GameImpl();
    bool completed(int& winner) const;
    bool takeTurn();
    void play();
    int checkerAt(int c, int r) const;
private:
    Scaffold* scaf;
    Player* redP;
    Player* blackP;
    int connectN; //number of checkers needed to win
    stack<int> turnHistory; // Records turn order, like the move history vector in Scaffold
   
};

GameImpl::GameImpl(int nColumns, int nLevels, int N, Player* red, Player* black)
{
    scaf = new Scaffold(nColumns, nLevels);
    redP = red;
    blackP = black;
    connectN = N;


    turnHistory.push(RED); //This will tell the algorithm that RED should go next
}

GameImpl::~GameImpl()
{
    delete scaf;
}

bool GameImpl::completed(int& winner) const
{
    

    for (int i = 1; i <= scaf->levels(); i++) { //Looping through all possible locations
        for (int j = 1; j <= scaf->cols(); j++) {


            //Horizontal Checks

            if (scaf->checkerAt(j, i) == RED) {
                int horizCheck = 0;
                for (int k = j; k <= (scaf->cols()); k++) {
                    if (scaf->checkerAt(k, i) == RED) {
                        horizCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (horizCheck >= connectN) {
                    winner = RED;
                    return true;
                }
            }
            if (scaf->checkerAt(j, i) == BLACK) {
                int horizCheck = 0;
                for (int k = j; k <= (scaf->cols()); k++) {
                    if (scaf->checkerAt(k, i) == BLACK) {
                        horizCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (horizCheck >= connectN) {
                    winner = BLACK;
                    return true;
                }
            }

            //Vertical Checks
            if (scaf->checkerAt(j, i) == RED) {
                int vertCheck = 0;
                for (int k = i; k <= (scaf->levels()); k++) {
                    if (scaf->checkerAt(j, k) == RED) {
                        vertCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (vertCheck >= connectN) {
                    winner = RED;
                    return true;
                }
            }
            if (scaf->checkerAt(j, i) == BLACK) {
                int vertCheck = 0;
                for (int k = i; k <= (scaf->levels()); k++) {
                    if (scaf->checkerAt(j, k) == BLACK) {
                        vertCheck++;
                    }
                    else {
                        break;
                    }
                }
                if (vertCheck >= connectN) {
                    winner = BLACK;
                    return true;
                }
            }

            //Up-Right Diagonal Check

            if (scaf->checkerAt(j, i) == RED) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf->cols()) && l <= (scaf->levels()); k++, l++) {
                    if (scaf->checkerAt(k, l) == RED) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }
                    
                }
                if (diagCheck >= connectN) {
                    winner = RED;
                    return true;
                }
            }

            if (scaf->checkerAt(j, i) == BLACK) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf->cols()) && l <= (scaf->levels()); k++, l++) {
                    if (scaf->checkerAt(k, l) == BLACK) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }

                }
                if (diagCheck >= connectN) {
                    winner = BLACK;
                    return true;
                }
            }

            //Down-Right Diagonal Checks

            if (scaf->checkerAt(j, i) == RED) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf->cols()) && l >= 1; k++, l--) {
                    if (scaf->checkerAt(k, l) == RED) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }

                }
                if (diagCheck >= connectN) {
                    winner = RED;
                    return true;
                }
            }

            if (scaf->checkerAt(j, i) == BLACK) {
                int diagCheck = 0;
                for (int k = j, l = i; k <= (scaf->cols()) && l >= 1; k++, l--) {
                    if (scaf->checkerAt(k, l) == BLACK) {
                        diagCheck++;
                    }
                    else {
                        break;
                    }

                }
                if (diagCheck >= connectN) {
                    winner = BLACK;
                    return true;
                }
            }
        }
    }


    if (this->scaf->numberEmpty() == 0) { //Full scaf, tie game
        winner = TIE;
        return true;
    }

   

    return false;  
}

bool GameImpl::takeTurn()
{
    int a = 0; //arbitrary integer used to check completed
    if (this->completed(a) == true) {
        
        return false; 
    }
    if (turnHistory.top() == RED) { //Use turnHistory stack to determine who goes next
        cout << redP->name() << "'s Turn!" << endl;
        scaf->makeMove(redP->chooseMove(*scaf, connectN, RED), RED);
        turnHistory.push(BLACK); //Pushing opponents color into the stack to go next
        
        return true;
    }
    else if (turnHistory.top() == BLACK) {
        cout << blackP->name() << "'s Turn!" << endl;
        scaf->makeMove(blackP->chooseMove(*scaf, connectN, BLACK), BLACK);
        turnHistory.push(RED);
      
        return true;
    }
    
    return false;  
}

void GameImpl::play()
{
    if (redP->isInteractive() == false && blackP->isInteractive() == false) { //Two bots playing
        int outcome = 69;
        while (this->completed(outcome) == false) {
            scaf->display();
            cout << "Press enter to continue." << endl; //Stops
            string trash;
            getline(cin, trash); //Just to allow user to observe one move at a time
            this->takeTurn();
         

        }
        scaf->display();
        if (outcome == RED) {
            cout << this->redP->name() << " (RED) won!" << endl;
            return;
        }
        if (outcome == BLACK) {
            cout << this->blackP->name() << " (BLACK) won!" << endl;
            return;
        }
        if (outcome == TIE) {
            cout << "Tied Game!" << endl;
            return;
        }

    }
    else { // At least one human playing
        int outcome = 69;
        while (this->completed(outcome) == false) {
            scaf->display(); //Displays before move is made
            this->takeTurn();
        }
        scaf->display(); //Displays made move
        if (outcome == RED) {
            cout << this->redP->name() << " (RED) won!" << endl;
            return;
        }
        if (outcome == BLACK) {
            cout << this->blackP->name() << " (BLACK) won!" << endl;
            return;
        }
        if (outcome == TIE) {
            cout << "Tied Game!" << endl;
            return;
        }
    } 


}

int GameImpl::checkerAt(int c, int r) const
{
    return scaf->checkerAt(c, r);
} 


Game::Game(int nColumns, int nLevels, int N, Player* red, Player* black)
{
    m_impl = new GameImpl(nColumns, nLevels, N, red, black);
}
 
Game::~Game()
{
    delete m_impl;
}
 
bool Game::completed(int& winner) const
{
    return m_impl->completed(winner);
}

bool Game::takeTurn()
{
    return m_impl->takeTurn();
}

void Game::play()
{
    m_impl->play();
}
 
int Game::checkerAt(int c, int r) const
{
    return m_impl->checkerAt(c, r);
}

