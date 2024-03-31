#include "transpositionTable.h"

transpositionTable::transpositionTable(int size) {
	tableSize = size;
	entries = new Entry[tableSize];
	for (int i = 0; i < tableSize; i++) {
		entries[i].key = 0ULL;
		entries[i].depth = 0;
		entries[i].value = 0;
	}
	initPieceHashes();
	initEnPassant();
}

transpositionTable::~transpositionTable() {
	delete[] entries;
}

void transpositionTable::initPieceHashes() {
	hash<ulong> inthash;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			for (int u = 0; u < 8; u++) {
				for (int v = 0; v < 8; v++) {
					pieceHashes[i][j][u][v] = inthash(ulong(1000 * i + 100 * j + 10 * u + v));
				}
			}
		}
	}
	return;
}

void transpositionTable::initEnPassant() {
	hash<ulong> hash;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			enPassantHashes[i][j] = hash(ulong(i * 10 + j));
		}
	}
}

void transpositionTable::initCastlingRights() {
	hash<ulong> hash;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			castlingRights[i][j] = hash(ulong(i * 10 + j));
		}
	}
	return;
}

ulong transpositionTable::hashBoard(int Board[64], int whiteMove, int enPassantCol) {
	ulong hash_key = 0ULL;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (Board[row * 8 + col] != 0) {
				int color, piece, selectedPiece;
				selectedPiece = Board[row * 8 + col];
				if (selectedPiece > 0) {
					color = 1;
				}
				else {
					color = 0;
				}
				switch (abs(selectedPiece)) {
				case 1:
					piece = 0;
					break;
				case 3:
					piece = 1;
					break;
				case 4:
					piece = 2;
					break;
				case 5:
					piece = 3;
					break;
				case 9:
					piece = 4;
					break;
				case 100:
					piece = 5;
					break;
				}
				hash_key ^= pieceHashes[color][piece][row][col];
			}

		}
	}
	if (enPassantCol != -1) {
		if (whiteMove == -1) {
			hash_key ^= enPassantHashes[1][enPassantCol];
		}
		else {
			hash_key ^= enPassantHashes[0][enPassantCol];
		}
	}
	return hash_key;
}

void transpositionTable::storeEvaluation(int whiteMove, int depth, int value, int nodes, ulong hash_key) {
	//ulong hash_key = hashBoard(Board, whiteMove, enPassantCol);
	if (entries[hash_key % tableSize].key != hash_key) {
		entries[hash_key % tableSize].key = hash_key;
		entries[hash_key % tableSize].depth = depth;
		entries[hash_key % tableSize].value = value;
		entries[hash_key % tableSize].nodesSaved = nodes;
		entries[hash_key % tableSize].whiteMove = whiteMove;
		return;
	}
	else {
		if (entries[hash_key % tableSize].depth > depth) {
			entries[hash_key % tableSize].key = hash_key;
			entries[hash_key % tableSize].depth = depth;
			entries[hash_key % tableSize].value = value;
			entries[hash_key % tableSize].nodesSaved = nodes;
			entries[hash_key % tableSize].whiteMove = whiteMove;
		}
		return;
	}
	return;
}

int transpositionTable::lookupEvaluation(int whiteMove, int depth, int& transpositions, ulong hash_key) {
	//ulong hash_key = hashBoard(Board, whiteMove, enPassantCol);
	if (entries[hash_key % tableSize].key == hash_key && entries[hash_key % tableSize].whiteMove == whiteMove) {
		if (entries[hash_key % tableSize].depth <= depth) {
			transpositions += entries[hash_key % tableSize].nodesSaved;
			return entries[hash_key % tableSize].value;
		}
	}

	return 999999;
}

void transpositionTable::clear() {
	for (int i = 0; i < tableSize; i++) {
		entries[i].key = 0ULL;
		entries[i].depth = 0;
		entries[i].value = 0;
	}
	return;
}