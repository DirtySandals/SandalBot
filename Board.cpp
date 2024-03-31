#pragma once
#include "Board.h"

// Constructor accepts FEN string and calls setUp(FEN) and setUpHistory()
//  setUp will initialise the _Board to a 8x8 Piece*** array according to the
//  FEN string setUpHistory() will initialise _BoardHistory as a 8x8x1 dynamic
//  string array.
Board::Board() {
    updateHistory();
}

// default contructor which deletes and frees memory of _Board and _BoardHistory
Board::~Board()
{
    cout << "deleting board" << endl;
    delete player1;
    delete player2;
}

// updateHistory() update _BoardHistory to the latest _Board position
void Board::updateHistory()
{
    string boardPosition = "";
    for (int square = 0; square < 64; square++) {
        boardPosition += to_string(board[square]);
    }
    auto it = boardHistory.find(boardPosition);
    if (it == boardHistory.end()) {
        boardHistory.insert({ boardPosition, 1 });
    }
    else {
        boardHistory[boardPosition]++;
    }
    
    return;
}

// Identifies whether a threefold repetition has occurred. (i.e. the same chess
// position has occurred three times already)
bool Board::isRepitition()
{
    string boardPosition = "";
    for (int square = 0; square < 64; square++) {
        boardPosition += to_string(board[square]);
    }
    auto it = boardHistory.find(boardPosition);
    if (it != boardHistory.end() && boardHistory[boardPosition] > 2) {
        return true;
    }
    return false;
}

// changeBoard function allows a move to occur by changing the board based on
// the move
void Board::changeBoard(unsigned short int selectSquare, unsigned short int targetSquare, unsigned short int promotion)
{   
    int takenpiece = board[targetSquare];
    int movingpiece = board[selectSquare];
    board[targetSquare] = movingpiece;
    board[selectSquare] = 0;
    if (abs(board[targetSquare]) == 1 && abs(selectSquare - targetSquare) == 16) {
        enPassantCol = selectSquare % 8;
    }
    else {
        enPassantCol = -1;
    }
    if (board[targetSquare] == 100) {
        whiteLongCastle = false;
        whiteShortCastle = false;
    }
    else if (board[targetSquare] == -100) {
        blackShortCastle = false;
        blackLongCastle = false;
    }
    if (board[targetSquare] == 5) {
        if (selectSquare > 60) {
            whiteShortCastle = false;
        }
        else {
            whiteLongCastle = false;
        }
    }
    else if (board[targetSquare] == -5) {
        if (selectSquare > 4) {
            blackShortCastle = false;
        }
        else {
            blackLongCastle = false;
        }
    }
    if (promotion == 0)
    {
        // enpassant
        if (abs(movingpiece) == 1 && (selectSquare - targetSquare) % 8 != 0 && takenpiece == 0) {
            board[(selectSquare - selectSquare % 8) + targetSquare % 8] = 0;
        }
        // castling
        else if (abs(movingpiece) == 100 && abs(targetSquare - selectSquare) == 2) {
            // case 2 = short castle, case -2 = long castle
            switch (targetSquare - selectSquare) {
            case 2:
                board[targetSquare - 1] = board[targetSquare + 1];
                board[targetSquare + 1] = 0;
                if (whiteMove == 1) {
                    whiteShortCastle = false;
                }
                else {
                    blackShortCastle = false;
                }
                break;
            case -2:
                board[targetSquare + 1] = board[targetSquare - 2];
                board[targetSquare - 2] = 0;
                if (whiteMove == 1) {
                    whiteLongCastle = false;
                }
                else {
                    blackLongCastle = false;
                }
                break;
            }
        }
    }
    else
    {
        if (promotion == 9) {
            board[targetSquare] = 9 * movingpiece;
        }
        else if (promotion == 4) {
            board[targetSquare] = 4 * movingpiece;
        }
        else if (promotion == 3) {
            board[targetSquare] = 3 * movingpiece;
        }
        else if (promotion == 5) {
            board[targetSquare] = 5 * movingpiece;
        }
    }
    return;
}

