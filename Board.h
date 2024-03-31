#pragma once
#ifndef BOARD_H
#define BOARD_H
#include <atomic>
#include <cctype>
#include <chrono>
#include <random>
#include <string>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <unordered_map>
#include "Human.h"
#include "Bot.cpp"
#include "OnlinePlayer.h"

using namespace std;

class Board
{
private:
    Player* player1 = nullptr;
    Player* player2 = nullptr;
    short int whiteMove = 1;
    unsigned int move = 1;
    double displayMove = 1;
    short int enPassantCol = -1;
    string moveList;
    bool whiteShortCastle = true;
    bool whiteLongCastle = true;
    bool blackShortCastle = true;
    bool blackLongCastle = true;
    unordered_map<string, unsigned short int> boardHistory;
    inline bool searchingKingAttack(short int Board[64], unsigned short int kingSquare, int whiteMove);
    inline vector<int> getPieces(short int whitemove);
    inline bool isType(unsigned short int playerNum, char playerIndex);
    
public:
    short int board[64] = { -5, -3, -4, -9, -100, -4, -3, -5, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 5, 3, 4, 9, 100, 4, 3, 5 };
    inline Board();
    inline ~Board();
    inline void updateHistory();
    inline bool isRepitition();
    inline bool isStalemate();
    inline void changeBoard(unsigned short int selectSquare, unsigned short int targetSquare, unsigned short int promotion);
    inline bool isCheck();
    inline bool isCheck(short int board[64]);
    inline bool isIllegalCheck(short int board[64]);
    inline bool checkmate();
    inline bool castleThroughCheck(unsigned short int selectSquare, unsigned short int targetSquare); 
    inline bool insuffienctMaterial();
    inline void legalMoves(vector<int>& legalMoves, unsigned short int selectSquare);
    inline int color(short int val);
    inline string arrtoFen();
    inline void copyBoard(short int emptyBoard[64]);
    inline string intToString(short int num);
    inline void assignPlayer1(char playerIndex, unsigned short int numDepth);
    inline void assignPlayer2(char playerIndex, unsigned short int numDepth);
    inline bool isMove(char playerIndex);
    inline string makeMove();
    inline void updateBotCastlingRights();
    inline int getwhiteMove() { return whiteMove; }
    inline void setwhiteMove() { whiteMove = -whiteMove; }
    inline int getMove() { return move; }
    inline void setMove() { move++; }
    inline double getDisplayMove() { return displayMove; }
    inline void setDisplayMove() { displayMove += 0.5; }
    inline string getMoveList() { return moveList; }
    inline void setMoveList(string coordinateSelect);
};

#endif