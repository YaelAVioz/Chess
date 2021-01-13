/***********
* Yael Avioz
* 207237421
* 01
* ass04
***********/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "ass4.h"
#define SIZE 8

const char FIRST_COL = 'a';

// FEN & Board characters
const char WHITE_PAWN = 'P';
const char WHITE_ROOK = 'R';
const char WHITE_KNIGHT = 'N';
const char WHITE_BISHOP = 'B';
const char WHITE_QUEEN = 'Q';
const char WHITE_KING = 'K';
const char BLACK_PAWN = 'p';
const char BLACK_ROOK = 'r';
const char BLACK_KNIGHT = 'n';
const char BLACK_BISHOP = 'b';
const char BLACK_QUEEN = 'q';
const char BLACK_KING = 'k';

// FEN separator for strtok()
const char SEP[] = "/";

// Board characters
const char EMPTY = ' ';

// function declerations
int legalPathPawn(char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn, char board[][SIZE]);
int clearPathPawn(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol);
int legalPathQueen(char srcRow, char srcCol, char destRow, char destCol);
int clearPathQueen(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn);
int legalPathRook(char srcRow, char srcCol, char destRow, char destCol);
int clearPathRook(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn);
int legalPathBishop(char srcRow, char srcCol, char destRow, char destCol);
int clearPathBishop(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn);
int legalPathKnight(char srcRow, char srcCol, char destRow, char destCol);
int clearPathKnight(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn);

/**************************
 * convert char to digitfin
 **************************/
int toDigit(char c) {
	assert('0' <= c && c <= '9');
	return c - '0';
}

void printColumns() {
	char column = toupper(FIRST_COL);
	printf("* |");
	for (int i = 0; i < SIZE; i++) {
		if (i) {
			printf(" ");
		}
		printf("%c", column);
		column++;
	}
	printf("| *\n");
}

void printSpacers() {
	printf("* -");
	for (int i = 0; i < SIZE; i++) {
		printf("--");
	}
	printf(" *\n");
}

void printRow(char row[], int rowIdx) {
	printf("%d ", rowIdx);
	int i = 0;
	while (row[i]) {
		if (isdigit(row[i])) {
			int spaces = toDigit(row[i++]);
			for (int j = 0; j < spaces; j++) {
				printf("|%c", EMPTY);
			}
		}
		else {
			printf("|%c", row[i++]);
		}
	}
	printf("| %d\n", rowIdx);
}

void printBoardFromFEN(char fen[]) {
	printColumns();
	printSpacers();
	int rowIdx = SIZE;
	char* fenRow = strtok(fen, SEP);
	while (fenRow != NULL) {
		printRow(fenRow, rowIdx--);
		fenRow = strtok(NULL, SEP);
	}
	printSpacers();
	printColumns();
}
/******************
*Function Name : createBoard
*Input : char board[][SIZE],char fen[]
	*Output : there is no output
	*Function Operation : the function create the game board
******************/
void createBoard(char board[][SIZE], char fen[])
{
	int row = 0, col = 0;
	int i; // i is the index of the fen string
	char* pfen = strtok(fen, SEP);
	//this loop adds the values of the string to the array
	for (row; row < SIZE; row++)
	{
		i = 0; //reset i to zero
		while (pfen[i] != '\0') //this loop check when the sring ends
		{
			if (pfen[i] >= '0' && pfen[i] <= '9') //check if there is space in the game borad
			{
				while (pfen[i] > '0') //this loop check how many spaces should be in the board
				{
					board[row][col] = EMPTY;
					col++;
					pfen[i]--;
				}
			}
			else
			{
				board[row][col] = pfen[i];
				col++;
			}
			i++;
		}
		pfen = strtok(NULL, SEP);
		col = 0;
	}
}

/******************
*Function Name : printBoard
*Input : char board[][SIZE]
	*Output : print the game board
	*Function Operation : the function convert the board to fen strin and sand it to the func PrintFromFEN
******************/
void printBoard(char board[][SIZE])
{
	//this part get the borad and converts it to a new fen string
	int row = 0, col = 0;
	int i = 0; //index of newFen
	int counter = 0;
	int rowIdx = SIZE;
	char newFen[80]; //the size is bigger then the max char could be in the array (SIZE*SIZE)+7'/'
	//this loop convert the board to fen string
	for (row; row < SIZE; row++)
	{
		for (col = 0; col < SIZE; col++)
		{
			if (board[row][col] == EMPTY) //check if there are spaces in the board
			{
				counter++;
			}
			else
			{
				if (counter != 0) //if there was spaces its convert them to a number
				{
					newFen[i] = '0' + counter; //the number of spaces in the board
					counter = 0; //reset the counter
					i++;
				}
				newFen[i] = board[row][col];
				i++;
			}
		}
		if (counter != 0) //if there was spaces its convert them to a number
		{
			newFen[i] = '0' + counter; //the number of spaces in the board
			counter = 0; //reset the counter
			i++;
		}
		newFen[i] = '/';
		i++;
	}
	newFen[i] = '\0';
	printBoardFromFEN(newFen); //the func printBoardToFen will print thr new FEN
}

//location representation in the board
typedef struct {
	int rowInBoard, colInBoard;
} Location;

//location representation in the Game
typedef struct {
	char rowInBoard, colInBoard;
} GameLocation;

typedef struct {
	char srcRow, srcCol, destRow, destCol;
	int isChess, isCapture;
	int useCol, useRow, isBad;
} pgnState;

/******************
* Function Name: locationInBoard
* Input: char row, char col
* Output: int row, int col
* Function Operation: the function swich the variabls of char location to int location
******************/
Location locationInBoard(char row, char col)
{
	int colInBoard, rowInBoard;
	colInBoard = (col - 'a');
	//this part swich the column 
	rowInBoard = '8' - row;
	Location location = { rowInBoard, colInBoard };
	return location;
}

/******************
* Function Name: locationInGame
* Input: char row, char col
* Output: int row, int col
* Function Operation: the function swich the variabls of int location to char location
******************/
GameLocation locationInGame(int row, int col)
{
	char colInBoard, rowInBoard;
	colInBoard = ('a' + col);
	//this part swich the column 
	rowInBoard = ('8' - row);
	GameLocation location = { rowInBoard, colInBoard };
	return location;
}

/******************
* Function Name: findMyPiece
* Input: char board[][SIZE], char piece, int srcRow, int srcCol
* Output: Location location
* Function Operation: the function recieves the board, a piece to find and its specific row\col
					  and returns the piece location - if it exists. otherwise it returns the {-1, -1}
******************/
GameLocation findMyPiece(char board[][SIZE], char piece, char srcRow, char srcCol, int useRow, int useCol)
{
	Location location = { -1,-1 };
	GameLocation gameLocation;
	Location srcLocation = locationInBoard(srcRow, srcCol);
	int foundFlag = 0; // this flag checks if have already found a piece
	//this loop find the location of the white king on the board and save it in the struct
	for (int row = 0; row < SIZE; row++)
	{
		for (int col = 0; col < SIZE; col++)
		{
			if (board[row][col] == piece)
			{
				if ((useRow != 1 || row == srcLocation.rowInBoard) && (useCol != 1 || col == srcLocation.colInBoard))
				{
					if (foundFlag == 1)
					{
						location.colInBoard = -1;
						location.rowInBoard = -1;
					}
					else
					{
						location.rowInBoard = row;
						location.colInBoard = col;
						foundFlag = 1;
					}
				}
			}
		}
	}
	gameLocation = locationInGame(location.rowInBoard, location.colInBoard);
	return gameLocation;
}