// copyboard function creates a deepcopy of the original board
void Board::copyBoard(short int emptyBoard[64])
{
    // nested loop i,j, to index each coordinate on the board
    for (int square = 0; square < 64; square++) {
        emptyBoard[square] = board[square];
    }
    return;
}

// isCheck computes whether you have checked the opponent
bool Board::isCheck()
{
    if (whiteMove == 1) {
        for (int square = 0; square < 64; square++) {
            if (board[square] == -100) {
                if (searchingKingAttack(board, square, -1)) {
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }
    else {
        for (int square = 63; square > -1; square--) {
            if (board[square] == 100) {
                if (searchingKingAttack(board, square, 1)) {
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }

    return 0;
}


bool Board::isCheck(short int board[64]) {
    if (whiteMove == 1) {
        for (int square = 0; square < 64; square++) {
            if (board[square] == -100) {
                if (searchingKingAttack(board, square, -1)) {
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }
    else {
        for (int square = 63; square > -1; square--) {
            if (board[square] == 100) {
                if (searchingKingAttack(board, square, 1)) {
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }

    return 0;
}

bool Board::isIllegalCheck(short int board[64]) {
    if (whiteMove == 1) {
        for (int square = 0; square < 64; square++) {
            if (board[square] == 100) {
                if (searchingKingAttack(board, square, 1)) {
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }
    else {
        for (int square = 63; square > -1; square--) {
            if (board[square] == -100) {
                if (searchingKingAttack(board, square, -1)) {
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }

    return 0;
}

// compute checkmate
bool Board::checkmate()
{
    for (int selectSquare = 0; selectSquare < 64; selectSquare++)
    {
        if (color(board[selectSquare]) == -whiteMove) {
            vector<int> legalmoves;
            // calculate the legal moves for the indexed piece
            legalMoves(legalmoves, selectSquare);
            for (size_t it = 0; it < legalmoves.size(); it++)
            {
                int targetSquare = legalmoves[it];
                short int tempBoard[64];
                copyBoard(tempBoard);
                int takenpiece = tempBoard[targetSquare];
                int movingpiece = tempBoard[selectSquare];
                tempBoard[targetSquare] = movingpiece;
                tempBoard[selectSquare] = 0;
                // enpassant
                if (abs(movingpiece) == 1 && (selectSquare - targetSquare) % 8 != 0 && takenpiece == 0) {
                    board[(selectSquare - selectSquare % 8) + targetSquare % 8] = 0;
                }
                if (!isCheck(tempBoard))
                {
                    return false; 
                }
            }
        }
    }
    return true;
}

// make sure you can't castle through check
bool Board::castleThroughCheck(unsigned short int selectSquare, unsigned short int targetSquare)
{
    if (searchingKingAttack(board, targetSquare, whiteMove)) {
        return true;
    }
    if (whiteMove == 1) {
        // if the selected square on the board is a king and are short castling
        if (targetSquare == 62)
        {
            if (searchingKingAttack(board, 61, 1)) {
                return true;
            }
        }
        // if the selected square on the board is a king and are long castling
        else if (targetSquare == 58)
        {
            if (searchingKingAttack(board, 59, 1)) {
                return true;
            }
        }
    }
    else {
        // if the selected square on the board is a king and are short castling
        if (targetSquare == 6)
        {
            if (searchingKingAttack(board, 5, -1)) {
                return true;
            }
        }
        // if the selected square on the board is a king and are long castling
        else if (targetSquare == 2)
        {
            if (searchingKingAttack(board, 3, -1)) {
                return true;
            }
        }
    }
    return false;
}



// arrToFen() converts a string 8x8 array representing a board to a FEN string
string Board::arrtoFen()
{
    // intialise FEN as an empty string
    string FEN = "";
    // intialise empty counter to 0. this variable will be used to generate
    // integers in the FEN string
    int emptyCounter = 0;
    // nested for lop i,j, to index the _Board
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // if the indexed piece is not an empty square
            if (board[i * 8 + j] != 0)
            {
                // if empty counter > 0, add its integer equavalient to the FEN string,
                // since there is a piece interupting the empty counter
                if (emptyCounter > 0)
                {
                    FEN = FEN + static_cast<char>(emptyCounter + 48);
                    emptyCounter = 0;
                }
                // add the piece's symbol to the FEN string
                FEN = FEN + intToString(board[i * 8 + j]);
            }
            // if the indexed piece is an empty square
            else
            {
                // increment the counter by 1 since it is uninterupted by a piece
                emptyCounter++;
                // if it is the last column, add the emptycounter to the FEN string
                if (j == 7)
                {
                    FEN = FEN + static_cast<char>(emptyCounter + 48);
                    // reset empty counter to zero
                    emptyCounter = 0;
                }
            }
        }
        ////after j loop, add "/" to the FEN string to signify a new row
        if (i != 7)
        {
            FEN = FEN + "/";
        }
    }
    // Once board position is declared onto the FEN string, add whose turn it is
    // based on whiteMove
    if (whiteMove == -1)
    {
        FEN += " w";
    }
    else
    {
        FEN += " b";
    }
    return FEN;
}

bool Board::isStalemate()
{
    for (int selectSquare = 0; selectSquare < 64; selectSquare++)
    {
        if (color(board[selectSquare]) == -whiteMove) {
            vector<int> legalmoves;
            // calculate the legal moves for the indexed piece
            legalMoves(legalmoves, selectSquare);
            for (size_t it = 0; it < legalmoves.size(); it++)
            {
                int targetSquare = legalmoves[it];
                short int tempBoard[64];
                copyBoard(tempBoard);
                int takenpiece = tempBoard[targetSquare];
                int movingpiece = tempBoard[selectSquare];
                tempBoard[targetSquare] = movingpiece;
                tempBoard[selectSquare] = 0;
                // enpassant
                if (abs(movingpiece) == 1 && (selectSquare - targetSquare) % 8 != 0 && takenpiece == 0) {
                    board[(selectSquare - selectSquare % 8) + targetSquare % 8] = 0;
                }
                if (!isIllegalCheck(tempBoard))
                {
                    return false;
                }
            }
        }
    }
    return true;
}


// append moveList with coordinateSelect
void Board::setMoveList(string coordinateSelect)
{
    if (whiteMove == 1) {
        moveList += '.';
    }
    else {
        moveList += '/';
    }
    moveList += coordinateSelect;
}

bool Board::insuffienctMaterial()
{
    // pawn, bishop, rook, knight, queen
    int white[5] = { 0 };
    int black[5] = { 0 };
    for (int square = 0; square < 64; square++)
    {
        if (board[square] == -1)
        {
            black[0] += 1;
        }
        else if (board[square] == -5)
        {
            black[2] += 1;
        }
        else if (board[square] == -4)
        {
            black[1] += 1;
        }
        else if (board[square] == -3)
        {
            black[3] += 1;
        }
        else if (board[square] == -9)
        {
            black[4] += 1;
        }
        else if (board[square] == 1)
        {
            white[0] += 1;
        }
        else if (board[square] == 5)
        {
            white[2] += 1;
        }
        else if (board[square] == 4)
        {
            white[1] += 1;
        }
        else if (board[square] == 3)
        {
            white[3] += 1;
        }
        else if (board[square] == 9)
        {
            white[4] += 1;
        }
    }
    if (white[0] == 0 && white[1] == 0 && white[2] == 0 && white[3] == 0 &&
        white[4] == 0 && black[0] == 0 && black[1] == 0 && black[2] == 0 &&
        black[3] == 0 && black[4] == 0)
    {
        return true;
    }
    else if (white[0] == 0 && white[1] == 1 && white[2] == 0 && white[3] == 0 &&
        white[4] == 0 && black[0] == 0 && black[1] == 0 && black[2] == 0 &&
        black[3] == 0 && black[4] == 0)
    {
        return true;
    }
    else if (white[0] == 0 && white[1] == 0 && white[2] == 0 && white[3] == 0 &&
        white[4] == 0 && black[0] == 0 && black[1] == 1 && black[2] == 0 &&
        black[3] == 0 && black[4] == 0)
    {
        return true;
    }
    else if (white[0] == 0 && white[1] == 0 && white[2] == 0 && white[3] == 1 &&
        white[4] == 0 && black[0] == 0 && black[1] == 0 && black[2] == 0 &&
        black[3] == 0 && black[4] == 0)
    {
        return true;
    }
    else if (white[0] == 0 && white[1] == 0 && white[2] == 0 && white[3] == 0 &&
        white[4] == 0 && black[0] == 0 && black[1] == 0 && black[2] == 0 &&
        black[3] == 1 && black[4] == 0)
    {
        return true;
    }
    else if (white[0] == 0 && white[1] == 1 && white[2] == 0 && white[3] == 0 &&
        white[4] == 0 && black[0] == 0 && black[1] == 1 && black[2] == 0 &&
        black[3] == 0 && black[4] == 0)
    {
        return true;
    }
    else if (white[0] == 0 && white[1] == 0 && white[2] == 0 && white[3] == 1 &&
        white[4] == 0 && black[0] == 0 && black[1] == 0 && black[2] == 0 &&
        black[3] == 1 && black[4] == 0)
    {
        return true;
    }
    return false;
}

void Board::legalMoves(vector<int>& legalMoves, unsigned short int selectSquare)
{
    int row = selectSquare / 8;
    row = row * 8;
    int col = selectSquare % 8;
    int ownColor = color(board[selectSquare]);

    if (abs(board[selectSquare]) == 100)
    {
        // enable king to move 1 square out from its square
        if (col < 7) {
            if (selectSquare < 55 && color(board[selectSquare + 9]) != ownColor && !searchingKingAttack(board, selectSquare + 9, ownColor)) {
                legalMoves.emplace_back(selectSquare + 9);
            }
            if (selectSquare < 63 && color(board[selectSquare + 1]) != ownColor && !searchingKingAttack(board, selectSquare + 1, ownColor)) {
                legalMoves.emplace_back(selectSquare + 1);
            }
            if (selectSquare > 6 && color(board[selectSquare - 7]) != ownColor && !searchingKingAttack(board, selectSquare - 7, ownColor)) {
                legalMoves.emplace_back(selectSquare - 7);
            }
        }
        if (col > 0) {
            if (selectSquare > 8 && color(board[selectSquare - 9]) != ownColor && !searchingKingAttack(board, selectSquare - 9, ownColor)) {
                legalMoves.emplace_back(selectSquare - 9);
            }
            if (selectSquare < 57 && color(board[selectSquare + 7]) != ownColor && !searchingKingAttack(board, selectSquare + 7, ownColor)) {
                legalMoves.emplace_back(selectSquare + 7);
            }
            if (selectSquare > 0 && color(board[selectSquare - 1]) != ownColor && !searchingKingAttack(board, selectSquare - 1, ownColor)) {
                legalMoves.emplace_back(selectSquare - 1);
            }
        }
        if (selectSquare < 56 && color(board[selectSquare + 8]) != ownColor && !searchingKingAttack(board, selectSquare + 8, ownColor)) {
            legalMoves.emplace_back(selectSquare + 8);
        }
        if (selectSquare > 7 && color(board[selectSquare - 8]) != ownColor && !searchingKingAttack(board, selectSquare - 8, ownColor)) {
            legalMoves.emplace_back(selectSquare - 8);
        }

        //castling
        if (!searchingKingAttack(board, selectSquare, ownColor)) {
            //white castling
            if (ownColor == 1 && selectSquare == 60)
            {
                //short castle
                if (whiteShortCastle && board[61] == 0 && board[62] == 0 && board[63] == 5 && !castleThroughCheck(60, 62))
                {
                    legalMoves.emplace_back(62);
                }
                //long castle
                if (whiteLongCastle && board[59] == 0 && board[58] == 0 && board[57] == 0 && board[56] == 5 && !castleThroughCheck(60, 58))
                {
                    legalMoves.emplace_back(58);
                }
            }
            //black castling
            else if (ownColor == -1 && selectSquare == 4)
            {
                //short castle
                if (blackShortCastle && board[5] == 0 && board[6] == 0 && board[7] == -5 && !castleThroughCheck(4, 6))
                {
                    legalMoves.emplace_back(6);
                }
                //long castle
                if (blackLongCastle && board[3] == 0 && board[2] == 0 && board[1] == 0 && board[0] == -5 && !castleThroughCheck(4, 2))
                {
                    legalMoves.emplace_back(2);
                }
            }
        }

        return;
    }
    else if (abs(board[selectSquare]) == 9)
    {
        // rook ---------------------------------
        for (int i = true; col + i < 8; i++)
        {
            if (color(board[selectSquare + i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i);
            }
            if (board[selectSquare + i] != 0)
            {
                break;
            }
        }

        for (int i = true; col - i > -1; i++)
        {
            if (color(board[selectSquare - i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i);
            }
            if (board[selectSquare - i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare + 8; i < 64; i += 8)
        {
            if (color(board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (board[i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare - 8; i > -1; i -= 8)
        {
            if (color(board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (board[i] != 0)
            {
                break;
            }
        }

        // Bishop ------------------------------------

        for (int i = 1; selectSquare + i * 9 < 64 && col + i < 8; i++)
        {
            if (color(board[selectSquare + i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 9);
            }
            if (board[selectSquare + i * 9] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare + i * 7 < 64 && col - i > -1; i++)
        {
            if (color(board[selectSquare + i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 7);
            }
            if (board[selectSquare + i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 7 > -1 && col + i < 8; i++)
        {
            if (color(board[selectSquare - i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 7);
            }
            if (board[selectSquare - i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 9 > -1 && col - i > -1; i++)
        {
            if (color(board[selectSquare - i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 9);
            }
            if (board[selectSquare - i * 9] != 0)
            {
                break;
            }
        }
        return;
    }
    else if (abs(board[selectSquare]) == 3)
    {
        if (col < 6) {
            if (selectSquare < 54 && ownColor != color(board[selectSquare + 10]))
            {
                legalMoves.emplace_back(selectSquare + 10);
            }
            if (selectSquare > 5 && ownColor != color(board[selectSquare - 6]))
            {
                legalMoves.emplace_back(selectSquare - 6);
            }
        }
        if (col < 7) {
            if (selectSquare < 47 && ownColor != color(board[selectSquare + 17]))
            {
                legalMoves.emplace_back(selectSquare + 17);
            }
            if (selectSquare > 14 && ownColor != color(board[selectSquare - 15]))
            {
                legalMoves.emplace_back(selectSquare - 15);
            }
        }
        if (col > 1) {
            if (selectSquare < 58 && ownColor != color(board[selectSquare + 6]))
            {
                legalMoves.emplace_back(selectSquare + 6);
            }
            if (selectSquare > 9 && ownColor != color(board[selectSquare - 10]))
            {
                legalMoves.emplace_back(selectSquare - 10);
            }
        }
        if (col > 0) {
            if (selectSquare < 49 && ownColor != color(board[selectSquare + 15]))
            {
                legalMoves.emplace_back(selectSquare + 15);
            }
            if (selectSquare > 16 && ownColor != color(board[selectSquare - 17]))
            {
                legalMoves.emplace_back(selectSquare - 17);
            }
        }

        return;
    }
    else if (abs(board[selectSquare]) == 4)
    {
        for (int i = 1; selectSquare + i * 9 < 64 && col + i < 8; i++)
        {
            if (color(board[selectSquare + i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 9);
            }
            if (board[selectSquare + i * 9] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare + i * 7 < 64 && col - i > -1; i++)
        {
            if (color(board[selectSquare + i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i * 7);
            }
            if (board[selectSquare + i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 7 > -1 && col + i < 8; i++)
        {
            if (color(board[selectSquare - i * 7]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 7);
            }
            if (board[selectSquare - i * 7] != 0)
            {
                break;
            }
        }

        for (int i = 1; selectSquare - i * 9 > -1 && col - i > -1; i++)
        {
            if (color(board[selectSquare - i * 9]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i * 9);
            }
            if (board[selectSquare - i * 9] != 0)
            {
                break;
            }
        }
        return;
    }
    else if (abs(board[selectSquare]) == 1)
    {
        // move forward one square if no piece is there
        if (board[selectSquare - ownColor * 8] == 0)
        {
            legalMoves.emplace_back(selectSquare - ownColor * 8);

            if (ownColor == 1 && selectSquare > 47 && board[selectSquare - 16] == 0)
            {
                legalMoves.emplace_back(selectSquare - 16);
            }
            else if (ownColor == -1 && selectSquare < 16 && board[selectSquare + 16] == 0)
            {
                legalMoves.emplace_back(selectSquare + 16);
            }
        }
        // taking a piece diagonallrow forward to the right
        if (col < 7)
        {
            if (color(board[selectSquare - ownColor * 8 + 1]) == -ownColor)
            {
                legalMoves.emplace_back(selectSquare - ownColor * 8 + 1);
            }
        }
        // taking a piece diagonallrow forward to the left
        if (col > 0)
        {
            if (color(board[selectSquare - ownColor * 8 - 1]) == -ownColor)
            {
                legalMoves.emplace_back(selectSquare - ownColor * 8 - 1);
            }
        }
        // en passant to the left of the pawn
        if (col > 0 && board[selectSquare - ownColor * 8 - 1] == 0 && enPassantCol == col - 1)
        {
            // checking if another pawn is necolt to the pawn at the row en passant
            // can occur for each color
            if (row == 24 && board[selectSquare - 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 - 1);
            }
            else if (row == 32 && board[selectSquare - 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 - 1);
            }
        }
        // en passant to the left of the pawn
        if (col < 7 && board[selectSquare - ownColor * 8 + 1] == 0 && enPassantCol == col + 1)
        {
            // checking if another pawn is necolt to the pawn at the row en passant
            // can occur for each color
            if (row == 24 && board[selectSquare + 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 + 1);
            }
            else if (row == 32 && board[selectSquare + 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                legalMoves.emplace_back(selectSquare - ownColor * 8 + 1);
            }
        }
    }
    else if (abs(board[selectSquare]) == 5)
    {
        //  enable rook to move on its rank and file
        for (int i = true; col + i < 8; i++)
        {
            if (color(board[selectSquare + i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare + i);
            }
            if (board[selectSquare + i] != 0)
            {
                break;
            }
        }

        for (int i = true; col - i > -1; i++)
        {
            if (color(board[selectSquare - i]) != ownColor)
            {
                legalMoves.emplace_back(selectSquare - i);
            }
            if (board[selectSquare - i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare + 8; i < 64; i += 8)
        {
            if (color(board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (board[i] != 0)
            {
                break;
            }
        }

        for (int i = selectSquare - 8; i > -1; i -= 8)
        {
            if (color(board[i]) != ownColor)
            {
                legalMoves.emplace_back(i);
            }
            if (board[i] != 0)
            {
                break;
            }
        }
        return;
    }
}

string Board::intToString(short int num)
{
    string symbol;
    if (abs(num) == 100)
    {
        symbol = "K";
    }
    else if (abs(num) == 9)
    {
        symbol = "Q";
    }
    else if (abs(num) == 5)
    {
        symbol = "R";
    }
    else if (abs(num) == 4)
    {
        symbol = "B";
    }
    else if (abs(num) == 3)
    {
        symbol = "N";
    }
    else if (abs(num) == 1)
    {
        symbol = "P";
    }
    else if (abs(num) == 0)
    {
        symbol = "-";
    }
    if (num < 0)
    {
        symbol = string(1, tolower(symbol[0]));
    }
    return symbol;
}

int Board::color(short int val)
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

vector<int> Board::getPieces(short int whitemove)
{
    vector<int> pieces;
    for (int square = 0; square < 64; square++)
    {
        if (color(board[square]) == whitemove)
        {
            pieces.emplace_back(square);
        }
    }

    return pieces;
}

void Board::assignPlayer1(char playerIndex, unsigned short int numDepth) {
    if (playerIndex == 'H') {
        player1 = new Human(1);
    }
    else if (playerIndex == 'B') {
        player1 = new Bot(1, numDepth);
    }
    else if (playerIndex == 'O') {
        player1 = new OnlinePlayer(1);
    }
}

void Board::assignPlayer2(char playerIndex, unsigned short int numDepth) {
    if (playerIndex == 'H') {
        player2 = new Human(-1);
    }
    else if (playerIndex == 'B') {
        player2 = new Bot(-1, numDepth);
    }
    else if (playerIndex == 'O') {
        player2 = new OnlinePlayer(-1);
    }
}

bool Board::isType(unsigned short int playerNum, char playerIndex) {
    if (playerNum == 1) {
        if (playerIndex == 'H') {
            if (typeid(*player1) == typeid(Human)) {
                return true;
            }
        }
        else if (playerIndex == 'B') {
            if (typeid(*player1) == typeid(Bot)) {
                return true;
            }
        }
        else if (playerIndex == 'O') {
            if (typeid(*player1) == typeid(OnlinePlayer)) {
                return true;
            }
        }
    }
    else if (playerNum == 2) {
        if (playerIndex == 'H') {
            if (typeid(*player2) == typeid(Human)) {
                return true;
            }
        }
        else if (playerIndex == 'B') {
            if (typeid(*player2) == typeid(Bot)) {
                return true;
            }
        }
        else if (playerIndex == 'O') {
            if (typeid(*player2) == typeid(OnlinePlayer)) {
                return true;
            }
        }
    }

    return false;
}

bool Board::isMove(char playerIndex) {
    if (move % 2 == 1) {
        return isType(1, playerIndex);
    }
    else {
        return isType(2, playerIndex);
    }
}

string Board::makeMove() {
    if (move % 2 == 1) {
        return player1->makeMove(board, enPassantCol);
    }
    else {
        return player2->makeMove(board, enPassantCol);
    }
}


bool Board::searchingKingAttack(short int Board[64], unsigned short int kingSquare, int whiteMove) {
    int col = kingSquare % 8;
    //Bishop ~ Queen
    for (int i = 1; kingSquare + i < 64 && col + i < 8; i++)
    {
        if (Board[kingSquare + i * 9] == -4 * whiteMove || Board[kingSquare + i * 9] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[kingSquare + i * 9] != 0)
        {
            break;
        }
    }

    for (int i = 1; kingSquare + i < 64 && col - i > -1; i++)
    {
        if (Board[kingSquare + i * 7] == -4 * whiteMove || Board[kingSquare + i * 7] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[kingSquare + i * 7] != 0)
        {
            break;
        }
    }

    for (int i = 1; kingSquare - i > -1 && col + i < 8; i++)
    {
        if (Board[kingSquare - i * 7] == -4 * whiteMove || Board[kingSquare - i * 7] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[kingSquare - i * 7] != 0)
        {
            break;
        }
    }

    for (int i = 1; kingSquare - i > -1 && col - i > -1; i++)
    {
        if (Board[kingSquare - i * 9] == -4 * whiteMove || Board[kingSquare - i * 9] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[kingSquare - i * 9] != 0)
        {
            break;
        }
    }

    //Rook ~ Queen
    for (int i = 1; col + i < 8; i++)
    {
        if (Board[kingSquare + i] == -5 * whiteMove || Board[kingSquare + i] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[kingSquare + i] != 0)
        {
            break;
        }
    }

    for (int i = 1; col - i > -1; i++)
    {
        if (Board[kingSquare - i] == -5 * whiteMove || Board[kingSquare - i] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[kingSquare - i] != 0)
        {
            break;
        }
    }

    for (int i = kingSquare + 8; i < 64; i += 8)
    {
        if (Board[i] == -5 * whiteMove || Board[i] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[i] != 0)
        {
            break;
        }
    }

    for (int i = kingSquare - 8; i > -1; i -= 8)
    {
        if (Board[i] == -5 * whiteMove || Board[i] == -9 * whiteMove)
        {
            return true;
        }
        if (Board[i] != 0)
        {
            break;
        }
    }

    //pawn
    if (col > 0 && Board[kingSquare - whiteMove * 8 - 1] == -whiteMove) {
        return true;
    }
    else if (col < 7 && Board[kingSquare - whiteMove * 8 + 1] == -whiteMove) {
        return true;
    }
    //Knight
    if (kingSquare < 54 && col < 6 && Board[kingSquare + 10] == -3 * whiteMove)
    {
        return true;
    }
    else if (kingSquare < 58 && col > 1 && Board[kingSquare + 6] == -3 * whiteMove)
    {
        return true;
    }
    else if (kingSquare > 9 && col > 1 && Board[kingSquare - 10] == -3 * whiteMove)
    {
        return true;
    }
    else if (kingSquare > 5 && col < 6 && Board[kingSquare - 6] == -3 * whiteMove)
    {
        return true;
    }
    else if (kingSquare < 47 && col < 7 && Board[kingSquare + 17] == -3 * whiteMove)
    {
        return true;
    }
    else if (kingSquare < 49 && col > 0 && Board[kingSquare + 15] == -3 * whiteMove)
    {
        return true;
    }
    else if (kingSquare > 14 && col < 7 && Board[kingSquare - 15] == -3 * whiteMove)
    {
        return true;
    }
    else if (kingSquare > 16 && col > 0 && Board[kingSquare - 17] == -3 * whiteMove)
    {
        return true;
    }

    //King
    if (col < 7) {
        if (kingSquare < 55 && Board[kingSquare + 9] == -100 * whiteMove) {
            return true;
        }
        else if (Board[kingSquare + 1] == -100 * whiteMove) {
            return true;
        }
        else if (kingSquare > 6 && Board[kingSquare - 7] == -100 * whiteMove) {
            return true;
        }
    }
    if (col > 0) {
        if (kingSquare < 57 && Board[kingSquare + 7] == -100 * whiteMove) {
            return true;
        }
        else if (Board[kingSquare - 1] == -100 * whiteMove) {
            return true;
        }
        else if (kingSquare > 8 && Board[kingSquare - 9] == -100 * whiteMove) {
            return true;
        }
    }
    if (kingSquare < 56 && Board[kingSquare + 8] == -100 * whiteMove) {
        return true;
    }
    else if (kingSquare > 7 && Board[kingSquare - 8] == -100 * whiteMove) {
        return true;
    }

    return false;
}

void Board::updateBotCastlingRights() {
    if (move % 2 == 1) {
        player1->updateCastlingRights(blackLongCastle, blackShortCastle, whiteShortCastle, whiteLongCastle);
    }
    else {
        player2->updateCastlingRights(blackLongCastle, blackShortCastle, whiteShortCastle, whiteLongCastle);
    }
}