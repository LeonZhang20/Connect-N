// main.cpp

#include "GameCore.h"
#include <string>
#include <iostream>
#include <cassert>
using namespace std;

#if defined(_WIN32) || defined(_WIN64)
#include <iostream>
#include <windows.h>
#include <conio.h>

struct KeepWindowOpenUntilDismissed
{
	~KeepWindowOpenUntilDismissed()
	{
		DWORD pids[1];
		if (GetConsoleProcessList(pids, 1) == 1)
		{
			std::cout << "Press any key to close this window . . . ";
			_getch();
		}
	}
} keepWindowOpenUntilDismissed;
#endif
void doPlayerTests()
{
	HumanPlayer hp("Marge");
	assert(hp.name() == "Marge" && hp.isInteractive());
	BadPlayer bp("Homer");
	assert(bp.name() == "Homer" && !bp.isInteractive());
	SmartPlayer sp("Lisa");
	assert(sp.name() == "Lisa" && !sp.isInteractive());
	Scaffold s(3, 2);
	s.makeMove(1, RED);
        s.makeMove(1, BLACK);
        s.undoMove();
        assert(s.checkerAt(1, 2) == VACANT);
        assert(s.numberEmpty() == 5);
	cout << "=========" << endl;
	int n = hp.chooseMove(s, 3, RED);
	cout << "=========" << endl;
	assert(n == 2 || n == 3);
	n = bp.chooseMove(s, 3, RED);
	assert(n == 2 || n == 3);
	n = sp.chooseMove(s, 3, RED);
	assert(n == 2 || n == 3);
}

int main()
{
        doPlayerTests();
        cout << "Passed all tests" << endl;
}
