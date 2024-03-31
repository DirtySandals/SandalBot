#include "openingHash.h"

openingHash::openingHash() {
	initPieceHashes();
	initEnPassant();
	initCastlingRights();
}

void openingHash::initPieceHashes() {
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

void openingHash::initEnPassant() {
	hash<ulong> hash;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			enPassantHashes[i][j] = hash(ulong(i * 10 + j));
		}
	}
}

void openingHash::initCastlingRights() {
	hash<ulong> hash;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			castlingRights[i][j] = hash(ulong(i * 10 + j));
		}
	}
}

ulong openingHash::hashBoard(int Board[64], bool castlingAbility[4], int enPassantCol, int whiteMove) {
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
	if (castlingAbility[0]) {
		hash_key ^= castlingRights[0][0];
	}
	if (castlingAbility[1]) {
		hash_key ^= castlingRights[0][1];
	}
	if (castlingAbility[3]) {
		hash_key ^= castlingRights[1][0];
	}
	if (castlingAbility[2]) {
		hash_key ^= castlingRights[1][1];
	}
	return hash_key;
}

vector<string> openingHash::obtainMoves(string hashLine) {
	vector<string> moves;
	string move = "";

	for (int j = 0; j < hashLine.size(); j++) {
		if (hashLine[j] != '\n') {
			if (hashLine[j] != ' ') {
				move += hashLine[j];
			}
			else {
				moves.emplace_back(move);
				move = "";
			}
		}
	}
	if (move.size() > 1) {
		moves.emplace_back(move);
	}

	return moves;
}

string openingHash::chooseOpeningMove(vector<string> moves, int board[64], int enPassantCol, int whiteMove) {
	string chosenMove;
	while (moves.size() != 0) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, moves.size() - 1);
		int randomIndex = dist(gen);
		chosenMove = moves[randomIndex];
		cout << "move selected: " << chosenMove << endl;
		chosenMove = notationToMove(board, chosenMove, enPassantCol, whiteMove);
		if (chosenMove == "error") {
			moves.erase(moves.begin() + randomIndex);
		}
		else {
			break;
		}
	}
	return chosenMove;
}

string openingHash::searchHash(int Board[64], bool castlingAbility[4], int enPassantCol, int whiteMove) {
    bool openingFound = false;
    bool reachedEnd = false;
	ulong hash_key = hashBoard(Board, castlingAbility, enPassantCol, whiteMove);
	ulong totalFileSize = 17000000;
	ulong filesize = totalFileSize / 100;
	ulong fileLineIndex = hash_key % totalFileSize;
	ulong datasetNum = fileLineIndex / filesize;
	ulong startingIndex = fileLineIndex - datasetNum * filesize;
    ifstream openings("openings\\openingHash" + to_string(datasetNum + 1) + ".txt");
    ifstream openings2("openings\\openingHash" + to_string(datasetNum + 2) + ".txt");
    vector<string> moveSelect;
    string currentHashLine;
    string moves = "";

    cout << "hash_key: " << hash_key << endl;
    cout << "filelineindex: " << fileLineIndex << endl;
    cout << "starting index: " << startingIndex << endl;

    if (openings.is_open()) {
        for (int lineCounter = 0; lineCounter < startingIndex - 1; lineCounter++) {
            getline(openings, currentHashLine);
        }
        for (int lineCounter = 0; lineCounter < 50 && openingFound == false; lineCounter++) {
            if (getline(openings, currentHashLine)) {
                string stringHash = "";
                for (size_t it = 0; it < currentHashLine.size() && currentHashLine[0] != '_'; it++) {
                    if (currentHashLine[it] == ' ') {
                        if (stoull(stringHash) == hash_key) {
                            openingFound = true;
                            for (size_t it2 = it + 1; it2 < currentHashLine.size() && currentHashLine[it2] != '\n'; it2++) {
                                moves += currentHashLine[it2];
                            }
                        }
                        else {
                            break;
                        }
                        it = currentHashLine.size();;
                    }
                    else {
                        stringHash += currentHashLine[it];
                    }
                }
            }
            else {
                reachedEnd = true;
                break;
            }
        }
    }
    if (!openingFound && reachedEnd) {
        for (int lineCounter = 0; lineCounter < 50; lineCounter++) {
            if (getline(openings, currentHashLine)) {
                string stringHash = "";
                for (size_t it = 0; it < currentHashLine.size() && currentHashLine[0] != '_' && openingFound == false; it++) {
                    if (currentHashLine[it] == ' ') {
                        if (stoull(stringHash) == hash_key) {
                            openingFound = true;
                            for (size_t it2 = it + 1; it2 < currentHashLine.size() && currentHashLine[it2] != '\n'; it2++) {
                                moves += currentHashLine[it2];
                            }
                        }
                        else {
                            break;
                        }
                        lineCounter = 50;
                    }
                    else {
                        stringHash += currentHashLine[it];
                    }
                }
            }
            else {
                reachedEnd = true;
                break;
            }
        }
    }
    openings.close();
    openings2.close();
    if (openingFound) {
        cout << "moves: " << moves << endl;
        moveSelect = obtainMoves(moves);
        return chooseOpeningMove(moveSelect, Board, enPassantCol, whiteMove);
    }
    else {
        return "not found";
    }
}

