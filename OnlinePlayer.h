#ifndef ONLINEPLAYER_H
#define ONLINEPLAYER_H
#include "Player.h"

class OnlinePlayer : public Player {
	private:
	public:
		inline OnlinePlayer(short int color) : Player(color) {}
		inline string makeMove(short int Board[64], short int enPassantCol) override { return "0"; }
		inline void updateCastlingRights(bool blackLongCastle, bool blackShortCastle, bool whiteShortCastle, bool whiteLongCastle) {}
};

#endif