/******************
* Function Name: findMyBNRQ
* Input: char board[][SIZE], char piece, int srcRow, int srcCol
* Output: Location location
* Function Operation: the function recieves the board, a piece to find and its specific row\col
					  and returns the piece location - if it exists. otherwise it returns the {-1, -1}
******************/
GameLocation findMyBNRQ(char board[][SIZE], char piece, char srcRow, char srcCol, char destRow, char destCol, int useRow, int useCol, int isWhiteTurn)
{
	Location location = { -1,-1 };
	GameLocation gameLocation;
	Location srcLocation = locationInBoard(srcRow, srcCol);
	GameLocation curLocation;
	int foundFlag = 0; // this flag checks if have already found a piece
	//this loop find the location of the white king on the board and save it in the struct
	for (int row = 0; row < SIZE; row++)
	{
		for (int col = 0; col < SIZE; col++)
		{
			curLocation = locationInGame(row, col);
			if ((useRow != 1 || row == srcLocation.rowInBoard) && (useCol != 1 || col == srcLocation.colInBoard))
			{
				if (board[row][col] == piece)
				{
					if (piece == 'R' || piece == 'r')
					{
						if (legalPathRook(curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol) == 1)
						{
							if (clearPathRook(board, curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol, isWhiteTurn) == 1)
							{
								location.rowInBoard = row;
								location.colInBoard = col;
							}
						}
					}
					if (piece == 'B' || piece == 'b')
					{
						if (legalPathBishop(curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol) == 1)
						{
							if (clearPathBishop(board, curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol, isWhiteTurn) == 1)
							{
								location.rowInBoard = row;
								location.colInBoard = col;
							}
						}
					}
					if (piece == 'N' || piece == 'n')
					{
						if (legalPathKnight(curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol) == 1)
						{
							if (clearPathKnight(board, curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol, isWhiteTurn) == 1)
							{
								location.rowInBoard = row;
								location.colInBoard = col;
							}
						}
					}
					if (piece == 'Q' || piece == 'q')
					{
						if (legalPathQueen(curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol) == 1)
						{
							if (clearPathQueen(board, curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol, isWhiteTurn) == 1)
							{
								location.rowInBoard = row;
								location.colInBoard = col;
							}
						}
					}
				}
			}
		}
	}
	gameLocation = locationInGame(location.rowInBoard, location.colInBoard);
	return gameLocation;
}

GameLocation findMyPawn(char board[][SIZE], char destRow, char destCol, char srcCol, int useCol, int isWhiteTurn)
{
	int foundFlag = 0;
	char loc, piece;
	int srcColBoard = srcCol - 'a';
	Location location = { -1,-1 };
	GameLocation gameLocation, curLocation;
	if (isWhiteTurn == 1)
	{
		piece = 'P';
	}
	else
	{
		piece = 'p';
	}

	// find the pawns
	for (int row = 0; row < SIZE; row++)
	{
		for (int col = 0; col < SIZE; col++)
		{
			curLocation = locationInGame(row, col);
			loc = board[row][col];
			if (loc == piece)
			{
				if (useCol != 1 || col == srcColBoard)
				{
					if (legalPathPawn(curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol, isWhiteTurn, board))
					{
						if (clearPathPawn(board, curLocation.rowInBoard, curLocation.colInBoard, destRow, destCol))
						{
							{
								if (foundFlag == 1)
								{
									location.rowInBoard = -1;
									location.colInBoard = -1;
								}
								if (foundFlag != 1)
								{
									location.rowInBoard = row;
									location.colInBoard = col;
									foundFlag = 1;
								}
							}
						}
					}
				}
			}
		}
	}
	gameLocation = locationInGame(location.rowInBoard, location.colInBoard);
	return gameLocation;
}

pgnState parsePGN(char pgn[])
{
	int strSize = 0; //check the length of the PGN string
	while (pgn[strSize] != '\0')
	{
		strSize++;
	}
	pgnState outputPGN = { .useCol = 0, .useRow = 0, .isChess = 0, .isBad = 0, .isCapture = 0 };
	pgnState badPGN = { .isBad = 1 };

	int i = 1, isCapture = 0;
	int captureIdx = 0;

	// finding +
	int final_idx = strSize - 1;
	if ((pgn[final_idx] == '+' || pgn[final_idx] == '#')) // check chess
	{
		outputPGN.isChess = 1; // in case the king allows another piece to chess the other king
		strSize -= 1;
	}
	for (i = 0; i < strSize; i++)
	{
		if (pgn[i] == 'x')
		{
			outputPGN.isCapture = 1;
			captureIdx = i;
		}
	}
	i = 1;
	if (outputPGN.isCapture == 1 || strSize >= 4)
	{
		if (pgn[i] >= 'a' && pgn[i] <= 'h') //check coulumn source
		{
			outputPGN.srcCol = pgn[i];
			i++;
			outputPGN.useCol = 1;
		}
		if (pgn[i] <= '0' + 8 && pgn[i] >= '0' + 1) //check the row source of the piece
		{
			outputPGN.srcRow = pgn[i];
			i++;
			outputPGN.useRow = 1;
		}
	}
	if (outputPGN.isCapture == 1)
	{
		i = captureIdx + 1;
	}
	if (pgn[i] >= 'a' && pgn[i] <= 'h') //check coulumn destination
	{
		outputPGN.destCol = pgn[i];
		i++;
	}
	else //in case it's not legal 
	{
		return badPGN;
	}
	if (pgn[i] <= '0' + 8 && pgn[i] >= '0' + 1) //check the row destination of the piece
	{
		outputPGN.destRow = pgn[i];
		i++;
	}
	else //in case it's not legal 
	{
		return badPGN;
	}
	return outputPGN;
}

