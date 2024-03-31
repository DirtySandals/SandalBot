#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H
#include <iostream>

typedef unsigned long long int ulong;
using namespace std;
struct Entry {
	ulong key;
	short int value;
	short int depth;
	short int nodesSaved;
	short int whiteMove;
};
class transpositionTable {
private:
	int tableSize = 0;
	Entry* entries;
	ulong pieceHashes[2][6][8][8] = { 0ULL };
	ulong enPassantHashes[2][8] = { 0ULL };
	ulong castlingRights[2][2] = { 0ULL };
	inline void initPieceHashes();
	inline void initEnPassant();
	inline void initCastlingRights();
public:
	inline transpositionTable(int size);
	inline ~transpositionTable();
	inline int lookupEvaluation(int whiteMove, int depth, int& transpositions, ulong hash_key);
	inline void storeEvaluation(int whiteMove, int depth, int value, int nodes, ulong hash_key);
	inline ulong hashBoard(int Board[64], int whiteMove, int enPassantCol);
	inline void clear();
};
#endif