string openingHash::notationToMove(int board[64], string notation, int enPassantCol, int whiteMove) {
	int color = whiteMove;
	int pieceSelectRow = -1;
	int pieceSelectCol = -1;
	int destinationRow = -1;
	int destinationCol = -1;
	int kingRow;
	int promotion = 0;
	if (color == 1) {
		kingRow = 7;
	}
	else {
		kingRow = 0;
	}

	if (notation == "O-O-O") {
		pieceSelectRow = kingRow;
		pieceSelectCol = 4;
		destinationRow = kingRow;
		destinationCol = 2;
	}
	else if (notation == "O-O") {
		pieceSelectRow = kingRow;
		pieceSelectCol = 4;
		destinationRow = kingRow;
		destinationCol = 6;
	}
	else if (notation.size() == 2) {
		int row = 7 - (notation[1] - '1');
		int col = notation[0] - 'a';
		if (board[(row + color) * 8 + col] == 1 * color) {
			pieceSelectRow = row + color;
			pieceSelectCol = col;
			destinationRow = row;
			destinationCol = col;
		}
		else if (board[(row + 2 * color) * 8 + col] == 1 * color && board[(row + color) * 8 + col] == 0) {
			pieceSelectRow = row + 2 * color;
			pieceSelectCol = col;
			destinationRow = row;
			destinationCol = col;
		}
	}
	else if (notation.size() == 3 && notation[0] >= 'A' && notation[0] <= 'Z') {
		int row = 7 - (notation[2] - '1');
		int col = notation[1] - 'a';
		char piece = notation[0];
		for (int square = 0; square < 64; square++) {
			if ((piece == 'Q' && board[square] == 9 * color) || (piece == 'K' && board[square] == 100 * color) || (piece == 'N' && board[square] == 3 * color) || (piece == 'R' && board[square] == 5 * color) || (piece == 'B' && board[square] == 4 * color)) {
				vector<int> legalmoves;
				oldlegalMoves(board, legalmoves, square, enPassantCol);
				for (size_t it = 0; it < legalmoves.size(); it++) {
					if (legalmoves[it] == row * 8 + col) {
						pieceSelectRow = square / 8;
						pieceSelectCol = square % 8;
						destinationRow = row;
						destinationCol = col;
					}
				}
			}
		}
	}
	else if (notation.size() == 3 && notation[0] >= 'a' && notation[0] <= 'z') {
		int row = 7 - (notation[2] - '1');
		int col = notation[1] - 'a';
		int initialRow = row + color;
		int initialCol = notation[0] - 'a';
		pieceSelectRow = initialRow;
		pieceSelectCol = initialCol;
		destinationRow = row;
		destinationCol = col;
	}
	else if (notation.size() == 4 && notation[0] >= 'A' && notation[0] <= 'Z' && isdigit(notation[1])) {
		int row = 7 - (notation[3] - '1');
		int col = notation[2] - 'a';
		int targetRow = 7 - (notation[1] - '1');
		char piece = notation[0];
		for (int square = 0; square < 64; square++) {
			if (square / 8 == targetRow && ((piece == 'Q' && board[square] == 9 * color) || (piece == 'K' && board[square] == 100 * color) || (piece == 'N' && board[square] == 3 * color) || (piece == 'R' && board[square] == 5 * color) || (piece == 'B' && board[square] == 4 * color))) {
				vector<int> legalmoves;
				oldlegalMoves(board, legalmoves, square, enPassantCol);
				for (size_t it = 0; it < legalmoves.size(); it++) {
					if (legalmoves[it] == row * 8 + col) {
						pieceSelectRow = square / 8;
						pieceSelectCol = square % 8;
						destinationRow = row;
						destinationCol = col;
					}
				}
			}
		}
	}
	else if (notation.size() == 4 && notation[0] >= 'A' && notation[0] <= 'Z') {
		int row = 7 - (notation[3] - '1');
		int col = notation[2] - 'a';
		int targetCol = notation[1] - 'a';
		char piece = notation[0];
		for (int square = 0; square < 64; square++) {
			if (square % 8 == targetCol && ((piece == 'Q' && board[square] == 9 * color) || (piece == 'K' && board[square] == 100 * color) || (piece == 'N' && board[square] == 3 * color) || (piece == 'R' && board[square] == 5 * color) || (piece == 'B' && board[square] == 4 * color))) {
				vector<int> legalmoves;
				oldlegalMoves(board, legalmoves, square, enPassantCol);
				for (size_t it = 0; it < legalmoves.size(); it++) {
					if (legalmoves[it] == row * 8 + col) {
						pieceSelectRow = square / 8;
						pieceSelectCol = square % 8;
						destinationRow = row;
						destinationCol = col;
					}
				}
			}
		}
	}
	else if (notation.size() == 4) {
		int row = 7 - (notation[1] - '1');
		int col = notation[0] - 'a';
		pieceSelectRow = row + color;
		pieceSelectCol = col;
		destinationRow = row;
		destinationCol = col;
		promotion = notation[3] - '0';
	}
	else if (notation.size() == 5) {
		int row = 7 - (notation[2] - '1');
		int col = notation[1] - 'a';
		int initialCol = notation[0] - 'a';
		pieceSelectRow = row + color;
		pieceSelectCol = initialCol;
		destinationRow = row;
		destinationCol = col;
		promotion = notation[4] - '0';
	}
	if (pieceSelectRow == -1 || pieceSelectCol == -1 || destinationRow == -1 || destinationCol == -1) {
		cout << "couldnt convert move" << endl;
		return "error";
	}
	else {
		return to_string(pieceSelectRow) + to_string(pieceSelectCol) + to_string(destinationRow) + to_string(destinationCol) + to_string(promotion);
	}
}

