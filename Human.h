#ifndef HUMAN_H
#define HUMAN_H
#include "Player.h"

class Human : public Player {
	private:
	public:
		inline Human(short int color) : Player(color) {}
		inline string makeMove(short int Board[64], short int enPassantCol) override { return "0"; }
		inline void updateCastlingRights(bool blackLongCastle, bool blackShortCastle, bool whiteShortCastle, bool whiteLongCastle) {}
};

#endif