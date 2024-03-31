#include <fstream>
#include <random>
#include <thread>
#include "Board.h"

using namespace std;
using namespace chrono;

extern Board* board;
extern unsigned short int promotion;
extern int pieceSelectRow;
extern int pieceSelectCol;
extern int destinationRow;
extern int destinationCol;
extern bool bookFailed;

inline string moveToNotation(string moveInput) {
    string move;
    int color = board->getwhiteMove();
    int pieceSelectRow = moveInput[0] - '0';
    int pieceSelectCol = moveInput[1] - '0';
    int destinationRow = moveInput[2] - '0';
    int destinationCol = moveInput[3] - '0';
    int piece = abs(board->board[destinationRow * 8 + destinationCol]);

    string usingPiece;
    if (piece == 9) {
        usingPiece = "Q";
    }
    else if (piece == 5) {
        usingPiece = "R";
    }
    else if (piece == 4) {
        usingPiece = "B";
    }
    else if (piece == 3) {
        usingPiece = "N";
    }
    else if (piece == 1) {
        usingPiece = to_string(pieceSelectCol);
    }
    else if (piece == 100) {
        usingPiece = "K";
    }
    if (usingPiece == "K" && pieceSelectCol == 4 && (destinationCol == 6 || destinationCol == 2)) {
        if (destinationCol == 6) {
            move = "O-O";
        }
        else if (destinationCol == 2) {
            move = "O-O-O";
        }
    }
    else if (piece == 1 || promotion != 0) {
        if (destinationRow == 7 || destinationRow == 0) {
            if (destinationCol == pieceSelectCol) {
                move = static_cast<char>(pieceSelectCol + 'a') + to_string(8 - destinationRow) + "=" + to_string(promotion);
            }
            else {
                move = static_cast<char>(pieceSelectCol + 'a');
                move += static_cast<char>(destinationCol + 'a') + to_string(8 - destinationRow) + "=" + to_string(promotion);
            }
        }
        else if (pieceSelectCol == destinationCol) {
            move = static_cast<char>(destinationCol + static_cast<int>('a')) + to_string(8 - destinationRow);
        }
        else {
            move += static_cast<char>(pieceSelectCol + 'a');
            move += static_cast<char>(destinationCol + 'a');
            move += to_string(8 - destinationRow);
        }
    }
    else {
        move = usingPiece + ' ';
        short int tempBoard[64];
        board->copyBoard(tempBoard);
        tempBoard[destinationRow * 8 + destinationCol] = 0;
        tempBoard[pieceSelectRow * 8 + pieceSelectCol] = piece * color;

        for (int selectSquare = 0; selectSquare < 64; selectSquare++) {
            if (tempBoard[selectSquare] == piece * color && !(selectSquare != pieceSelectRow * 8 + pieceSelectCol)) {
                vector<int> legalmoves;
                board->legalMoves(legalmoves, selectSquare);
                for (size_t it = 0; it < legalmoves.size(); it++) {
                    if (legalmoves[it] == destinationRow * 8 + destinationCol) {
                        short int tempBoard2[64];
                        board->copyBoard(tempBoard2);
                        tempBoard[destinationRow * 8 + destinationCol] = tempBoard[selectSquare];
                        tempBoard[pieceSelectRow * 8 + pieceSelectCol] = 0;
                        if (!board->isIllegalCheck(tempBoard2)) {
                            if (pieceSelectCol == selectSquare % 8) {
                                move.erase(1, 1);
                                move += to_string(8 - pieceSelectRow);
                            }
                            else if (pieceSelectRow == selectSquare / 8) {
                                move[1] = static_cast<char>(pieceSelectCol + 'a');
                            }
                            else {
                                move[1] = static_cast<char>(pieceSelectCol + 'a');
                            }
                        }
                    }
                }
            }
        }
        if (move[1] == ' ') {
            move.erase(1, 1);
        }
        move += static_cast<char>(destinationCol + 'a') + to_string(8 - destinationRow);

    }
    cout << "move: " << move << endl;
    return move;
}



inline bool findSubString(string line, string subString) {
    if (subString.length() > line.length()) {
        return false;
    }

    for (size_t i = 0; i < subString.length(); i++) {
        if (subString[i] != line[i]) {
            return false;
        }
    }
    return true;
}

inline void writeMoves() {
    ofstream file("GameResult.txt");

    // open file for editing
    if (file.is_open())
    {
        // write the original FEN position of the board to the first line of the
        // .txt file
        file << "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w\n";
        // write the moveList onto the second line of the .txt file. This describes
        // all the moves in the game
        file << board->getMoveList();
        // convert the last position on the board to a FEN string and write it to
        // the third line of the .txt file
        file << "\n" + board->arrtoFen();
        // Close the file
        file.close();
    }
}

//old opening function, very slow due to txt file
/*
inline void chooseOpening() {
    random_device rd;
    mt19937 gen(rd());
    auto startTime = high_resolution_clock::now();
    string moveList = board->getMoveList();
    ifstream file("openings.txt");
    string line;
    bool openingFound = false;
    // open file for editing
    if (file.is_open())
    {
        int fileSize = 0;
        while (getline(file, line)) {
            fileSize++;
        }
        uniform_int_distribution<int> distribution(1, fileSize);
        streampos randomLine = distribution(gen);
        file.clear();
        file.seekg(0, ios::beg);
        for (int i = 0; i < randomLine && getline(file, line); i++) {

        }
        getline(file, line);
        do {
            if (findSubString(line, moveList)) {
                line.erase(0, board->getMoveList().length());
                openingFound = true;
                break;
            }
        } while (getline(file, line));

        if (!openingFound) {
            file.clear();
            file.seekg(0, ios::beg);
            int currentline = 0;
            do {
                currentline++;
                getline(file, line);
                if (findSubString(line, moveList)) {
                    line.erase(0, board->getMoveList().length());
                    openingFound = true;
                    break;
                }
            } while (currentline != randomLine);
        }

        file.clear();
        file.seekg(0, ios::beg);
        if (!openingFound || line.empty()) {
            cout << "Out of book." << endl;
            cout << "making own move" << endl;
            bookFailed = true;
        }
        else {
            if (line[0] == '.') {
                line.erase(0, 1);
            }
            if (line[0] == '/') {
                line.erase(0, 1);
            }
            size_t pos;
            if (board->getwhiteMove() == 1) {
                pos = line.find('/');
            }
            else {
                pos = line.find('.');
            }
            if (pos != string::npos) {
                line.erase(pos);
            }
            notationToMove(line);
        }
        // Close the file
        file.close();
    }
    else {
        printf("Opening File Did Not Open");
    }
    auto endTime = high_resolution_clock::now();
    duration<double> duration = endTime - startTime;
    cout << "Runtime for opening: " << duration.count() << endl;
    if (duration.count() < 0.2) {
        this_thread::sleep_for(chrono::milliseconds(200 - int(duration.count() * 1000)));
    }
    return;
}
*/