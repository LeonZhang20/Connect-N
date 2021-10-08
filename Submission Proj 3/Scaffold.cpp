// Scaffold.cpp

#include "provided.h"
#include <iostream>
#include <vector>
#include <stack>
using namespace std;

class ScaffoldImpl
{
  public:
    ScaffoldImpl(int nColumns, int nLevels); 
    int cols() const;
    int levels() const;
    int numberEmpty() const;
    int checkerAt(int column, int level) const; 
    void display() const; 
    bool makeMove(int column, int color); 
    int undoMove(); 

private:
    int mCol;
    int mLev;
    vector<vector<char>> mGrid; //2D Vector Representing the Scaffold
    int arrayWidth; //Properties of the 2D Vector
    int arrayHeight;
    stack<int> moveHistory; //moveHistory Stack
};

ScaffoldImpl::ScaffoldImpl(int nColumns, int nLevels)
{
    if (nColumns > 0 && nLevels > 0) {
        mCol = nColumns;
        mLev = nLevels;
        arrayWidth = 2 * nColumns + 1;
        arrayHeight = nLevels + 1;
        mGrid.resize(arrayWidth);  //Constructing Empty Grid
        for (int i = 0; i < (arrayWidth); i++)
            mGrid[i].resize(arrayHeight);  


        //Adding in bottom row
        for (int i = 0; i < (arrayWidth); i++) {

            if (i == 0) {
                mGrid[i][arrayHeight-1] = '+';
            }
            else {
                if (mGrid[i - 1][arrayHeight - 1] == '+') {
                    mGrid[i][arrayHeight - 1] = '-';
                }
                else {
                    mGrid[i][arrayHeight - 1] = '+';
                }
                
            }
        }

        mGrid[0][0] = '|'; //Corner

        //Adding in vertical walls

        for (int i = 1; i < (arrayWidth); i++) {
            if (mGrid[i - 1][0] == '|') {
                mGrid[i][0] = ' ';
            }
            else {
                mGrid[i][0] = '|';
            }
        }
        for (int i = 0; i < (arrayWidth); i++) {


            for (int j = 1; j < (arrayHeight - 1); j++) {
                mGrid[i][j] = mGrid[i][j - 1];
            }


        }
        
                    
    }
    else {
        cerr << "Invalid Scaffold" << endl;
        exit(1);
    }
}

int ScaffoldImpl::cols() const
{
    return mCol;  
}

int ScaffoldImpl::levels() const
{
    return mLev; 
}

int ScaffoldImpl::numberEmpty() const
{
 

    int counter = 0;

    for (int i = 0; i < (arrayHeight); i++) {


        for (int j = 1; j < (arrayWidth); j++) {
            if (mGrid[j][i] == ' ') {
                counter++;
            }
        }


    }
    return counter;  
}

int ScaffoldImpl::checkerAt(int column, int level) const
{

    int counter = 0;

    int posWidth = 2 * column - 1; 
    int posHeight = (arrayHeight - 1) - level;

    if (mGrid[posWidth][posHeight] == 'R') {
        counter = RED;
    }
    if (mGrid[posWidth][posHeight] == 'B') {
        counter = BLACK;
    }
    if (mGrid[posWidth][posHeight] == ' ') {
        counter = VACANT;
    }

    return counter; 
}

void ScaffoldImpl::display() const
{
    for (int i = 0; i < arrayHeight; i++)
    {
        for (int j = 0; j < arrayWidth; j++)
        {
            cout << mGrid[j][i];
        }
        cout << endl;
    }



}

bool ScaffoldImpl::makeMove(int column, int color)
{
    if (column < 1 || column > mCol) { //Invalid Column
        return false;
    }
    int counter = 0;
    for (int i = mLev; i >= 1; i--) { //Check for vacancy
        if (this->checkerAt(column, i) == VACANT) {
            counter++;
            break;
        }
       
        
    }
    if (counter == 0) { //No vacant spot
        return false;
    }

    int insertLev = 1;

    for (int i = mLev; i >= 1; i--) { //Finding Insert Level, AKA Lowest possible level
        if (this->checkerAt(column, i) == RED || this->checkerAt(column, i) == BLACK) {
            insertLev = i + 1;
            break;
        }
    }


    //Converting Col and Lev into corrected coordinates on 2D Vector

    int posWidth = 2 * column - 1;
    int posHeight = (arrayHeight - 1) - insertLev;

    if (color == RED) { //Inserting values
        mGrid[posWidth][posHeight] = 'R';
        moveHistory.push(column); //Store this move history into the stack
        return true;
    }
    else if (color == BLACK) {
        mGrid[posWidth][posHeight] = 'B';
        moveHistory.push(column);
        return true;
    }

    return false;
  
}

int ScaffoldImpl::undoMove()
{
    if (moveHistory.empty()) {
        return 0;
    }

    int column = moveHistory.top(); //Take the most recent column played in

    int deleteLev = 0;

    for (int i = mLev; i >= 1; i--) { //Finding Insert Level, e.g. highest level with a checker
        if (this->checkerAt(column, i) == RED || this->checkerAt(column, i) == BLACK) {
            deleteLev = i;
            break;
        }
    }

    //Converting Col and Lev into corrected coordinates on 2D Vector
    int posWidth = 2 * column - 1;
    int posHeight = (arrayHeight - 1) - deleteLev;

  
    mGrid[posWidth][posHeight] = ' '; //Deleting the checker
    moveHistory.pop(); //Deleting move history
       
    return column;
    
}

//******************** Scaffold functions *******************************

//  These functions simply delegate to ScaffoldImpl's functions.
//  You probably don't want to change any of this code. 

Scaffold::Scaffold(int nColumns, int nLevels)
{
    m_impl = new ScaffoldImpl(nColumns, nLevels);
}
 
Scaffold::~Scaffold()
{
    delete m_impl;
}
 
Scaffold::Scaffold(const Scaffold& other)
{
    m_impl = new ScaffoldImpl(*other.m_impl);
}
 
Scaffold& Scaffold::operator=(const Scaffold& rhs)
{
    if (this != &rhs)
    {
        Scaffold temp(rhs);
        swap(m_impl, temp.m_impl);
    }
    return *this;
}
 
int Scaffold::cols() const
{
    return m_impl->cols();
}

int Scaffold::levels() const
{
    return m_impl->levels();
}

int Scaffold::numberEmpty() const
{
    return m_impl->numberEmpty();
}

int Scaffold::checkerAt(int column, int level) const
{
    return m_impl->checkerAt(column, level);
}
 
void Scaffold::display() const
{
    m_impl->display();
}
 
bool Scaffold::makeMove(int column, int color)
{
    return m_impl->makeMove(column, color);
}
 
int Scaffold::undoMove()
{
    return m_impl->undoMove();
}