int openingHash::color(int val)
{
    if (val > 0)
    {
        return 1;
    }
    else if (val < 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void openingHash::oldlegalMoves(int Board[64], vector<int>& legalMoves, int selectSquare, int enPassantCol) {
    int row = selectSquare / 8;
    row = row * 8;
    int col = selectSquare % 8;
    int ownColor = color(Board[selectSquare]);

    if (abs(Board[selectSquare]) == 100)
    {
        // enable king to move 1 square out from its square
        if (col < 7) {
            if (selectSquare < 55 && color(Board[selectSquare + 9]) != ownColor) {
                legalMoves.emplace_back(selectSquare + 9);
            }
            if (selectSquare < 63 && color(Board[selectSquare + 1]) != ownColor) {
                legalMoves.emplace_back(selectSquare + 1);
            }
            if (selectSquare > 6 && color(Board[selectSquare - 7]) != ownColor) {
                legalMoves.emplace_back(selectSquare - 7);
            }
        }
        if (col > 0) {
            if (selectSquare > 8 && color(Board[selectSquare - 9]) != ownColor) {
                legalMoves.emplace_back(selectSquare - 9);
            }
            if (selectSquare < 57 && color(Board[selectSquare + 7]) != ownColor) {
                legalMoves.emplace_back(selectSquare + 7);
            }
            if (selectSquare > 0 && color(Board[selectSquare - 1]) != ownColor) {
                legalMoves.emplace_back(selectSquare - 1);
            }
        }
        if (selectSquare < 56 && color(Board[selectSquare + 8]) != ownColor) {
            legalMoves.emplace_back(selectSquare + 8);
        }
        if (selectSquare > 7 && color(Board[selectSquare - 8]) != ownColor) {
            legalMoves.emplace_back(selectSquare - 8);
        }

        //castling

            //white castling
            if (ownColor == 1 && selectSquare == 60)
            {
                //short castle
                if (Board[61] == 0 && Board[62] == 0 && Board[63] == 5)
                {
                    legalMoves.emplace_back(62);
                }
                //long castle
                if (Board[59] == 0 && Board[58] == 0 && Board[57] == 0 && Board[56] == 5)
                {
                    legalMoves.emplace_back(58);
                }
            }
            //black castling
            else if (ownColor == -1 && selectSquare == 4)
            {
                //short castle
                if (Board[5] == 0 && Board[6] == 0 && Board[7] == -5)
                {
                    legalMoves.emplace_back(6);
                }
                //long castle
                if (Board[3] == 0 && Board[2] == 0 && Board[1] == 0 && Board[0] == -5)
                {
                    legalMoves.emplace_back(2);
                }
            }

        return;
    }
    else if (abs(Board[selectSquare]) == 9)
    {
        // rook ---------------------------------
        for (int i = 1; col + i < 8; i++)
        {
            if (color(Board[selectSquare + i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i);
            }
            if (Board[selectSquare + i] != 0)
            {
                break;
            }
        }

        for (int i = 1; col - i > -1; i++)
        {
            if (color(Board[selectSquare - i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i);
            }
            if (Board[selectSquare - i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare + 8; i < 64; i += 8)
        {
            if (color(Board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (Board[i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare - 8; i > -1; i -= 8)
        {
            if (color(Board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (Board[i] != 0)
            {
                break;
            }
        }

        // Bishop ------------------------------------

        for (int i = 1; selectSquare + i * 9 < 64 && col + i < 8; i++)
        {
            if (color(Board[selectSquare + i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 9);
            }
            if (Board[selectSquare + i * 9] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare + i * 7 < 64 && col - i > -1; i++)
        {
            if (color(Board[selectSquare + i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 7);
            }
            if (Board[selectSquare + i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 7 > -1 && col + i < 8; i++)
        {
            if (color(Board[selectSquare - i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 7);
            }
            if (Board[selectSquare - i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 9 > -1 && col - i > -1; i++)
        {
            if (color(Board[selectSquare - i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 9);
            }
            if (Board[selectSquare - i * 9] != 0)
            {
                break;
            }
        }
        return;
    }
    else if (abs(Board[selectSquare]) == 3)
    {
        if (col < 6) {
            if (selectSquare < 54 && ownColor != color(Board[selectSquare + 10]))
            {
                legalMoves.emplace_back(selectSquare + 10);
            }
            if (selectSquare > 5 && ownColor != color(Board[selectSquare - 6]))
            {
                legalMoves.emplace_back(selectSquare - 6);
            }
        }
        if (col < 7) {
            if (selectSquare < 47 && ownColor != color(Board[selectSquare + 17]))
            {
                legalMoves.emplace_back(selectSquare + 17);
            }
            if (selectSquare > 14 && ownColor != color(Board[selectSquare - 15]))
            {
                legalMoves.emplace_back(selectSquare - 15);
            }
        }
        if (col > 1) {
            if (selectSquare < 58 && ownColor != color(Board[selectSquare + 6]))
            {
                legalMoves.emplace_back(selectSquare + 6);
            }
            if (selectSquare > 9 && ownColor != color(Board[selectSquare - 10]))
            {
                legalMoves.emplace_back(selectSquare - 10);
            }
        }
        if (col > 0) {
            if (selectSquare < 49 && ownColor != color(Board[selectSquare + 15]))
            {
                legalMoves.emplace_back(selectSquare + 15);
            }
            if (selectSquare > 16 && ownColor != color(Board[selectSquare - 17]))
            {
                legalMoves.emplace_back(selectSquare - 17);
            }
        }

        return;
    }
    else if (abs(Board[selectSquare]) == 4)
    {
        for (int i = 1; selectSquare + i * 9 < 64 && col + i < 8; i++)
        {
            if (color(Board[selectSquare + i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 9);
            }
            if (Board[selectSquare + i * 9] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare + i * 7 < 64 && col - i > -1; i++)
        {
            if (color(Board[selectSquare + i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 7);
            }
            if (Board[selectSquare + i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 7 > -1 && col + i < 8; i++)
        {
            if (color(Board[selectSquare - i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 7);
            }
            if (Board[selectSquare - i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 9 > -1 && col - i > -1; i++)
        {
            if (color(Board[selectSquare - i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 9);
            }
            if (Board[selectSquare - i * 9] != 0)
            {
                break;
            }
        }
        return;
    }
    else if (abs(Board[selectSquare]) == 1)
    {
        // move forward one square if no piece is there
        if (Board[selectSquare - ownColor * 8] == 0)
        {
            legalMoves.emplace_back(selectSquare - ownColor * 8);

            if (ownColor == 1 && selectSquare > 47 && Board[selectSquare - 16] == 0)
            {
                legalMoves.emplace_back(selectSquare - 16);
            }
            else if (ownColor == -1 && selectSquare < 16 && Board[selectSquare + 16] == 0)
            {
                legalMoves.emplace_back(selectSquare + 16);
            }
        }
        // taking a piece diagonallrow forward to the right
        if (col < 7)
        {
            if (color(Board[selectSquare - ownColor * 8 + 1]) == -ownColor)
            {
                legalMoves.emplace_back(selectSquare - ownColor * 8 + 1);
            }
        }
        // taking a piece diagonallrow forward to the left
        if (col > 0)
        {
            if (color(Board[selectSquare - ownColor * 8 - 1]) == -ownColor)
            {
                legalMoves.emplace_back(selectSquare - ownColor * 8 - 1);
            }
        }
        // en passant to the left of the pawn
        if (col > 0 && Board[selectSquare - ownColor * 8 - 1] == 0 && enPassantCol == col - 1)
        {
            // checking if another pawn is necolt to the pawn at the row en passant
            // can occur for each color
            if (row == 24 && Board[selectSquare - 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 - 1);
            }
            else if (row == 32 && Board[selectSquare - 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 - 1);
            }
        }
        // en passant to the left of the pawn
        if (col < 7 && Board[selectSquare - ownColor * 8 + 1] == 0 && enPassantCol == col + 1)
        {
            // checking if another pawn is necolt to the pawn at the row en passant
            // can occur for each color
            if (row == 24 && Board[selectSquare + 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 + 1);
            }
            else if (row == 32 && Board[selectSquare + 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 + 1);
            }
        }
    }
    else if (abs(Board[selectSquare]) == 5)
    {
        //  enable rook to move on its rank and file
        for (int i = 1; col + i < 8; i++)
        {
            if (color(Board[selectSquare + i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i);
            }
            if (Board[selectSquare + i] != 0)
            {
                break;
            }
        }

        for (int i = 1; col - i > -1; i++)
        {
            if (color(Board[selectSquare - i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i);
            }
            if (Board[selectSquare - i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare + 8; i < 64; i += 8)
        {
            if (color(Board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (Board[i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare - 8; i > -1; i -= 8)
        {
            if (color(Board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (Board[i] != 0)
            {
                break;
            }
        }
        return;
    }
}