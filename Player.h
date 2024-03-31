#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>

using namespace std;

class Player {
	protected:
		short int whiteMove;
	public:
		inline Player() { whiteMove = 1; }
		inline Player(short int color) : whiteMove(color) {}
		inline virtual string makeMove(short int Board[64], short int enPassantCol) = 0;
		inline virtual void updateCastlingRights(bool blackLongCastle, bool blackShortCastle, bool whiteShortCastle, bool whiteLongCastle) = 0;
		inline short int getColor() { return whiteMove; }
};

#endif