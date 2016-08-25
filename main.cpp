/*
 * There must be a way that movement is seamless/instant, but:
 * 		- The draw must be smooth
 * 		- Movement must be ticked slowly
 * 	Maybe store a memory of the previously drawn field, and compare differences
 *
 */

/*
 * TODO:
 * 		- Pause button (esc)
 * 			- Restart
 * 			- Exit
 * 		- Actual tetrimino pieces
 * 			- Collision
 * 			- Tracking falling and movement
 * 			- Randomizing
 * 			- Drawing
 * 			- Rotating
 * 		- Scoring
 * 		- Game Over and Restarting
 * 		- Smooth Drawing on screen (currently flickering)
 * 		- Quick drop
 */

/*
 * Features a single, non-static field for playing (as opposed to say a static
 * drawing of a field with another layer with the actual blocks).
 *
 */
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

enum tetrimino_t{
	Long,
	Square,
	Tee,
	LShape,
	ReverseLShape,
	Squiggle,
	ReverseSquiggle,
	tetriminoAmount
};

char objectSymbol = '@';
int objectX, objectY, drawCallCounter, symbolsInRow;
bool gameOver;
tetrimino_t nextPiece;
const int FIELDSIZEX = 12;
const int FIELDSIZEY = 22;
const int GAMESPEED = 30;
const int FALLSPEED = 300;

char field[FIELDSIZEY][FIELDSIZEX] = {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
					  	  	  	  	  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', '@', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', '@', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|',
									  '|', '@', '@', '@', '@', '@', '@', '@', '@', ' ', '@', '|',
									  '|', '@', '@', '@', '@', ' ', '@', '@', '@', '@', '@', '|',
									  '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'};

/*	Note:
 *
 * 	ARRAY[YCOORD][XCOORD]
 *
 */

void ShiftDown(int topCompleteRowY);
void Draw();
bool ValidSpace(int Xcoord, int Ycoord);
bool EmptySpaceBelow(int Xcoord, int Ycoord);
void Input();
void Logic();
void Falling();
void DrawGameOver();
void NextTetrimino(tetrimino_t &nextPiece);
void TetriminoPiece(tetrimino_t	nextPiece);

void TetriminoPiece(tetrimino_t nextPiece)
{
	if(nextPiece == 0)
	{
		//Long
		field[objectY][objectX] = '@';
		field[objectY + 1][objectX] = '@';
		field[objectY + 2][objectX] = '@';
		field[objectY + 3][objectX] = '@';
	}
	else if(nextPiece == 1)
	{
		//Square
		field[objectY][objectX] = '@';
		field[objectY + 1][objectX] = '@';
		field[objectY][objectX + 1] = '@';
		field[objectY + 1][objectX + 1] = '@';
	}
	else if(nextPiece == 2)
	{
		//Tee
		field[objectY][objectX] = '@';
		field[objectY + 1][objectX - 1] = '@';
		field[objectY + 1][objectX] = '@';
		field[objectY + 1][objectX + 1] = '@';
	}
	else if(nextPiece == 3)
	{
		//L shape
		field[objectY][objectX] = '@';
		field[objectY + 1][objectX] = '@';
		field[objectY + 2][objectX] = '@';
		field[objectY + 2][objectX + 1] = '@';
	}
	else if(nextPiece == 4)
	{
		//Reverse L shape
		field[objectY][objectX] = '@';
		field[objectY + 1][objectX] = '@';
		field[objectY + 2][objectX] = '@';
		field[objectY + 2][objectX - 1] = '@';
	}
	else if(nextPiece == 5)
	{
		//Squiggle
		field[objectY][objectX] = '@';
		field[objectY + 1][objectX] = '@';
		field[objectY + 1][objectX + 1] = '@';
		field[objectY + 2][objectX + 1] = '@';
	}
	else if(nextPiece == 6)
	{
		//Reverse Squiggle
		field[objectY][objectX] = '@';
		field[objectY + 1][objectX] = '@';
		field[objectY + 1][objectX - 1] = '@';
		field[objectY + 2][objectX - 1] = '@';
	}
	objectX = 4;
	objectY = 1;

}

void NextTetrimino(tetrimino_t &nextPiece)
{
	nextPiece = static_cast<tetrimino_t>(rand() % tetriminoAmount);
}

void ShiftDown(int topCompleteRowY)
{
	//Shifts everything in the playspace down
	//TODO: currently game only handles a 1x1 block falling. Once actual tetrimino implemented, be ready to handle multiple rows completed

	int i, j;
	for(i = topCompleteRowY; i > 1; i--)
	{
		for(j = 1; j < (FIELDSIZEX - 1); j++)
		{
			field[i][j] = field[i-1][j];
		}
	}
}

void Draw()
{
	/*
	 * Constantly draws the field and pieces
	 * Each draw, checks each row to see if it is full or not
	 * If full, calls upon ShiftDown() to shift everything down by 1
	 */

	int i, j;
	system("cls");
	for(i = 0; i < FIELDSIZEY; i++)
	{
		symbolsInRow = 0;
		for(j = 0; j < FIELDSIZEX; j++)
		{
			if((i == objectY) && (j == objectX))
			{
				//WILL BE CHANGED, PROBABLY WITH A METHOD TO DRAW THE NEW TETRIMINO
				TetriminoPiece(nextPiece);
				//cout << objectSymbol;
			}
			/*
			 *	Don't know what the second conditional is... Probably not important
			else if((field[i][j] == objectSymbol) && ((i != (FIELDSIZEX - 3)) || (i != 0)))
			{
				symbolsInRow++;
				cout << field[i][j];
			}*/
			else if(field[i][j] == objectSymbol)
			{
				symbolsInRow++;
				cout << field[i][j];
			}
			else
			{
				cout << field[i][j];
			}
		}
		cout << "\t\t" << symbolsInRow;				//FOR DEBUGGING
		if(symbolsInRow == (FIELDSIZEX-2))
		{
			//FULL ROW
			cout << "\t\t\tFULL ROW";				//FOR DEBUGGING
			for(j = 1; j < FIELDSIZEX-1; j++)		//REPLACES ENTIRE FULL ROW WITH BLANK
			{
				field[i][j] = ' ';
			}
			ShiftDown(i);
		}
		cout << endl;
	}
	cout << "Draw Call Counter: " << drawCallCounter << endl;
	Sleep(GAMESPEED);
}

