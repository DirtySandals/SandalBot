#ifndef BOT_H
#define BOT_H
#include <vector>
#include <chrono>
#include <thread>
#include <unordered_map>
#include "transpositionTable.cpp"
#include "openingHash.cpp"
#include "Player.h"

using namespace chrono;

typedef unsigned long long int ulong;

class Bot : public Player {
	private:
		string miniMaxMove;
		//transpositionTable tTable = transpositionTable(10000000);
        openingHash openingBook;
        unsigned short int numDepth;
		int transpositions = 0;
		unsigned int nodes = 0;
		unsigned int nodeSum = 0;
		unsigned int numBotMoves = 1;
		unsigned int dNdt = 0;
        unsigned int fastestMove = 0;
		unsigned short int whiteCastled = 0;
		unsigned short int blackCastled = 0;
        double displayMove = 1;
        bool exitedOpening = false;
        //0=bl,1=bs,2=ws,3=wl
        bool castlingAbility[4] = { true, true, true, true };
		ulong pieceHashes[12][64] = { 0ULL };
		ulong enPassantHashes[16] = { 0ULL };
		inline void initPieceHashes();
		inline void initEnPassant();
        short int whitepawneval[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 30, 30, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0 };
        short int blackpawneval[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            5, 10, 10, -20, -20, 10, 10, 5,
            5, -5, -10, 0, 0, -10, -5, 5,
            0, 0, 0, 30, 30, 0, 0, 0,
            5, 5, 10, 25, 25, 10, 5, 5,
            10, 10, 20, 30, 30, 20, 10, 10,
            50, 50, 50, 50, 50, 50, 50, 50,
            0, 0, 0, 0, 0, 0, 0, 0 };
        short int knighteval[64] = {
            -50, -40, -30, -30, -30, -30, -40, -50,
            -40, -20, 0, 0, 0, 0, -20, -40,
            -30, 0, 10, 15, 15, 10, 0, -30,
            -30, 5, 15, 20, 20, 15, 5, -30,
            -30, 5, 15, 20, 20, 15, 5, -30,
            -30, 0, 10, 15, 15, 10, 0, -30,
            -40, -20, 0, 0, 0, 0, -20, -40,
            -50, -40, -30, -30, -30, -30, -40, -50 };
        short int whitebishopeval[64] = {
            -20, -10, -10, -10, -10, -10, -10, -20,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -10, 0, 5, 10, 10, 5, 0, -10,
            -10, 5, 5, 10, 10, 5, 5, -10,
            -10, 0, 10, 10, 10, 10, 0, -10,
            -10, 10, 10, 10, 10, 10, 10, -10,
            -10, 15, 0, 0, 0, 0, 15, -10,
            -20, -10, -10, -10, -10, -10, -10, -20 };
        short int blackbishopeval[64] = {
            -20, -10, -10, -10, -10, -10, -10, -20,
            -10, 15, 0, 0, 0, 0, 15, -10,
            -10, 10, 10, 10, 10, 10, 10, -10,
            -10, 0, 10, 10, 10, 10, 0, -10,
            -10, 5, 5, 10, 10, 5, 5, -10,
            -10, 0, 5, 10, 10, 5, 0, -10,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -20, -10, -10, -10, -10, -10, -10, -20 };
        short int whiterookeval[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            5, 10, 10, 10, 10, 10, 10, 5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            0, 0, 0, 5, 5, 0, 0, 0 };
        short int blackrookeval[64] = {
            0, 0, 0, 5, 5, 0, 0, 0,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            5, 10, 10, 10, 10, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0 };
        short int whitequeeneval[64] = {
            -20, -10, -10, -5, -5, -10, -10, -20,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -10, 0, 5, 5, 5, 5, 0, -10,
            -5, 0, 5, 5, 5, 5, 0, -5,
            0, 0, 5, 5, 5, 5, 0, -5,
            -10, 5, 5, 5, 5, 5, 0, -10,
            -10, 0, 5, 0, 0, 0, 0, -10,
            -20, -10, -10, -5, -5, -10, -10, -20 };
        short int blackqueeneval[64] = {
            -20, -10, -10, -5, -5, -10, -10, -20,
            -10, 0, 5, 0, 0, 0, 0, -10,
            -10, 5, 5, 5, 5, 5, 0, -10,
            0, 0, 5, 5, 5, 5, 0, -5,
            -5, 0, 5, 5, 5, 5, 0, -5,
            -10, 0, 5, 5, 5, 5, 0, -10,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -20, -10, -10, -5, -5, -10, -10, -20 };
        short int whitekingeval[64] = {
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -20, -30, -30, -40, -40, -30, -30, -20,
            -10, -20, -20, -20, -20, -20, -20, -10,
            20, 20, 0, 0, 0, 0, 20, 20,
            20, 30, 29, 0, 0, 0, 20, 20 };
        short int blackkingeval[64] = {
            20, 30, 29, 0, 0, 0, 20, 20,
            20, 20, 0, 0, 0, 0, 20, 20,
            -10, -20, -20, -20, -20, -20, -20, -10,
            -20, -30, -30, -40, -40, -30, -30, -20,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30,
            -30, -40, -40, -50, -50, -40, -40, -30 };
        unordered_map<string, unsigned short int> positions;
        inline void copyBoard(int emptyBoard[64], short int actualBoard[64]);
        inline vector<int> getPieces(short int board[64], short int pieceColor);
        inline void updateDNdt(int newNodes);
        inline string commaSeparator(int integer);
        inline int miniMax(int board[64], int depth, bool isMaximising, int enPassantCol, vector<int>& ownPieces, vector<int>& oppositePieces, int alpha, int beta);
        inline bool legalMoves_mm(int Board[64], int selectSquare, int enPassantCol, bool isMaximising, int depth, vector<int>& ownPieces, vector<int>& oppositePieces, const int selectIndex, int& alpha, int& beta);
        inline void continueSearch(int board[64], bool isMaximising, int depth, int enPassantCol, vector<int>& ownPieces, vector<int>& oppositePieces, int& alpha, int& beta, int selectSquare, int targetSquare, int promotion);
        inline void alterBoard(int board[64], int selectSquare, int targetSquare, vector<int>& oppositePieces, vector<int>& ownPieces, int& deleteIndex, int& takenPiece, int selectIndex);
        inline void undoMove(int board[64], int selectSquare, int targetSquare, vector<int>& oppositePieces, vector<int>& ownPieces, int deleteIndex, int takenPiece, int movingPiece, int selectIndex);
        inline bool isCheck_mm(int Board[64], int whiteMove);
        inline bool checkmate_mm(int Board[64], int whitemove, vector<int>& ownPieces, int enPassantCol);
        inline bool castleThroughCheck_mm(int Board[64], int destinationSquare, int whitemove);
        inline bool searchingKingAttack(int Board[64], int kingSquare, int whiteMove);
        inline void changeBoard_mm(int Board[64], int selectSquare, int targetSquare, int promotion);
        inline ulong hashBoard(int Board[64], int whiteMove, int enPassantCol);
        inline int ForceKingToCornerEndgameEval(int friendlySquare, int opposingSquare);
        inline int color(int val);
        inline int deletePiece(int square, vector<int>& pieces);
        inline void oldlegalMoves(int Board[64], vector<int>& legalMoves, int selectSquare, int enPassantCol);
        inline int eval(int board[64], bool isMaximising, int enPassantCol, vector<int>& ownPieces, vector<int>& oppositePieces);        
	public:
        inline Bot(short int whiteMove, unsigned short int numDepth);
        inline string makeMove(short int Board[64], short int enPassantCol) override;
        inline void updateCastlingRights(bool blackLongCastle, bool blackShortCastle, bool whiteShortCastle, bool whiteLongCastle);
};

#endif