/******************
* Function Name: isValidLocation
* Input: Location location
* Output: 0 or 1
* Function Operation: the function gets a location and returns 0 if it's invalid ({-1, -1})
					  and 1 if it is a legal location.
******************/
int isValidLocation(GameLocation gameLocation)
{
	Location location = locationInBoard(gameLocation.rowInBoard, gameLocation.colInBoard);
	if (location.colInBoard == -1 && location.rowInBoard == -1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/******************
* Function Name: checkCapture
* Input: char board[][SIZE], char destRow, char destCol, int isWhiteTurn
* Output: true or flase
* Function Operation: the function check if the piece can capture -
  check if the destination location is not empty or that there a piece of the other player
******************/
int checkCapture(char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
{
	Location locationCapture;
	locationCapture = locationInBoard(destRow, destCol); //switch the location char variabls to int variabls
	//loc - save the value that in the requested location 
	char loc = board[locationCapture.rowInBoard][locationCapture.colInBoard];
	if (isWhiteTurn == 1) //check if it's white turn
	{
		//if there is a piece in the location destination we want to make sure its the other player piece
		if (loc != EMPTY) //check its not empty place in the board 
		{
			if (loc >= 'a' && loc <= 'z') //if thats true so there is capture (we check between a-z because that means that the piece could be only one of the black pieces)
			{
				return 1; //return capture is true
			}
		}
		return 0; //if it wasnt one of the black piece so the capture is not available
	}
	else //if it's the black turn
	{
		//if there is a piece in the location destination we want to make sure its the other player piece
		if (loc != EMPTY)
		{
			if (loc >= 'A' && loc <= 'Z') //if thats true so there is capture
			{
				return 1;//return capture is true
			}
		}
		return 0; //if it wasnt one of the white piece so the capture is not available
	}
	return 0; //if the board in this location is empty there is no capture
}

/******************
* Function Name: isPromotion
* Input: char board[][SIZE], int srcRow, int srcCol, int isWhiteTurn, char pgn[]
* Output: /
* Function Operation: the function check if the pawn is promotion - if yes, the function will change the board to the the requested piece
******************/
void isPromotion(char board[][SIZE], int srcRow, int srcCol, int isWhiteTurn, char newPiece)
{
	if (isWhiteTurn == 1) //white turn
	{
		if (srcRow == 0) //the pawn is in row 0
		{
			board[srcRow][srcCol] = newPiece; //change the pawn to the requested piece
		}
		else
		{
			return;
		}
	}
	else //black turn
	{
		if (srcRow == 7) //the pawn is in row 7
		{
			board[srcRow][srcCol] = newPiece; //change the pawn to the requested piece
		}
		else
		{
			return;
		}
	}
}

/******************
* Function Name: legalPathKing
* Input: char srcRow, char srcCol, char destRow, char destCol
* Output: true or flase
* Function Operation: the function checks if the piece (King) is moving according to the game rules
******************/
int legalPathKing(char srcRow, char srcCol, char destRow, char destCol)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the destination cahr locatoin variabls to int variabls 
	int stepSum = abs(src.rowInBoard - dest.rowInBoard) + abs(src.colInBoard - dest.colInBoard);
	if (stepSum == 1 || stepSum == 2) //the king can move only one step at a time so we check if there was a chang of 1 in the row or col or in both of them.
	{
		return 1; //return true if he moves acording to the ruels 
	}
	return 0; //else retruen false (the move is not legal)
}

/******************
* Function Name: clearPathKing
* Input: (char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
* Output: true or flase
* Function Operation: the function check if the path to the destination is available for the king piece
******************/
int clearPathKing(char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
{
	Location dest = locationInBoard(destRow, destCol); //switch the char location variabls to int variabls 
	char loc = board[dest.rowInBoard][dest.colInBoard]; //loc - save the value of the destination point
	if (loc == EMPTY) //if the destination point is empty its legal
	{
		return 1; //return true
	}
	else if (isWhiteTurn == 1) //check if it's white trun 
	{
		if (loc >= 'A' && loc <= 'Z')
		{
			return 0;
		}
		if (loc >= 'a' || loc >= 'z') //if there is a black piece in the destination point its a legal move
		{
			return 1;
		}
		else //if its not a black piece (should be white) its not a legal move
		{
			return 0;
		}
	}
	else if (isWhiteTurn != 1) //check if its the black turn
	{
		if (loc >= 'a' && loc <= 'z')
		{
			return 0;
		}
		if (loc >= 'A' || loc >= 'Z') //if there is a white piece in the destination point its a legal move
		{
			return 1;
		}
		else //if its not a white piece (should be black) its not a legal move
		{
			return 0;
		}
	}
}

/******************
* Function Name: legalPathRook
* Input: char srcRow, char srcCol, char destRow, char destCol
* Output: true or flase
* Function Operation: the function check if the piece (Rook) moveing acording to the game rules
******************/
int legalPathRook(char srcRow, char srcCol, char destRow, char destCol)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the destination cahr locatoin variabls to int variabls
	 //check if the piece moves in a column
	if (((src.rowInBoard - dest.rowInBoard) == 0) && ((src.colInBoard - dest.colInBoard) != 0))
	{
		return 1; //return true if he moves acording to the ruels 
	}
	//check if the piece moves in a row
	else if (((src.rowInBoard - dest.rowInBoard) != 0) && ((src.colInBoard - dest.colInBoard) == 0))
	{
		return 1; //return true if he moves acording to the ruels 
	}
	return 0; //else retruen false (the move is not legal)
}

/******************
* Function Name: clearPathRook
* Input: (char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
* Output: true or flase
* Function Operation: the function check if the path to the destination is available for the Rook piece
******************/
int clearPathRook(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the char location variabls to int variabls 
	char loc = board[dest.rowInBoard][dest.colInBoard]; //loc - save the value of the destination point
	if (isWhiteTurn == 1) //it's white turn
	{
		if (loc >= 'A' && loc <= 'Z')
		{
			return 0;
		}
		if (loc == EMPTY || (loc >= 'a' || loc <= 'z')) //if the destination point is empty or have black piece it's legal
		{
			//check if the piece moves in a column
			if (((src.rowInBoard - dest.rowInBoard) == 0) && ((src.colInBoard - dest.colInBoard) != 0))
			{
				//if the piece goes left in the board
				if ((src.colInBoard - dest.colInBoard) >= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int col = src.colInBoard - 1; col > dest.colInBoard; col--)
					{
						if (board[src.rowInBoard][col] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
				//if the piece goes right in the board
				if ((src.colInBoard - dest.colInBoard) <= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int col = src.colInBoard + 1; col < dest.colInBoard; col++)
					{
						if (board[src.rowInBoard][col] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
			}
			//check if the piece moves in a row
			else if (((src.rowInBoard - dest.rowInBoard) != 0) && ((src.colInBoard - dest.colInBoard) == 0))
			{
				//if the piece moving forward in the board
				if ((src.rowInBoard - dest.rowInBoard) >= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int row = src.rowInBoard - 1; row > dest.rowInBoard; row--)
					{
						if (board[row][src.colInBoard] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
				//if the piece goes back on the board
				if ((src.rowInBoard - dest.rowInBoard) <= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int row = src.rowInBoard + 1; row < dest.rowInBoard; row++)
					{
						if (board[row][src.colInBoard] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
			}
		}
	}
	if (isWhiteTurn != 1) //it's black turn
	{
		if (loc >= 'a' && loc <= 'z')
		{
			return 0;
		}
		if (loc == EMPTY || (loc >= 'A' || loc <= 'Z')) //if the destination point is empty or have black piece it's legal
		{
			//check if the piece moves in a column
			if (((src.rowInBoard - dest.rowInBoard) == 0) && ((src.colInBoard - dest.colInBoard) != 0))
			{
				//if the piece goes left in the board
				if ((src.colInBoard - dest.colInBoard) >= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int col = src.colInBoard - 1; col > dest.colInBoard; col--)
					{
						if (board[src.rowInBoard][col] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
				//if the piece goes right in the board
				if ((src.colInBoard - dest.colInBoard) <= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int col = src.colInBoard + 1; col < dest.colInBoard; col++)
					{
						if (board[src.rowInBoard][col] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
			}
			//check if the piece moves in a row
			else if (((src.rowInBoard - dest.rowInBoard) != 0) && ((src.colInBoard - dest.colInBoard) == 0))
			{
				//if the piece moving forward in the board
				if ((src.rowInBoard - dest.rowInBoard) >= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int row = src.rowInBoard - 1; row > dest.rowInBoard; row--)
					{
						if (board[row][src.colInBoard] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
				//if the piece goes back on the board
				if ((src.rowInBoard - dest.rowInBoard) <= 0)
				{
					//this loop check that the piece path at the column is empty and he can move
					for (int row = src.rowInBoard + 1; row < dest.rowInBoard; row++)
					{
						if (board[row][src.colInBoard] != EMPTY)
						{
							return 0; // if there is a piece in the path its not a legal move
						}
					}
				}
			}
		}
	}
	return 1;
}

/******************
* Function Name: legalPathBishop
* Input: char srcRow, char srcCol, char destRow, char destCol
* Output: true or flase
* Function Operation: the function check if the piece (Bishop) moveing acording to the game rules
******************/
int legalPathBishop(char srcRow, char srcCol, char destRow, char destCol)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the destination cahr locatoin variabls to int variabls
	 //check if the piece moves in a column
	if (abs(src.rowInBoard - dest.rowInBoard) == abs(src.colInBoard - dest.colInBoard))
	{
		return 1; //return true if he moves acording to the ruels 
	}
	else
	{
		return 0; //else retruen false (the move is not legal)
	}
}

/******************
* Function Name: findRookDirection
* Input: (Location src, Location dest)
* Output: string
* Function Operation: find the Bishops direction
******************/
char* findRookDirection(Location src, Location dest)
{
	int rowDiff = dest.rowInBoard - src.rowInBoard;
	int colDiff = dest.colInBoard - src.colInBoard;

	if (rowDiff > 0 && colDiff > 0)
	{
		return "down_and_right";
	}
	else if (rowDiff < 0 && colDiff > 0)
	{
		return "up_and_right";
	}
	else if (rowDiff > 0 && colDiff < 0)
	{
		return "down_and_left";
	}
	else if (rowDiff < 0 && colDiff < 0)
	{
		return "up_and_left";
	}
}

/******************
* Function Name: clearPathBishop
* Input: (char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
* Output: true or flase
* Function Operation: the function check if the path to the destination is available for the Bishop piece
******************/
int clearPathBishop(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the char location variabls to int variabls 
	char loc = board[dest.rowInBoard][dest.colInBoard]; //loc - save the value of the destination point
	char* direction = findRookDirection(src, dest);
	if (isWhiteTurn == 1) //it's white turn
	{
		if (loc >= 'A' && loc <= 'Z')
		{
			return 0;
		}
		if (loc != EMPTY && (loc < 'a' || loc > 'z')) //if the destination point is empty or have black piece it's legal
		{
			return 0;
		}
	}
	if (isWhiteTurn == 0) //it's black turn
	{
		if (loc >= 'a' && loc <= 'z')
		{
			return 0;
		}
		if (loc != EMPTY && (loc < 'A' || loc > 'Z')) //if the destination point is empty or have black piece it's legal
		{
			return 0;
		}
	}
	if (direction == "up_and_right")
	{
		src.rowInBoard -= 1;
		src.colInBoard += 1;
		while (src.rowInBoard != dest.rowInBoard && src.colInBoard != dest.colInBoard)
		{
			if (board[src.rowInBoard][src.colInBoard] != EMPTY)
			{
				return 0;
			}
			src.rowInBoard -= 1;
			src.colInBoard += 1;
		}
	}
	if (direction == "up_and_left")
	{
		src.rowInBoard -= 1;
		src.colInBoard -= 1;
		while (src.rowInBoard != dest.rowInBoard && src.colInBoard != dest.colInBoard)
		{
			if (board[src.rowInBoard][src.colInBoard] != EMPTY)
			{
				return 0;
			}
			src.rowInBoard -= 1;
			src.colInBoard -= 1;
		}
	}
	if (direction == "down_and_right")
	{
		src.rowInBoard += 1;
		src.colInBoard += 1;
		while (src.rowInBoard != dest.rowInBoard && src.colInBoard != dest.colInBoard)
		{
			if (board[src.rowInBoard][src.colInBoard] != EMPTY)
			{
				return 0;
			}
			src.rowInBoard += 1;
			src.colInBoard += 1;
		}
	}if (direction == "down_and_left")
	{
		src.rowInBoard += 1;
		src.colInBoard -= 1;
		while (src.rowInBoard != dest.rowInBoard && src.colInBoard != dest.colInBoard)
		{
			if (board[src.rowInBoard][src.colInBoard] != EMPTY)
			{
				return 0;
			}
			src.rowInBoard += 1;
			src.colInBoard -= 1;
		}
	}
	return 1;
}

/******************
* Function Name: legalPathQueen
* Input: char srcRow, char srcCol, char destRow, char destCol
* Output: true or flase
* Function Operation: the function check if the piece (Queen) moveing acording to the game rules
******************/
int legalPathQueen(char srcRow, char srcCol, char destRow, char destCol)
{
	//queen moves like a Bishop or a Rook 
	if (legalPathRook(srcRow, srcCol, destRow, destCol) == 1)
	{
		if (legalPathBishop(srcRow, srcCol, destRow, destCol) == 0)
		{
			return 1; //it's a legal move
		}
	}
	else if (legalPathBishop(srcRow, srcCol, destRow, destCol) == 1)
	{
		if (legalPathRook(srcRow, srcCol, destRow, destCol) == 0)
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

/******************
* Function Name: clearPathQueen
* Input: (char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
* Output: true or flase
* Function Operation: the function check if the path to the destination is available for the Queen piece
******************/
int clearPathQueen(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn)
{
	//queen moves like a Bishop or a Rook 
	if (legalPathRook(srcRow, srcCol, destRow, destCol) == 1) //moves like a Rook in a row or a col
	{
		if (clearPathRook(board, srcRow, srcCol, destRow, destCol, isWhiteTurn) == 1) //check empty path
		{
			return 1; //if the path is empty return true
		}
		else
		{
			return 0; //if the path is not empty return false
		}
	}
	else if (legalPathBishop(srcRow, srcCol, destRow, destCol) == 1) //move like a Bishop in a cross
	{
		if (clearPathBishop(board, srcRow, srcCol, destRow, destCol, isWhiteTurn) == 1) //check empty path
		{
			return 1; //if the path is empty return true
		}
		else
		{
			return 0; //if the path is not empty return false
		}
	}
}

/******************
* Function Name: legalPathKnight
* Input: char srcRow, char srcCol, char destRow, char destCol
* Output: true or flase
* Function Operation: the function check if the piece (Knight) moveing acording to the game rules
******************/
int legalPathKnight(char srcRow, char srcCol, char destRow, char destCol)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the destination cahr locatoin variabls to int variabls
	//All these 4 if checks if the move is legal
	if ((abs(src.rowInBoard - dest.rowInBoard) == 2) && (abs(src.colInBoard - dest.colInBoard) == 1))
	{
		return 1;
	}
	else if ((abs(src.rowInBoard - dest.rowInBoard) == 1) && (abs(src.colInBoard - dest.colInBoard) == 2))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************
* Function Name: clearPathKnight
* Input: (char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
* Output: true or flase
* Function Operation: the function check if the path to the destination is available for the Knight piece
******************/
int clearPathKnight(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the destination cahr locatoin variabls to int variabls
	char loc = board[dest.rowInBoard][dest.colInBoard]; //loc - save the value of the destination point
	if (isWhiteTurn == 1) //it's white turn
	{
		if (loc >= 'A' && loc <= 'Z')
		{
			return 0;
		}
		if (loc == EMPTY || (loc >= 'a' || loc <= 'z')) //check if the destination point is empty or have black piece 
		{
			return 1; //it's a legal move
		}
	}
	if (isWhiteTurn != 1) //it's black turn
	{
		if (loc >= 'a' && loc <= 'z')
		{
			return 0;
		}
		if (loc == EMPTY || (loc >= 'A' || loc <= 'Z')) //check if the destination point is empty or have black piece 
		{
			return 1; //it's a legal move
		}
	}
	return 0;
}

/******************
* Function Name: legalPathPawn
* Input: char srcRow, char srcCol, char destRow, char destCol
* Output: true or flase
* Function Operation: the function check if the piece (Pawn) moveing acording to the game rules
******************/
int legalPathPawn(char srcRow, char srcCol, char destRow, char destCol, int isWhiteTurn, char board[][SIZE])
{
	Location src = locationInBoard(srcRow, srcCol); //switch the source cahr locatoin variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the destination cahr locatoin variabls to int variabls
	if (isWhiteTurn == 1) //it's the white turn
	{
		if (src.rowInBoard == 6) //check if its the first move of the pawn in the game
		{
			if ((src.rowInBoard - dest.rowInBoard) <= 2 && (src.colInBoard == dest.colInBoard))
			{
				return 1; //this is a legal move
			}
		}
		if (checkCapture(board, destRow, destCol, isWhiteTurn) == 1)
		{
			if ((src.rowInBoard - 1 == dest.rowInBoard) && (src.colInBoard - 1 == dest.colInBoard)) //front and right
			{
				return 1; //this is a legal move
			}
			else if ((src.rowInBoard - 1 == dest.rowInBoard) && (src.colInBoard + 1 == dest.colInBoard)) //front and left
			{
				return 1; //this is a legal move
			}
			else
			{
				return 0; //this is not a legal move
			}
		}
		else if ((src.rowInBoard - dest.rowInBoard) == 1 && (src.colInBoard == dest.colInBoard))
		{
			return 1; //this is a legal move
		}
		else
		{
			return 0; //this is not a legal move
		}
	}
	if (isWhiteTurn != 1) //its a black turn
	{
		if (src.rowInBoard == 1) //check if its the first move of the pawn in the game
		{
			if ((dest.rowInBoard - src.rowInBoard) <= 2 && (src.colInBoard == dest.colInBoard))
			{
				return 1; //this is a legal move
			}
		}
		if (checkCapture(board, destRow, destCol, isWhiteTurn) == 1)
		{
			if ((dest.rowInBoard - 1 == src.rowInBoard) && (src.colInBoard - 1 == dest.colInBoard)) //front and left
			{
				return 1; //this is a legal move
			}
			else if ((dest.rowInBoard - 1 == src.rowInBoard) && (src.colInBoard + 1 == dest.colInBoard)) //front and right
			{
				return 1; //this is a legal move
			}
			else
			{
				return 0; //this is not a legal move
			}
		}
		else if ((dest.rowInBoard - src.rowInBoard) == 1 && (src.colInBoard == dest.colInBoard))
		{
			return 1; //this is a legal move
		}
		else
		{
			return 0; //this is not a legal move
		}
	}
	return 0;
}

/******************
* Function Name: clearPathPawn
* Input: (char board[][SIZE], char destRow, char destCol, int isWhiteTurn)
* Output: true or flase
* Function Operation: the function check if the path to the destination is available for the pawn piece
******************/
int clearPathPawn(char board[][SIZE], char srcRow, char srcCol, char destRow, char destCol)
{
	Location src = locationInBoard(srcRow, srcCol); //switch the char location variabls to int variabls 
	Location dest = locationInBoard(destRow, destCol); //switch the char location variabls to int variabls 
	char loc = board[dest.rowInBoard][dest.colInBoard]; //loc - save the value of the destination point
	if (abs(dest.rowInBoard - src.rowInBoard) == 1 && (dest.colInBoard == src.colInBoard))
	{
		if (board[dest.rowInBoard][dest.colInBoard] == EMPTY)
		{
			return 1; //clear path
		}
		else
		{
			return 0; //not clear
		}
	}
	else if (abs(dest.rowInBoard - src.rowInBoard) == 2 && (dest.colInBoard == src.colInBoard))
	{
		if (board[dest.rowInBoard][dest.colInBoard] == EMPTY)
		{
			//check the spot between the source and the destination point (the average point)
			if (board[((dest.rowInBoard + src.rowInBoard) / 2)][dest.colInBoard] == EMPTY)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0; //not clear
		}
	}
	else
	{
		return 1; //check just the move without capture 
	}
}

/******************
* Function Name: isWhiteChess
* Input: char board[][SIZE]
* Output: true or flase
* Function Operation: the function check if there is chess to the white
******************/
int isWhiteChess(char board[][SIZE])
{
	GameLocation king = findMyPiece(board, 'K', -1, -1, 0, 0);
	int isWhiteTurn = 1;
	// in case the location is invalid
	if (isValidLocation(king) == 0)
	{
		return 0;
	}
	//this part checks which one of the black pieces in the game can move to the kings location
	for (int row = 0; row < SIZE; row++)
	{
		for (int col = 0; col < SIZE; col++)
		{
			GameLocation curLocation = locationInGame(row, col);
			if (board[row][col] == 'p') //if there is a black pawn - check if he can get to the king
			{
				if (legalPathPawn(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn, board) == 1)
				{
					if (clearPathPawn(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'q') //if there is a black queen - check if she can get to the king
			{
				if (legalPathQueen(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathQueen(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'b') //if there is a black bishop - check if he can get to the king
			{
				if (legalPathBishop(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathBishop(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'r') //if there is a black rook - check if he can get to the king
			{
				if (legalPathRook(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathRook(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'n') //if there is a black knight - check if he can get to the king
			{
				if (legalPathKnight(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathKnight(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
		}
	}
	return 0;
}

/******************
* Function Name: isBlackChess
* Input: char board[][SIZE], int isWhiteTurn
* Output: true or flase
* Function Operation: the function check if there is chess to the black
******************/
int isBlackChess(char board[][SIZE])
{
	GameLocation kingLocation = findMyPiece(board, 'k', -1, -1, 0, 0);
	int isWhiteTurn = 1;
	//this loop find the location of the white king on the board and save it in the struct
	// in case the location is invalid
	if (isValidLocation(kingLocation) == 0)
	{
		return 0;
	}
	GameLocation king = locationInGame(kingLocation.rowInBoard, kingLocation.colInBoard);
	//this part check which of the black piece in the game have can move to the king location
	for (int row = 0; row < SIZE; row++)
	{
		for (int col = 0; col < SIZE; col++)
		{
			GameLocation curLocation = locationInGame(row, col);
			if (board[row][col] == 'P') //if there is a black pawn - check if he can get to the king
			{
				if (legalPathPawn(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn, board) == 1)
				{
					if (clearPathPawn(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'Q') //if there is a black queen - check if she can get to the king
			{
				if (legalPathQueen(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathQueen(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'B') //if there is a black bishop - check if he can get to the king
			{
				if (legalPathBishop(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathBishop(board, row, col, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'R') //if there is a black rook - check if he can get to the king
			{
				if (legalPathRook(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathRook(board, row, col, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
			if (board[row][col] == 'N') //if there is a black knight - check if he can get to the king
			{
				if (legalPathKnight(curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard) == 1)
				{
					if (clearPathKnight(board, curLocation.rowInBoard, curLocation.colInBoard, king.rowInBoard, king.colInBoard, isWhiteTurn) == 1)
					{
						return 1; //chess
					}
				}
			}
		}
	}
	return 0;
}

/******************
* Function Name: moveKing
* Input: char pgn[], char board[][SIZE], int isWhiteTurn
* Output: true or false
* Function Operation: the function check if the king can make the move acording to PGN string
******************/
int moveKing(char pgn[], char board[][SIZE], int isWhiteTurn)
{
	int i = 1; // index of pgn string - starts from i=1 because i=0 is the type of the piece 
			   // This variabls are use to get information from PGN string
	char srcRow, srcCol, destRow, destCol; //location variabls
	int isCapture = 0, isChess = 0; //flages for chess and capture
	char tempBoard[SIZE][SIZE]; //temporary board
	GameLocation kingLocation;

	//in case the move is not legal - retuen flase
	int strSize = 0; //check the length of the PGN string
	while (pgn[strSize] != '\0')
	{
		strSize++;
	}
	if (pgn[i] == 'x') // check if the piece capture another piece 
	{
		isCapture = 1;
		i++;
	}
	if (pgn[i] >= 'a' && pgn[i] <= 'h') //check coulumn destination
	{
		destCol = pgn[i];
		i++;
	}
	else //in case it's not legal 
	{
		return 0;
	}
	if (pgn[i] <= '0' + 8 && pgn[i] >= '0' + 1) //check the row destination of the piece
	{
		destRow = pgn[i];
		i++;
	}
	else //in case it's not legal 
	{
		return 0;
	}
	if ((strSize - 1 == i) && (pgn[i] == '+' || pgn[i] == '#')) // check chess
	{
		isChess = 1; // in case the king allows another piece to chess the other king
	}

	// finding the kings position
	if (isWhiteTurn == 1) //if its the white player turn
	{
		kingLocation = findMyPiece(board, 'K', -1, -1, 0, 0);
	}
	else //this is the black player turn
	{
		kingLocation = findMyPiece(board, 'k', -1, -1, 0, 0);
	}
	// this parts validates the location
	if (isValidLocation(kingLocation) == 0)
	{
		return 0;
	}
	else
	{
		srcRow = kingLocation.rowInBoard;
		srcCol = kingLocation.colInBoard;
	}

	//check if the capture was legal
	if (isCapture == 1)
	{
		int legalCap = checkCapture(board, destRow, destCol, isWhiteTurn);
		if (legalCap == 0) //if it wasnt legal - retuen move not legal
		{
			return 0;
		}
	}

	//check if the king moves acording to the game ruels, if not - return move not legal 
	if (legalPathKing(srcRow, srcCol, destRow, destCol) == 0)
	{
		return 0;
	}
	//check if path is empty or there is piece of the other player - if the king can move
	if (clearPathKing(board, destRow, destCol, isWhiteTurn) == 0)
	{
		return 0;
	}

	//this loop copy the board to a temporary board
	for (int r = 0; r <= SIZE; r++)
	{
		for (int c = 0; c <= SIZE; c++)
		{
			tempBoard[r][c] = board[r][c]; //copy board to a temporary board
		}
	}
	Location srcLocation = locationInBoard(srcRow, srcCol);
	Location destLocation = locationInBoard(destRow, destCol);
	tempBoard[srcLocation.rowInBoard][srcLocation.colInBoard] = EMPTY;
	if (isWhiteTurn == 1)
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0]; //move the piece to the destination board
	}
	else
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0] - 'A' + 'a';
	}

	// check if it's whites turn and if so - if white is in chess return false
	if (isWhiteTurn == 1)
	{
		if (isWhiteChess(tempBoard))
		{
			return 0;
		}
		// in case white has moved and created a state of chess for the black
		else if (isChess == 1 && isBlackChess(tempBoard) == 0)
		{
			return 0;
		}
	}
	else
	{
		if (isBlackChess(tempBoard))
		{
			return 0;
		}
		// in case white has moved and created a state of chess for the black
		else if (isChess == 1 && isWhiteChess(tempBoard) == 0)
		{
			return 0;
		}
	}

	// copy tempBoard to board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			board[r][c] = tempBoard[r][c]; //copy board to a temporary board
		}
	}
	return 1;
}

/******************
* Function Name: moveQueen
* Input: char pgn[], char board[][SIZE], int isWhiteTurn
* Output: true or false
* Function Operation: the function check if the Queen can make the move acording to PGN string
******************/
int moveQueen(char pgn[], char board[][SIZE], int isWhiteTurn)
{
	int i = 1; // index of pgn string - starts from i=1 because i=0 is the type of the piece 
			   // This variabls are use to get information from PGN string
	char srcRow, srcCol, destRow, destCol; //location variabls
	int isCapture = 0, isChess = 0, useCol = 0, useRow = 0; //flages for chess and capture
	char tempBoard[SIZE][SIZE]; //temporary board
	pgnState curPgn;
	GameLocation queenLocation;

	//in case the move is not legal - retuen flase
	int strSize = 0; //check the length of the PGN string
	while (pgn[strSize] != '\0')
	{
		strSize++;
	}

	curPgn = parsePGN(pgn);
	if (curPgn.isBad == 1)
	{
		return 0;
	}
	srcCol = curPgn.srcCol;
	srcRow = curPgn.srcRow;
	useCol = curPgn.useCol;
	useRow = curPgn.useRow;
	destCol = curPgn.destCol;
	destRow = curPgn.destRow;
	isCapture = curPgn.isCapture;
	isChess = curPgn.isChess;


	// finding the kings position
	if (isWhiteTurn == 1) //if its the white player turn
	{
		queenLocation = findMyBNRQ(board, 'Q', srcRow, srcCol, destRow, destCol, useRow, useCol, isWhiteTurn);
	}
	else //this is the black player turn
	{
		queenLocation = findMyBNRQ(board, 'q', srcRow, srcCol, destRow, destCol, useRow, useCol, isWhiteTurn);
	}
	// this parts validates the location
	if (isValidLocation(queenLocation) == 0)
	{
		return 0;
	}
	else
	{
		srcRow = queenLocation.rowInBoard;
		srcCol = queenLocation.colInBoard;
	}

	//check if the capture was legal
	if (isCapture == 1)
	{
		int legalCap = checkCapture(board, destRow, destCol, isWhiteTurn);
		if (legalCap == 0) //if it wasnt legal - retuen move not legal
		{
			return 0;
		}
	}

	//check if the queen moves acording to the game ruels, if not - return move not legal 
	if (legalPathQueen(srcRow, srcCol, destRow, destCol) == 0)
	{
		return 0;
	}
	//check if path is empty or there is piece of the other player - if the queen can move
	if (clearPathQueen(board, srcRow, srcCol, destRow, destCol, isWhiteTurn) == 0)
	{
		return 0;
	}

	//this loop copy the board to a temporary board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			tempBoard[r][c] = board[r][c]; //copy board to a temporary board
		}
	}
	Location srcLocation = locationInBoard(srcRow, srcCol);
	Location destLocation = locationInBoard(destRow, destCol);
	tempBoard[srcLocation.rowInBoard][srcLocation.colInBoard] = EMPTY;
	if (isWhiteTurn == 1)
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0]; //move the piece to the destination board
	}
	else
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0] - 'A' + 'a';
	}

	// check if it's whites turn and if so - if white is in chess return false
	if (isWhiteTurn == 1)
	{
		if (isBlackChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}
	else
	{
		if (isWhiteChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}

	// copy tempBoard to board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			board[r][c] = tempBoard[r][c]; //copy board to a temporary board
		}
	}
	return 1;
}

/******************
* Function Name: moveRook
* Input: char pgn[], char board[][SIZE], int isWhiteTurn
* Output: true or false
* Function Operation: the function check if the Rook can make the move acording to PGN string
******************/
int moveRook(char pgn[], char board[][SIZE], int isWhiteTurn)
{
	int i = 1; // index of pgn string - starts from i=1 because i=0 is the type of the piece 
				// This variabls are use to get information from PGN string
	char srcRow = 0, srcCol = 0, destRow, destCol, piece; //location variabls
	int isCapture = 0, isChess = 0, useRow = 0, useCol = 0; //flages for chess and capture
	char tempBoard[SIZE][SIZE]; //temporary board
	GameLocation rookLocation;
	pgnState curPgn;
	//in case the move is not legal - retuen flase
	int strSize = 0; //check the length of the PGN string
	while (pgn[strSize] != '\0')
	{
		strSize++;
	}

	// parse pgn
	curPgn = parsePGN(pgn);
	if (curPgn.isBad == 1)
	{
		return 0;
	}
	srcCol = curPgn.srcCol;
	srcRow = curPgn.srcRow;
	useCol = curPgn.useCol;
	useRow = curPgn.useRow;
	destCol = curPgn.destCol;
	destRow = curPgn.destRow;
	isCapture = curPgn.isCapture;
	isChess = curPgn.isChess;

	// finding the Rook position
	if (isWhiteTurn == 1) //if its the white player turn
	{
		piece = 'R';
	}
	else //this is the black player turn
	{
		piece = 'r';
	}
	rookLocation = findMyBNRQ(board, piece, srcRow, srcCol, destRow, destCol, useRow, useCol, isWhiteTurn);
	// this parts validates the location
	if (isValidLocation(rookLocation) == 0)
	{
		return 0;
	}
	else
	{
		srcRow = rookLocation.rowInBoard;
		srcCol = rookLocation.colInBoard;
	}

	//check if the capture was legal
	int legalCap = checkCapture(board, destRow, destCol, isWhiteTurn);
	if (isCapture && !legalCap)
	{
		return 0;
	}
	else if (!isCapture && legalCap)
	{
		return 0;
	}
	
	//check if the rook moves acording to the game ruels, if not - return move not legal 
	if (legalPathRook(srcRow, srcCol, destRow, destCol) == 0)
	{
		return 0;
	}
	//check if path is empty or there is piece of the other player - if the rook can move
	if (clearPathRook(board, srcRow, srcCol, destRow, destCol, isWhiteTurn) == 0)
	{
		return 0;
	}

	//this loop copy the board to a temporary board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			tempBoard[r][c] = board[r][c]; //copy board to a temporary board
		}
	}
	Location srcLocation = locationInBoard(srcRow, srcCol);
	Location destLocation = locationInBoard(destRow, destCol);
	tempBoard[srcLocation.rowInBoard][srcLocation.colInBoard] = EMPTY;
	if (isWhiteTurn == 1)
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0]; //move the piece to the destination board
	}
	else
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0] - 'A' + 'a';
	}

	// check if it's whites turn and if so - if white is in chess return false
	if (isWhiteTurn == 1)
	{
		if (isWhiteChess(tempBoard))
		{
			return 0;
		}
		if (isBlackChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}
	else
	{
		if (isBlackChess(tempBoard))
		{
			return 0;
		}
		if (isWhiteChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}

	// copy tempBoard to board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			board[r][c] = tempBoard[r][c]; //copy board to a temporary board
		}
	}
	return 1;
}

/******************
* Function Name: moveBishop
* Input: char pgn[], char board[][SIZE], int isWhiteTurn
* Output: true or false
* Function Operation: the function check if the Bishop can make the move acording to PGN string
******************/
int moveBishop(char pgn[], char board[][SIZE], int isWhiteTurn)
{
	int i = 1; // index of pgn string - starts from i=1 because i=0 is the type of the piece 
				// This variabls are use to get information from PGN string
	char srcRow = 0, srcCol = 0, destRow, destCol; //location variabls
	int isCapture = 0, isChess = 0, useRow = 0, useCol = 0; //flages for chess and capture
	char tempBoard[SIZE][SIZE]; //temporary board
	GameLocation bishopLocation;
	pgnState curPgn;
	//in case the move is not legal - retuen flase
	int strSize = 0; //check the length of the PGN string

	// parse pgn
	curPgn = parsePGN(pgn);
	if (curPgn.isBad == 1)
	{
		return 0;
	}

	srcCol = curPgn.srcCol;
	srcRow = curPgn.srcRow;
	useCol = curPgn.useCol;
	useRow = curPgn.useRow;
	destCol = curPgn.destCol;
	destRow = curPgn.destRow;
	isCapture = curPgn.isCapture;
	isChess = curPgn.isChess;

	// finding the Bishop position
	if (isWhiteTurn == 1) //if its the white player turn
	{
		bishopLocation = findMyBNRQ(board, 'B', srcRow, srcCol, destRow, destCol, useRow, useCol, isWhiteTurn);

	}
	else //this is the black player turn
	{
		bishopLocation = findMyBNRQ(board, 'b', srcRow, srcCol, destRow, destCol, useRow, useCol, isWhiteTurn);
	}
	// this parts validates the location
	if (isValidLocation(bishopLocation) == 0)
	{
		return 0;
	}
	else
	{
		srcRow = bishopLocation.rowInBoard;
		srcCol = bishopLocation.colInBoard;
	}

	//check if the capture was legal
	if (isCapture == 1)
	{
		int legalCap = checkCapture(board, destRow, destCol, isWhiteTurn);
		if (legalCap == 0) //if it wasnt legal - retuen move not legal
		{
			return 0;
		}
	}
	//check if the bishop moves acording to the game ruels, if not - return move not legal 
	if (legalPathBishop(srcRow, srcCol, destRow, destCol) == 0)
	{
		return 0;
	}
	//check if path is empty or there is piece of the other player - if the bishop can move
	if (clearPathBishop(board, srcRow, srcCol, destRow, destCol, isWhiteTurn) == 0)
	{
		return 0;
	}

	//this loop copy the board to a temporary board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			tempBoard[r][c] = board[r][c]; //copy board to a temporary board
		}
	}
	Location srcLocation = locationInBoard(srcRow, srcCol);
	Location destLocation = locationInBoard(destRow, destCol);
	tempBoard[srcLocation.rowInBoard][srcLocation.colInBoard] = EMPTY;
	if (isWhiteTurn == 1)
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0]; //move the piece to the destination board
	}
	else
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0] - 'A' + 'a';
	}

	// check if it's whites turn and if so - if white is in chess return false
	if (isWhiteTurn == 1)
	{
		if (isWhiteChess(tempBoard))
		{
			return 0;
		}
		if (isBlackChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}
	else
	{
		if (isBlackChess(tempBoard))
		{
			return 0;
		}
		if (isWhiteChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}

	// copy tempBoard to board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			board[r][c] = tempBoard[r][c]; //copy board to a temporary board
		}
	}
	return 1;
}

/******************
* Function Name: moveKnight
* Input: char pgn[], char board[][SIZE], int isWhiteTurn
* Output: true or false
* Function Operation: the function check if the Knight can make the move acording to PGN string
******************/
int moveKnight(char pgn[], char board[][SIZE], int isWhiteTurn)
{
	int i = 1; // index of pgn string - starts from i=1 because i=0 is the type of the piece 
				// This variabls are use to get information from PGN string
	char srcRow = 0, srcCol = 0, destRow, destCol; //location variabls
	int isCapture = 0, isChess = 0, useRow = 0, useCol = 0; //flages for chess and capture
	char tempBoard[SIZE][SIZE]; //temporary board
	GameLocation knightLocation;
	pgnState curPgn;
	//in case the move is not legal - retuen flase
	int strSize = 0; //check the length of the PGN string
	while (pgn[strSize] != '\0')
	{
		strSize++;
	}

	// parse pgn
	curPgn = parsePGN(pgn);
	if (curPgn.isBad == 1)
	{
		return 0;
	}
	srcCol = curPgn.srcCol;
	srcRow = curPgn.srcRow;
	useCol = curPgn.useCol;
	useRow = curPgn.useRow;
	destCol = curPgn.destCol;
	destRow = curPgn.destRow;
	isCapture = curPgn.isCapture;
	isChess = curPgn.isChess;

	// finding the knight position
	if (isWhiteTurn == 1) //if its the white player turn
	{
		knightLocation = findMyBNRQ(board, 'N', srcRow, srcCol, destRow, destCol, useRow, useCol, isWhiteTurn);
	}
	else //this is the black player turn
	{
		knightLocation = findMyBNRQ(board, 'n', srcRow, srcCol, destRow, destCol, useRow, useCol, isWhiteTurn);
	}
	// this parts validates the location
	if (isValidLocation(knightLocation) == 0)
	{
		return 0;
	}
	else
	{
		srcRow = knightLocation.rowInBoard;
		srcCol = knightLocation.colInBoard;
	}

	//check if the capture was legal
	if (isCapture == 1)
	{
		int legalCap = checkCapture(board, destRow, destCol, isWhiteTurn);
		if (legalCap == 0) //if it wasnt legal - retuen move not legal
		{
			return 0;
		}
	}

	//check if the knight moves acording to the game ruels, if not - return move not legal 
	if (legalPathKnight(srcRow, srcCol, destRow, destCol) == 0)
	{
		return 0;
	}
	//check if path is empty or there is piece of the other player - if the knight can move
	if (clearPathKnight(board, srcRow, srcCol, destRow, destCol, isWhiteTurn) == 0)
	{
		return 0;
	}

	//this loop copy the board to a temporary board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			tempBoard[r][c] = board[r][c]; //copy board to a temporary board
		}
	}
	Location srcLocation = locationInBoard(srcRow, srcCol);
	Location destLocation = locationInBoard(destRow, destCol);
	tempBoard[srcLocation.rowInBoard][srcLocation.colInBoard] = EMPTY;
	if (isWhiteTurn == 1)
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0]; //move the piece to the destination board
	}
	else
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = pgn[0] - 'A' + 'a';
	}

	// check if it's whites turn and if so - if white is in chess return false
	if (isWhiteTurn == 1)
	{
		if (isWhiteChess(tempBoard))
		{
			return 0;
		}
		if (isBlackChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}
	else
	{
		if (isBlackChess(tempBoard))
		{
			return 0;
		}
		if (isWhiteChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}

	// copy tempBoard to board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			board[r][c] = tempBoard[r][c]; //copy board to a temporary board
		}
	}
	return 1;
}

/******************
* Function Name: movePawn
* Input: char pgn[], char board[][SIZE], int isWhiteTurn
* Output: true or false
* Function Operation: the function check if the Pawn can make the move acording to PGN string
******************/
int movePawn(char pgn[], char board[][SIZE], int isWhiteTurn)
{
	int i = 0; // index of pgn string - starts from i=0 because i=0 is the destination
					// This variabls are use to get information from PGN string
	char destRow, destCol, srcRow, srcCol = 0; //location variabls
	int isCapture = 0, isChess = 0, useCol = 0, doPromotion = 0; //flages for chess and capture
	char tempBoard[SIZE][SIZE]; //temporary board
	char newPiece; //the new piece after the promotion
	GameLocation pawnLocation;


	//in case the move is not legal - retuen flase
	int strSize = 0; //check the length of the PGN string
	while (pgn[strSize] != '\0')
	{
		strSize++;
	}
	if (pgn[1] == 'x') // check if the piece capture another piece 
	{
		isCapture = 1;
	}
	if (pgn[i] >= 'a' && pgn[i] <= 'h') //check coulumn destination
	{
		if (isCapture == 1)
		{
			srcCol = pgn[i];
			destCol = pgn[i + 2];
			useCol = 1;
			i += 3;
		}
		else
		{
			destCol = pgn[i];
			i++;
		}
	}
	else //in case it's not legal 
	{
		return 0;
	}
	if (pgn[i] <= '0' + 8 && pgn[i] >= '0' + 1) //check the row destination of the piece
	{
		destRow = pgn[i];
		i++;
	}
	else //in case it's not legal 
	{
		return 0;
	}
	// finding the pawn position
	pawnLocation = findMyPawn(board, destRow, destCol, srcCol, useCol, isWhiteTurn);
	// this parts validates the location
	if (isValidLocation(pawnLocation) == 0)
	{
		return 0;
	}
	else
	{
		srcRow = pawnLocation.rowInBoard;
		srcCol = pawnLocation.colInBoard;
	}
	if (pgn[i] == '=') // check if the piece capture another piece 
	{
		i++;
		if (pgn[i] == 'Q' || pgn[i] == 'B' || pgn[i] == 'N' || pgn[i] == 'R')
		{
			newPiece = pgn[i];
		}
		doPromotion = 1;
		i++;
	}
	if ((strSize - 1 == i) && (pgn[i] == '+' || pgn[i] == '#')) // check chess
	{
		isChess = 1; // in case the king allows another piece to chess the other king
	}

	//check if the capture was legal
	if (isCapture == 1)
	{
		int legalCap = checkCapture(board, destRow, destCol, isWhiteTurn);
		if (legalCap == 0) //if it wasnt legal - retuen move not legal
		{
			return 0;
		}
	}

	//check if the rook moves acording to the game ruels, if not - return move not legal 
	if (legalPathPawn(srcRow, srcCol, destRow, destCol, isWhiteTurn, board) == 0)
	{
		return 0;
	}
	//check if path is empty or there is piece of the other player - if the rook can move
	if (clearPathPawn(board, srcRow, srcCol, destRow, destCol) == 0)
	{
		return 0;
	}

	Location srcLocation = locationInBoard(srcRow, srcCol);
	Location destLocation = locationInBoard(destRow, destCol);
	//this loop copy the board to a temporary board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			tempBoard[r][c] = board[r][c]; //copy board to a temporary board
		}
	}
	tempBoard[srcLocation.rowInBoard][srcLocation.colInBoard] = EMPTY;
	if (doPromotion == 1 && isWhiteTurn == 1)
	{
		isPromotion(tempBoard, destLocation.rowInBoard, destLocation.colInBoard, isWhiteTurn, newPiece);
	}
	else if (doPromotion == 1 && isWhiteTurn == 0)
	{
		isPromotion(tempBoard, destLocation.rowInBoard, destLocation.colInBoard, isWhiteTurn, newPiece - 'A' + 'a');
	}
	else if (isWhiteTurn == 1)
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = 'P'; //move the piece to the destination board
	}
	else
	{
		tempBoard[destLocation.rowInBoard][destLocation.colInBoard] = 'p';
	}

	// check if it's whites turn and if so - if white is in chess return false
	if (isWhiteTurn == 1)
	{
		if (isBlackChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}
	else
	{
		if (isWhiteChess(tempBoard) == 1 && isChess == 0)
		{
			return 0;
		}
	}

	// copy tempBoard to board
	for (int r = 0; r < SIZE; r++)
	{
		for (int c = 0; c < SIZE; c++)
		{
			board[r][c] = tempBoard[r][c]; //copy board to a temporary board
		}
	}
	return 1;
}


/******************
*Function Name : makeMove
*Input : char board[][SIZE], char pgn[], int isWhiteTurn
	*Output : make move in the game
	*Function Operation : the function gets PGN string and check if it's legal move acording to the game ruels if yes - the function will change the board
******************/
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	if (pgn[0] == 'K')
	{
		if (moveKing(pgn, board, isWhiteTurn) == 0)
		{
			return 0;
		}
	}
	if (pgn[0] == 'Q')
	{
		if (moveQueen(pgn, board, isWhiteTurn) == 0)
		{
			return 0;
		}
	}
	if (pgn[0] == 'R')
	{
		if (moveRook(pgn, board, isWhiteTurn) == 0)
		{
			return 0;
		}
	}
	if (pgn[0] == 'B')
	{
		if (moveBishop(pgn, board, isWhiteTurn) == 0)
		{
			return 0;
		}
	}
	if (pgn[0] == 'N')
	{
		if (moveKnight(pgn, board, isWhiteTurn) == 0)
		{
			return 0;
		}
	}
	if (pgn[0] >= 'a' && pgn[0] <= 'h')
	{
		if (movePawn(pgn, board, isWhiteTurn) == 0)
		{
			return 0;
		}
	}
	return 1;
}