bool ValidSpace(int Xcoord, int Ycoord)
{
	char charInField = field[Ycoord][Xcoord];
	if((Xcoord < 0 ) || (Ycoord < 0))
	{
		return false;
	}
	else if((Xcoord > FIELDSIZEX-1) || (Ycoord > FIELDSIZEY-1))
	{
		return false;
	}
	else if((charInField == '-') || (charInField == '|') || (charInField == '@'))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool EmptySpaceBelow(int Xcoord, int Ycoord)
{
	//Gets the char below and looks for empty space, true if there IS space
	char fieldCharBelow;
	if(nextPiece == 0)
	{
		//Long
		fieldCharBelow = field[Ycoord + 4][Xcoord];
	}
	else if(nextPiece == 1)
	{
		//Square
		fieldCharBelow = field[Ycoord + 2][Xcoord];
	}
	else if(nextPiece == 2)
	{
		//Tee
		fieldCharBelow = field[Ycoord + 3][Xcoord];
	}
	else if(nextPiece == 3)
	{
		//L Shape
		fieldCharBelow = field[Ycoord + 3][Xcoord];
	}
	else if(nextPiece == 4)
	{
		//Reverse L Shape
		fieldCharBelow = field[Ycoord + 3][Xcoord];
	}
	else if(nextPiece == 5)
	{
		//Squiggle
		fieldCharBelow = field[Ycoord + 3][Xcoord];
	}
	else if(nextPiece == 6)
	{
		//Reverse Squiggle
		fieldCharBelow = field[Ycoord + 3][Xcoord];
	}

	if((fieldCharBelow == '-') || (fieldCharBelow == '@'))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Input()
{
	if(GetAsyncKeyState(VK_LEFT))
	{
		if(ValidSpace(objectX-1, objectY))
		{
			objectX--;
		}
	}
	else if(GetAsyncKeyState(VK_RIGHT))
	{
		if(ValidSpace(objectX+1, objectY))
		{
			objectX++;
		}
	}
	else if(GetAsyncKeyState(VK_DOWN))
	{
		if(ValidSpace(objectX, objectY+1))
		{
			objectY++;
		}
	}
	//Put on pause, getasynckeystate is too sensitive, need to register
	//one press at most.
	/*else if(GetAsyncKeyState(VK_SPACE)) {
		//Scan each field below current object and stop above invalid space
		//Only works for dropping on @ for some reason
		for(k = objectY; k < (fieldSize-2); k++) {
			if(!EmptySpaceBelow(objectX, k)) {
				objectY = k;
				Logic();
			} else if(!ValidSpace(objectX, k)) {
				cout << "INVALID SPACE" << endl;
			}
		}
	}*/
	//Used for debugging as you normally can't go up
	else if(GetAsyncKeyState(VK_UP))
	{
		if(ValidSpace(objectX, objectY-1))
		{
			objectY--;
		}
	}
}

void DrawGameOver()
{
	char yesOrNo = 'X';
	field[7][4] = 'G'; field[7][5] = 'A'; field[7][6] = 'M'; field[7][7] = 'E';
	field[8][4] = 'O'; field[8][5] = 'V'; field[8][6] = 'E'; field[8][7] = 'R';
	Draw();
	cout << "PLAY AGAIN?" << endl;
	cout << "<Y/N>" << endl;
	while((yesOrNo != 'Y') || (yesOrNo != 'N'))
	{
		cin >> yesOrNo;
	}
	if(yesOrNo == 'Y')
	{
		cout << "AGAIN!" << endl;
		cin >> yesOrNo;
	}
	else
	{
		exit(0);
	}
}

void Logic()
{
	if(!EmptySpaceBelow(objectX, objectY))
	{
		if(objectY == 0)
		{
			gameOver = true;
			DrawGameOver();
		}
		else
		{
			//DETECTS TETRIMINO HITTING THE FLOOR
			/*
		 	 * TODO:
		 	 * 	- Implement methods for each tetrimino, checking if it hit the floor
		 	 */
			//Insert object in field
			//NextTetrimino(nextPiece);
			//TetriminoSpawn(nextPiece);
			field[objectY][objectX] = '@';
			//'Make' a new object by going back to original spawn
			objectX = 4;
			objectY = 1;
		}
	}
}

void Falling()
{
	if(ValidSpace(objectX, objectY+1))
	{
		objectY++;
	}
}

int main()
{
	drawCallCounter = 0;
	objectX = 4;
	objectY = 1;
	srand(time(0));
	NextTetrimino(nextPiece);
	while(!gameOver)
	{
		Draw();
		drawCallCounter++;
		Input();
		Logic();
		if(drawCallCounter == (FALLSPEED/GAMESPEED))
		{
			Falling();
			drawCallCounter = 0;
		}
	}
	return 0;
}
