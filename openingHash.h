#ifndef OPENINGHASH_H
#define OPENINGHASH_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>

using namespace std;

typedef unsigned long long int ulong;

class openingHash {
	private:
		ulong pieceHashes[2][6][8][8] = { 0ULL };
		ulong enPassantHashes[2][8] = { 0ULL };
		ulong castlingRights[2][2] = { 0ULL };
		inline void initPieceHashes();
		inline void initEnPassant();
		inline void initCastlingRights();
		inline ulong hashBoard(int Board[64], bool castlingAbility[4], int enPassantCol, int whiteMove);
		inline vector<string> obtainMoves(string hashLine);
		inline string chooseOpeningMove(vector<string> moves, int board[64], int enPassantCol, int whiteMove);
		inline string notationToMove(int board[64], string notation, int enPassantCol, int whiteMove);
		inline void oldlegalMoves(int Board[64], vector<int>& legalMoves, int selectSquare, int enPassantCol);
		inline int color(int val);
	public:
		inline openingHash();
		inline string searchHash(int Board[64], bool castlingAbility[4], int enPassantCol, int whiteMove);
};

#endif