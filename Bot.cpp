#include "Bot.h"

Bot::Bot(short int whiteMove, unsigned short int numDepth) : Player(whiteMove) {
    this->numDepth = numDepth;
    initEnPassant();
    initPieceHashes();
}
/*
hash = hashBoard(tempBoard, 1, passantcol);
int potentialEval = tTable.lookupEvaluation(tempBoard, -1, passantcol, depth, hash);
if (potentialEval != 999999) {
    score = potentialEval;
    nodes++;
    transpositions++;
}
else {
    score = miniMax(tempBoard, depth + 1, numDepth, false, passantcol, whitePieces, blackPieces, alpha, beta);
    tTable.storeEvaluation(tempBoard, 1 - 2 * isMaximising, passantcol, depth, score, hash);
}
*/
int Bot::eval(int board[64], bool isMaximising, int enPassantCol, vector<int>& ownPieces, vector<int>& oppositePieces) {
    int eval = 0;
    int friendlyKingSquare = -1;
    int opposingKingSquare = -1;
    if (isCheck_mm(board, 1 - 2 * isMaximising)) {
        if (!isMaximising && checkmate_mm(board, 1, ownPieces, enPassantCol))
        {
            return 100000;
        }
        else if (isMaximising && checkmate_mm(board, -1, ownPieces, enPassantCol))
        {
            return -100000;
        }
    }
    if (isMaximising) {
        for (size_t it = 0; it < ownPieces.size(); it++) {
            if (ownPieces[it] == -1) {
                continue;
            }
            switch (board[ownPieces[it]])
            {
            case 1:
                eval += 100 + whitepawneval[ownPieces[it]];
                break;
            case 5:
                eval += 500 + whiterookeval[ownPieces[it]];
                break;
            case 100:
                eval += whitekingeval[ownPieces[it]];
                friendlyKingSquare = ownPieces[it];
                break;
            case 4:
                eval += 330 + whitebishopeval[ownPieces[it]];
                break;
            case 3:
                eval += 320 + knighteval[ownPieces[it]];
                break;
            case 9:
                eval += 900 + whitequeeneval[ownPieces[it]];
                break;
            }
        }

        for (size_t it = 0; it < oppositePieces.size(); it++) {
            if (oppositePieces[it] == -1) {
                continue;
            }
            switch (board[oppositePieces[it]]) {
            case -1:
                eval += -100 - blackpawneval[oppositePieces[it]];
                break;
            case -5:
                eval += -500 - blackrookeval[oppositePieces[it]];
                break;
            case -100:
                eval += blackkingeval[oppositePieces[it]];
                opposingKingSquare = oppositePieces[it];
                break;
            case -4:
                eval += -330 - blackbishopeval[oppositePieces[it]];
                break;
            case -3:
                eval += -320 - knighteval[oppositePieces[it]];
                break;
            case -9:
                eval += -900 - blackqueeneval[oppositePieces[it]];
                break;
            }
        }
    }
    else {
        for (size_t it = 0; it < oppositePieces.size(); it++) {
            if (oppositePieces[it] == -1) {
                continue;
            }
            switch (board[oppositePieces[it]])
            {
            case 1:
                eval += 100 + whitepawneval[oppositePieces[it]];
                break;
            case 5:
                eval += 500 + whiterookeval[oppositePieces[it]];
                break;
            case 100:
                eval += whitekingeval[oppositePieces[it]];
                opposingKingSquare = oppositePieces[it];
                break;
            case 4:
                eval += 330 + whitebishopeval[oppositePieces[it]];
                break;
            case 3:
                eval += 320 + knighteval[oppositePieces[it]];
                break;
            case 9:
                eval += 900 + whitequeeneval[oppositePieces[it]];
                break;
            }
        }

        for (size_t it = 0; it < ownPieces.size(); it++) {
            if (ownPieces[it] == -1) {
                continue;
            }
            switch (board[ownPieces[it]]) {
            case -1:
                eval += -100 - blackpawneval[ownPieces[it]];
                break;
            case -5:
                eval += -500 - blackrookeval[ownPieces[it]];
                break;
            case -100:
                eval += blackkingeval[ownPieces[it]];
                friendlyKingSquare = ownPieces[it];
                break;
            case -4:
                eval += -330 - blackbishopeval[ownPieces[it]];
                break;
            case -3:
                eval += -320 - knighteval[ownPieces[it]];
                break;
            case -9:
                eval += -900 - blackqueeneval[ownPieces[it]];
                break;
            }
        }
    }

    //promote getting king closer, moreso in endgames
    eval += int(ForceKingToCornerEndgameEval(friendlyKingSquare, opposingKingSquare) * min(int(displayMove), 40));

    if (castlingAbility[0] && board[2] == -100 && board[3] == -5) {
        eval += min(int(-600 + blackCastled * 50), 0);
    }
    else if (castlingAbility[1] && board[6] == -100 && board[5] == -5) {
        eval += min(int(-600 + blackCastled * 50), 0);
    }

    if (castlingAbility[3] && board[58] == 100 && board[59] == 5) {
        eval += max(int(600 - whiteCastled * 50), 0);
    }
    else if (castlingAbility[2] && board[62] == 100 && board[61] == 5) {
        eval += max(int(600 - whiteCastled * 50), 0);
    }
    return eval;
}
int Bot::miniMax(int board[64], int depth, bool isMaximising, int enPassantCol, vector<int>& ownPieces, vector<int>& oppositePieces, int alpha, int beta)
{
    nodes++;
    // end condition (checkmate , stalemate, draw, depth reached)
    if (depth == numDepth)
    {
        return eval(board, isMaximising, enPassantCol, ownPieces, oppositePieces);
    }

    bool isMove = false;
    
    // is maximising
    if (isMaximising)
    {
        for (size_t it = 0; it < ownPieces.size(); it++)
        {
            if (ownPieces[it] != -1 && legalMoves_mm(board, ownPieces[it], enPassantCol, true, depth, ownPieces, oppositePieces, it, alpha, beta)) {
                isMove = true;
            }
        }
        if (!isMove)
        {
            nodes++;
            if (isCheck_mm(board, -1))
            {
                return -100000 * abs(numDepth + 1 - depth);
            }
            else
            {
                return 0;
            }
        }
        return alpha;
    }
    else
    {
        for (size_t it = 0; it < ownPieces.size(); it++)
        {
            if (ownPieces[it] != -1 && legalMoves_mm(board, ownPieces[it], enPassantCol, false, depth, ownPieces, oppositePieces, it, alpha, beta)) {
                isMove = true;
            }
            if (alpha >= beta)
            {
                return alpha;
            }           
        }
        if (!isMove)
        {
            nodes++;
            if (isCheck_mm(board, 1))
            {
                return 100000 * abs(numDepth + 1 - depth);
            }
            else
            {
                return 0;
            }
        }
        return beta;
    }
}

bool Bot::isCheck_mm(int Board[64], int whiteMove) {
    if (whiteMove == 1) {
        for (int square = 0; square < 64; square++) {
            if (Board[square] == -100) {
                if (searchingKingAttack(Board, square, -1)) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
    }
    else {
        for (int square = 63; square > -1; square--) {
            if (Board[square] == 100) {
                if (searchingKingAttack(Board, square, 1)) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
    }

    return false;
}
bool Bot::checkmate_mm(int Board[64], int whitemove, vector<int>& ownPieces, int enPassantCol) {
    int tempBoard[64];
    int selectSquare;
    int targetSquare;
    for (size_t it = 0; it < ownPieces.size(); it++)
    {
        if (ownPieces[it] != -1) {
            selectSquare = ownPieces[it];
            vector<int> legalMoves;
            // calculate the legal moves for the indexed piece
            oldlegalMoves(Board, legalMoves, ownPieces[it], enPassantCol);
            for (size_t it2 = 0; it2 < legalMoves.size(); it2++)
            {
                targetSquare = legalMoves[it2];
                for (int v = 0; v < 64; v++) {
                    tempBoard[v] = Board[v];
                }
                // enact the move
                changeBoard_mm(tempBoard, selectSquare, targetSquare, 0);
                // call isCheck function on the temporary board to see if the
                // other color avoided checkmate
                if (!isCheck_mm(tempBoard, whitemove)) {
                    return false;
                }
            }
        }
    }
    // return whitemove, (i.e. if its white's move, white will win)
    return true;
}

// make sure you can't castle through check
bool Bot::castleThroughCheck_mm(int Board[64], int destinationSquare, int whitemove)
{
    if (searchingKingAttack(Board, destinationSquare, whitemove)) {
        return true;
    }
    if (whitemove == 1) {
        // if the selected square on the board is a king and are short castling
        if (destinationSquare == 62)
        {
            if (searchingKingAttack(Board, 61, 1)) {
                return true;
            }
        }
        // if the selected square on the board is a king and are long castling
        else if (destinationSquare == 58)
        {
            if (searchingKingAttack(Board, 59, 1)) {
                return true;
            }
        }
    }
    else {
        // if the selected square on the board is a king and are short castling
        if (destinationSquare == 6)
        {
            if (searchingKingAttack(Board, 5, -1)) {
                return true;
            }
        }
        // if the selected square on the board is a king and are long castling
        else if (destinationSquare == 2)
        {
            if (searchingKingAttack(Board, 3, -1)) {
                return true;
            }
        }
    }
    return false;
}

bool Bot::searchingKingAttack(int Board[64], int kingSquare, int whiteMove) {
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
int Bot::deletePiece(int square, vector<int>& pieces) {
    for (int i = 0; i < pieces.size(); i++) {
        if (pieces[i] == square) {
            pieces[i] = -1;
            return i;
        }
    }
    return -1;
}
void Bot::alterBoard(int board[64], const int selectSquare, const int targetSquare, vector<int>& oppositePieces, vector<int>& ownPieces, int& deleteIndex, int& takenPiece, const int selectIndex) {
    takenPiece = board[targetSquare];
    ownPieces[selectIndex] = targetSquare;
    board[targetSquare] = board[selectSquare];
    board[selectSquare] = 0;
    if (takenPiece != 0) {
        deleteIndex = deletePiece(targetSquare, oppositePieces);
    }
}

void Bot::undoMove(int board[64], int selectSquare, int targetSquare, vector<int>& oppositePieces, vector<int>& ownPieces, int deleteIndex, int takenPiece, int movingPiece, int selectIndex) {
    ownPieces[selectIndex] = selectSquare;
    board[selectSquare] = movingPiece;
    board[targetSquare] = takenPiece;
    if (takenPiece != 0) {
        oppositePieces[deleteIndex] = targetSquare;
    }
}

bool Bot::legalMoves_mm(int Board[64], const int selectSquare, const int enPassantCol, const bool isMaximising, const int depth, vector<int>& ownPieces, vector<int>& oppositePieces, const int selectIndex, int& alpha, int& beta)
{
    int row = selectSquare / 8;
    row = row * 8;
    const int col = selectSquare % 8;
    const int ownColor = color(Board[selectSquare]);
    int takenPiece;
    const int movingPiece = Board[selectSquare];
    int deleteIndex;
    bool isMove = false;

    if (abs(Board[selectSquare]) == 100)
    {
        // enable king to move 1 square out from its square
        if (col < 7) {
            if (selectSquare < 55 && color(Board[selectSquare + 9]) != ownColor) {
                alterBoard(Board, selectSquare, selectSquare + 9, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!searchingKingAttack(Board, selectSquare + 9, ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 9, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 9, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare < 63 && color(Board[selectSquare + 1]) != ownColor) {
                alterBoard(Board, selectSquare, selectSquare + 1, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!searchingKingAttack(Board, selectSquare + 1, ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 1, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 1, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare > 6 && color(Board[selectSquare - 7]) != ownColor) {
                alterBoard(Board, selectSquare, selectSquare - 7, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!searchingKingAttack(Board, selectSquare - 7, ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 7, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 7, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
        }
        if (col > 0) {
            if (selectSquare > 8 && color(Board[selectSquare - 9]) != ownColor) {
                alterBoard(Board, selectSquare, selectSquare - 9, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!searchingKingAttack(Board, selectSquare - 9, ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 9, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 9, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare < 57 && color(Board[selectSquare + 7]) != ownColor) {
                alterBoard(Board, selectSquare, selectSquare + 7, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!searchingKingAttack(Board, selectSquare + 7, ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 7, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 7, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare > 0 && color(Board[selectSquare - 1]) != ownColor) {
                alterBoard(Board, selectSquare, selectSquare - 1, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!searchingKingAttack(Board, selectSquare - 1, ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 1, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 1, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
        }
        if (selectSquare < 56 && color(Board[selectSquare + 8]) != ownColor) {
            alterBoard(Board, selectSquare, selectSquare + 8, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
            if (!searchingKingAttack(Board, selectSquare + 8, ownColor)) {
                continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 8, 0);
                isMove = true;
            }
            undoMove(Board, selectSquare, selectSquare + 8, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
            if (alpha >= beta) {
                return true;
            }
        }
        if (selectSquare > 7 && color(Board[selectSquare - 8]) != ownColor) {
            alterBoard(Board, selectSquare, selectSquare - 8, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
            if (!searchingKingAttack(Board, selectSquare - 8, ownColor)) {
                continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 8, 0);
                isMove = true;
            }
            undoMove(Board, selectSquare, selectSquare - 8, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
            if (alpha >= beta) {
                return true;
            }
        }

        //castling
        if (!searchingKingAttack(Board, selectSquare, ownColor)) {
            //white castling
            if (ownColor == 1 && selectSquare == 60)
            {
                //short castle
                if (castlingAbility[2] && Board[61] == 0 && Board[62] == 0 && Board[63] == 5 && !castleThroughCheck_mm(Board, 62, 1))
                {
                    Board[62] = 100;
                    Board[60] = 0;
                    Board[61] = 5;
                    Board[63] = 0;
                    ownPieces[selectIndex] = 62;
                    int castlingRook = deletePiece(63, ownPieces);
                    ownPieces[castlingRook] = 61;
                    whiteCastled = depth;
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, 60, 62, 0);
                    isMove = true;
                    Board[62] = 0;
                    Board[60] = 100;
                    Board[61] = 0;
                    Board[63] = 5;
                    ownPieces[castlingRook] = 63;
                    ownPieces[selectIndex] = 60;
                    if (alpha >= beta) {
                        return true;
                    }
                }
                //long castle
                if (castlingAbility[3] && Board[59] == 0 && Board[58] == 0 && Board[57] == 0 && Board[56] == 5 && !castleThroughCheck_mm(Board, 58, 1))
                {
                    Board[58] = 100;
                    Board[60] = 0;
                    Board[59] = 5;
                    Board[56] = 0;
                    ownPieces[selectIndex] = 58;
                    int castlingRook = deletePiece(56, ownPieces);
                    ownPieces[castlingRook] = 59;
                    whiteCastled = depth;
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, 60, 58, 0);
                    isMove = true;
                    Board[58] = 0;
                    Board[60] = 100;
                    Board[59] = 0;
                    Board[56] = 5;
                    ownPieces[castlingRook] = 56;
                    ownPieces[selectIndex] = 60;
                    if (alpha >= beta) {
                        return true;
                    }
                }
            }
            //black castling
            else if (ownColor == -1 && selectSquare == 4 && !castleThroughCheck_mm(Board, 4, -1))
            {
                //short castle
                if (castlingAbility[1] && Board[5] == 0 && Board[6] == 0 && Board[7] == -5 && !castleThroughCheck_mm(Board, 6, -1))
                {
                    Board[6] = -100;
                    Board[4] = 0;
                    Board[5] = -5;
                    Board[7] = 0;
                    ownPieces[selectIndex] = 6;
                    int castlingRook = deletePiece(7, ownPieces);
                    ownPieces[castlingRook] = 5;
                    blackCastled = depth;
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, 4, 6, 0);
                    isMove = true;
                    Board[6] = 0;
                    Board[4] = -100;
                    Board[5] = 0;
                    Board[7] = -5;
                    ownPieces[castlingRook] = 7;
                    ownPieces[selectIndex] = 4;
                    if (alpha >= beta) {
                        return true;
                    }
                }
                //long castle
                if (castlingAbility[0] && Board[3] == 0 && Board[2] == 0 && Board[1] == 0 && Board[0] == -5 && !castleThroughCheck_mm(Board, 2, -1))
                {
                    Board[2] = -100;
                    Board[4] = 0;
                    Board[3] = -5;
                    Board[0] = 0;
                    ownPieces[selectIndex] = 2;
                    int castlingRook = deletePiece(0, ownPieces);
                    ownPieces[castlingRook] = 3;
                    blackCastled = depth;
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, 4, 2, 0);
                    isMove = true;
                    Board[2] = 0;
                    Board[4] = -100;
                    Board[3] = 0;
                    Board[0] = -5;
                    ownPieces[castlingRook] = 0;
                    ownPieces[selectIndex] = 4;
                    if (alpha >= beta) {
                        return true;
                    }
                }
            }
        }
    }
    else if (abs(Board[selectSquare]) == 9)
    {
        // rook ---------------------------------
        for (int i = 1; col + i < 8; i++)
        {
            if (color(Board[selectSquare + i]) != ownColor)
            {
                alterBoard(Board, selectSquare, selectSquare + i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare - i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare + i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + i * 9, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare + i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + i * 7, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare - i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - i * 7, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare - i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - i * 9, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (Board[selectSquare - i * 9] != 0)
            {
                break;
            }
        }
    }
    else if (abs(Board[selectSquare]) == 3)
    {
        if (col < 6) {
            if (selectSquare < 54 && ownColor != color(Board[selectSquare + 10]))
            {
                alterBoard(Board, selectSquare, selectSquare + 10, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 10, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 10, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare > 5 && ownColor != color(Board[selectSquare - 6]))
            {
                alterBoard(Board, selectSquare, selectSquare - 6, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 6, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 6, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
        }
        if (col < 7) {
            if (selectSquare < 47 && ownColor != color(Board[selectSquare + 17]))
            {
                alterBoard(Board, selectSquare, selectSquare + 17, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 17, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 17, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare > 14 && ownColor != color(Board[selectSquare - 15]))
            {
                alterBoard(Board, selectSquare, selectSquare - 15, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 15, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 15, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
        }
        if (col > 1) {
            if (selectSquare < 58 && ownColor != color(Board[selectSquare + 6]))
            {
                alterBoard(Board, selectSquare, selectSquare + 6, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 6, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 6, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare > 9 && ownColor != color(Board[selectSquare - 10]))
            {
                alterBoard(Board, selectSquare, selectSquare - 10, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 10, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 10, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
        }
        if (col > 0) {
            if (selectSquare < 49 && ownColor != color(Board[selectSquare + 15]))
            {
                alterBoard(Board, selectSquare, selectSquare + 15, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 15, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 15, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (selectSquare > 16 && ownColor != color(Board[selectSquare - 17]))
            {
                alterBoard(Board, selectSquare, selectSquare - 17, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 17, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 17, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
        }
    }
    else if (abs(Board[selectSquare]) == 4)
    {
        for (int i = 1; selectSquare + i * 9 < 64 && col + i < 8; i++)
        {
            if (color(Board[selectSquare + i * 9]) != ownColor)
            {
                alterBoard(Board, selectSquare, selectSquare + i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + i * 9, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare + i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + i * 7, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare - i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - i * 7, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - i * 7, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare - i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - i * 9, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - i * 9, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (Board[selectSquare - i * 9] != 0)
            {
                break;
            }
        }
    }
    else if (abs(Board[selectSquare]) == 1)
    {
        // move forward one square if no piece is there
        if (Board[selectSquare - ownColor * 8] == 0)
        {
            if ((ownColor == 1 && selectSquare < 16) || (ownColor == -1 && selectSquare > 47)) {
                takenPiece = 0;
                int promotions[4] = {9 * ownColor, 5 * ownColor, 4 * ownColor, 3 * ownColor};
                for (int p = 0; p < 4; p++) {
                    ownPieces[selectIndex] = selectSquare - ownColor * 8;
                    Board[selectSquare - ownColor * 8] = promotions[p];
                    Board[selectSquare] = 0;
                    takenPiece = 0;
                    if (!isCheck_mm(Board, -ownColor)) {
                        continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8, abs(promotions[p]));
                        isMove = true;
                    }
                    undoMove(Board, selectSquare, selectSquare - ownColor * 8, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                    if (alpha >= beta) {
                        return true;
                    }
                }
            }
            else {
                alterBoard(Board, selectSquare, selectSquare - ownColor * 8, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - ownColor * 8, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }

            if (ownColor == 1 && selectSquare > 47 && Board[selectSquare - 16] == 0)
            {
                alterBoard(Board, selectSquare, selectSquare - 16, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, col, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - 16, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - 16, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            else if (ownColor == -1 && selectSquare < 16 && Board[selectSquare + 16] == 0)
            {
                alterBoard(Board, selectSquare, selectSquare + 16, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, col, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + 16, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + 16, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
        }
        // taking a piece diagonallrow forward to the right
        if (col < 7)
        {
            if (color(Board[selectSquare - ownColor * 8 + 1]) == -ownColor)
            {
                if ((ownColor == 1 && selectSquare < 16) || (ownColor == -1 && selectSquare > 47)) {
                    int promotions[4] = { 9 * ownColor, 5 * ownColor, 4 * ownColor, 3 * ownColor };
                    for (int p = 0; p < 4; p++) {
                        takenPiece = Board[selectSquare - ownColor * 8 + 1];
                        ownPieces[selectIndex] = selectSquare - ownColor * 8 + 1;
                        Board[selectSquare - ownColor * 8 + 1] = promotions[p];
                        Board[selectSquare] = 0;
                        deleteIndex = deletePiece(selectSquare - ownColor * 8 + 1, oppositePieces);
                        if (!isCheck_mm(Board, -ownColor)) {
                            continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 + 1, abs(promotions[p]));
                            isMove = true;
                        }
                        undoMove(Board, selectSquare, selectSquare - ownColor * 8 + 1, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                        if (alpha >= beta) {
                            return true;
                        }
                    }
                }
                else {
                    alterBoard(Board, selectSquare, selectSquare - ownColor * 8 + 1, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                    if (!isCheck_mm(Board, -ownColor)) {
                        continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 + 1, 0);
                        isMove = true;
                    }
                    undoMove(Board, selectSquare, selectSquare - ownColor * 8 + 1, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                    if (alpha >= beta) {
                        return true;
                    }
                }
            }
        }
        // taking a piece diagonallrow forward to the left
        if (col > 0)
        {
            if (color(Board[selectSquare - ownColor * 8 - 1]) == -ownColor)
            {
                if ((ownColor == 1 && selectSquare < 16) || (ownColor == -1 && selectSquare > 47)) {
                    int promotions[4] = { 9 * ownColor, 5 * ownColor, 4 * ownColor, 3 * ownColor };
                    for (int p = 0; p < 4; p++) {
                        takenPiece = Board[selectSquare - ownColor * 8 - 1];
                        ownPieces[selectIndex] = selectSquare - ownColor * 8 - 1;
                        Board[selectSquare - ownColor * 8 - 1] = promotions[p];
                        Board[selectSquare] = 0;
                        deleteIndex = deletePiece(selectSquare - ownColor * 8 - 1, oppositePieces);
                        if (!isCheck_mm(Board, -ownColor)) {
                            continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 - 1, abs(promotions[p]));
                            isMove = true;
                        }
                        undoMove(Board, selectSquare, selectSquare - ownColor * 8 - 1, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                        if (alpha >= beta) {
                            return true;
                        }
                    }
                }
                else {
                    alterBoard(Board, selectSquare, selectSquare - ownColor * 8 - 1, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                    if (!isCheck_mm(Board, -ownColor)) {
                        continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 - 1, 0);
                        isMove = true;
                    }
                    undoMove(Board, selectSquare, selectSquare - ownColor * 8 - 1, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                    if (alpha >= beta) {
                        return true;
                    }
                }
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
                Board[selectSquare - ownColor * 8 - 1] = 1;
                Board[selectSquare - 1] = 0;
                Board[selectSquare] = 0;
                ownPieces[selectIndex] = selectSquare - ownColor * 8 - 1;
                deleteIndex = deletePiece(selectSquare - 1, oppositePieces);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 - 1, 0);
                    isMove = true;
                }
                Board[selectSquare - ownColor * 8 - 1] = 0;
                Board[selectSquare - 1] = -1;
                Board[selectSquare] = 1;
                oppositePieces[deleteIndex] = selectSquare - 1;
                ownPieces[selectIndex] = selectSquare;
                if (alpha >= beta) {
                    return true;
                }
            }
            else if (row == 32 && Board[selectSquare - 1] == -ownColor)
            {
                Board[selectSquare - ownColor * 8 - 1] = -1;
                Board[selectSquare - 1] = 0;
                Board[selectSquare] = 0;
                ownPieces[selectIndex] = selectSquare - ownColor * 8 - 1;
                deleteIndex = deletePiece(selectSquare - 1, oppositePieces);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 - 1, 0);
                    isMove = true;
                }
                Board[selectSquare - ownColor * 8 - 1] = 0;
                Board[selectSquare - 1] = 1;
                Board[selectSquare] = -1;
                oppositePieces[deleteIndex] = selectSquare - 1;
                ownPieces[selectIndex] = selectSquare;
                if (alpha >= beta) {
                    return true;
                }
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
                Board[selectSquare - ownColor * 8 + 1] = 1;
                Board[selectSquare + 1] = 0;
                Board[selectSquare] = 0;
                ownPieces[selectIndex] = selectSquare - ownColor * 8 + 1;
                deleteIndex = deletePiece(selectSquare + 1, oppositePieces);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 + 1, 0);
                    isMove = true;
                }
                Board[selectSquare - ownColor * 8 + 1] = 0;
                Board[selectSquare + 1] = -1;
                Board[selectSquare] = 1;
                oppositePieces[deleteIndex] = selectSquare + 1;
                ownPieces[selectIndex] = selectSquare;
                if (alpha >= beta) {
                    return true;
                }
            }
            else if (row == 32 && Board[selectSquare + 1] == -ownColor)
            {
                // For en passant onlrow, a 2 is labelled for a move since there are
                // other ecolternal factors for this move
                Board[selectSquare - ownColor * 8 + 1] = -1;
                Board[selectSquare + 1] = 0;
                Board[selectSquare] = 0;
                ownPieces[selectIndex] = selectSquare - ownColor * 8 + 1;
                deleteIndex = deletePiece(selectSquare + 1, oppositePieces);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - ownColor * 8 + 1, 0);
                    isMove = true;
                }
                Board[selectSquare - ownColor * 8 + 1] = 0;
                Board[selectSquare + 1] = 1;
                Board[selectSquare] = -1;
                oppositePieces[deleteIndex] = selectSquare + 1;
                ownPieces[selectIndex] = selectSquare;
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare + i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare + i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare + i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, selectSquare - i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, selectSquare - i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, selectSquare - i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
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
                alterBoard(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, selectIndex);
                if (!isCheck_mm(Board, -ownColor)) {
                    continueSearch(Board, isMaximising, depth, -1, ownPieces, oppositePieces, alpha, beta, selectSquare, i, 0);
                    isMove = true;
                }
                undoMove(Board, selectSquare, i, oppositePieces, ownPieces, deleteIndex, takenPiece, movingPiece, selectIndex);
                if (alpha >= beta) {
                    return true;
                }
            }
            if (Board[i] != 0)
            {
                break;
            }
        }
    }
    return isMove;
}

void Bot::continueSearch(int board[64], bool isMaximising, int depth, int enPassantCol, vector<int>& ownPieces, vector<int>& oppositePieces, int& alpha, int& beta, int selectSquare, int targetSquare, int promotion) {
    /*
    ulong hash_key = hashBoard(board, -1 + 2 * isMaximising, enPassantCol);
    int lookupVal = tTable.lookupEvaluation(-1 + 2 * isMaximising, depth, transpositions, hash_key);
    int currentNodes = nodes;
    if (lookupVal != 999999) {
        if (isMaximising) {
            alpha = lookupVal;
            return;
        }
        else {
            beta = lookupVal;
            return;
        }
    }
    */
    int score = miniMax(board, depth + 1, !isMaximising, enPassantCol, oppositePieces, ownPieces, alpha, beta);
    //tTable.storeEvaluation(-1 + 2 * isMaximising, depth, score, nodes - currentNodes, hash_key);
    if (isMaximising) {
        if (score > alpha) {
            alpha = score;
            if (depth == 0) {
                miniMaxMove = to_string(selectSquare / 8) + to_string(selectSquare % 8) + to_string(targetSquare / 8) + to_string(targetSquare % 8) + to_string(promotion);
            }
        }
    }
    else {
        if (score < beta) {
            beta = score;
            if (depth == 0) {
                miniMaxMove = to_string(selectSquare / 8) + to_string(selectSquare % 8) + to_string(targetSquare / 8) + to_string(targetSquare % 8) + to_string(promotion);
            }
        }
    }
}

void Bot::changeBoard_mm(int Board[64], int selectSquare, int targetSquare, int promotion) {
    int takenpiece = Board[targetSquare];
    int movingpiece = Board[selectSquare];
    Board[targetSquare] = movingpiece;
    Board[selectSquare] = 0;
    if (promotion == 0)
    {
        // enpassant
        if (abs(movingpiece) == 1 && (selectSquare - targetSquare) % 8 != 0 && takenpiece == 0) {
            Board[(selectSquare - selectSquare % 8) + targetSquare % 8] = 0;
        }
        // castling
        else if (abs(movingpiece) == 100 && abs(targetSquare - selectSquare) == 2) {
            // case 2 = short castle, case -2 = long castle
            switch (targetSquare - selectSquare) {
            case 2:
                Board[targetSquare - 1] = Board[targetSquare + 1];
                Board[targetSquare + 1] = 0;
                break;
            case -2:
                Board[targetSquare + 1] = Board[targetSquare - 2];
                Board[targetSquare - 2] = 0;
                break;
            }
        }
    }
    else
    {
        if (promotion == 9) {
            Board[targetSquare] = 9 * movingpiece;
        }
        else if (promotion == 4) {
            Board[targetSquare] = 4 * movingpiece;
        }
        else if (promotion == 3) {
            Board[targetSquare] = 3 * movingpiece;
        }
        else if (promotion == 5) {
            Board[targetSquare] = 5 * movingpiece;
        }
    }
    return;
}

int Bot::color(int val)
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

void Bot::initPieceHashes() {
    hash<ulong> inthash;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            pieceHashes[i][j] = inthash(ulong(100 * i + j));
        }
    }
    return;
}

void Bot::initEnPassant() {
    hash<ulong> hash;
    for (int i = 0; i < 16; i++) {
            enPassantHashes[i] = hash(ulong(i));
    }
    return;
}

ulong Bot::hashBoard(int Board[64], int whiteMove, int enPassantCol) {
    ulong hash_key = 0ULL;
    int pieceIndex;
    for (int square = 0; square < 64; square++) {
        if (Board[square] != 0) {
            switch (Board[square]) {
            case 1:
                pieceIndex = 0;
                break;
            case 3:
                pieceIndex = 1;
                break;
            case 4:
                pieceIndex = 2;
                break;
            case 5:
                pieceIndex = 3;
                break;
            case 9:
                pieceIndex = 4;
                break;
            case 100:
                pieceIndex = 5;
                break;
            case -1:
                pieceIndex = 6;
                break;
            case -3:
                pieceIndex = 7;
                break;
            case -4:
                pieceIndex = 8;
                break;
            case -5:
                pieceIndex = 9;
                break;
            case -9:
                pieceIndex = 10;
                break;
            case -100:
                pieceIndex = 11;
                break;
            }
            hash_key ^= pieceHashes[pieceIndex][square];
        }
    }
    if (enPassantCol != -1) {
        if (whiteMove == -1) {
            hash_key ^= enPassantHashes[enPassantCol];
        }
        else {
            hash_key ^= enPassantHashes[8 + enPassantCol];
        }
    }
    return hash_key;
}

int Bot::ForceKingToCornerEndgameEval(int friendlySquare, int opposingSquare) {
    int evaluation = 0;
    int oppDistRow = max(3 - opposingSquare / 8, opposingSquare / 8 - 4);
    int oppDistCol = max(3 - opposingSquare % 8, opposingSquare % 8 - 4);
    evaluation = oppDistCol + oppDistRow;

    int kingDistRow = abs(friendlySquare - opposingSquare) / 8;
    int kingDistCol = abs((friendlySquare % 8) - (opposingSquare % 8));

    evaluation += (14 - (kingDistCol + kingDistRow)) * whiteMove;
    return evaluation;
}
/*
bool Bot::botRepitition(int Board[8][8]) {
    // intialise variables to 0
    int count = 0;
    int repititionCount = 0;
    // nested loop of i,j
    for (int i = 0; i < move; i++)
    {
        // i = the move the position was recorded at
        // repetition becomes zero for each value of i
        repititionCount = 0;
        // j = another move for which to compare move i position to move j
        // position count becomes 0 for each iteration
        count = 0;
        // nested loop of u, v
        for (int u = 0; u < 8; u++)
        {
            for (int v = 0; v < 8; v++)
            {
                // If the piece at coordinates u,v on move i = the piece at
                // coordinates u,v at move j, increment count by 1
                if (_BoardHistory[u][v][i] == Board[u][v])
                {
                    count++;
                }
                else {
                    goto nextboard;
                }
            }
        }
    nextboard:
        // since the dimensions of the board is 8x8 (8x8=64), if count = 64, every
        // piece on every square comparing moves i and j are the same. Therefore,
        // the repetition count will be incremented by 1
        if (count == 64)
        {
            repititionCount++;
        }
        // If repititionCount >= 3, the function will return true since a threefold
        // repitition has occurred Even through it should be == 3, >= 3 is used as a
        // safety measure
        if (repititionCount >= 2)
        {
            return true;
        }
    }
    // if threefold repetition has not occurred, return false
    return false;
}
*/

string Bot::makeMove(short int Board[64], short int enPassantCol) {
    int replicaBoard[64];
    int eval;
    copyBoard(replicaBoard, Board);
    displayMove++;
    if (!exitedOpening) {
        auto startTime = high_resolution_clock::now();
        string openingMove = openingBook.searchHash(replicaBoard, castlingAbility, enPassantCol, whiteMove);
        if (openingMove != "not found" && openingMove != "error") {
            auto endTime = high_resolution_clock::now();
            duration<double> duration = endTime - startTime;
            cout << "Opening Move: " << openingMove << endl;
            cout << "Runtime for opening: " << duration.count() << endl;
            if (duration.count() < 0.2) {
                this_thread::sleep_for(chrono::milliseconds(200 - int(duration.count() * 1000)));
            }
            return openingMove;
        }
        else {
            cout << "opening move not found." << endl;
            exitedOpening = true;
        }
    }
    vector<int> whitePieces = getPieces(Board, 1);
    vector<int> blackPieces = getPieces(Board, -1);
    auto startTime = high_resolution_clock::now();
    if (whiteMove == 1) {
        eval = miniMax(replicaBoard, 0, 1, enPassantCol, whitePieces, blackPieces, -2000000, 2000000);
    }
    else {
        eval = miniMax(replicaBoard, 0, 0, enPassantCol, blackPieces, whitePieces, -2000000, 2000000);
    }
    auto endTime = high_resolution_clock::now();
    duration<double> duration = endTime - startTime;
    updateDNdt(int(nodes / duration.count()));
    fastestMove = max(unsigned int(nodes / duration.count()), fastestMove);
    //tTable.clear();
    //if move is calculated under 400 ms, wait until 400ms has passed
    if (duration.count() < 0.4) {
        this_thread::sleep_for(chrono::milliseconds(400 - int(duration.count() * 1000)));
    }
    //print statements ---------------------------------
    printf("Evaluation: %.2f, Nodes: %s\n", double(eval) / 100, commaSeparator(nodes).c_str());
    //cout << "Transpositions: " << commaSeparator(transpositions) << ", transposition %: " << 100 * double(transpositions) / (double(nodes)) << "%" << endl;
    cout << "Nodes per second: " << commaSeparator(int(nodes / duration.count())) << endl;
    cout << "Fastest Nodes per second: " << commaSeparator(fastestMove) << endl;
    cout << "Average Nodes per second: " << commaSeparator(dNdt) << endl;
    printf("Total Run Time: %.2f seconds\n", duration.count());
    //----------------------------------------------------
    nodes = 0;
    transpositions = 0;
    return miniMaxMove;
}

void Bot::copyBoard(int emptyBoard[64], short int actualBoard[64]) {
    for (int square = 0; square < 64; square++) {
        emptyBoard[square] = actualBoard[square];
    }
}

vector<int> Bot::getPieces(short int board[64], short int pieceColor)
{
    vector<int> pieces;
    for (int square = 0; square < 64; square++)
    {
        if (color(board[square]) == pieceColor)
        {
            pieces.emplace_back(square);
        }
    }
    return pieces;
}

void Bot::updateDNdt(int newNodes) {
    nodeSum += newNodes;
    dNdt = nodeSum / numBotMoves;
    numBotMoves++;
    return;
}

string Bot::commaSeparator(int integer) {
    int n = 0;
    string numCommas;
    while (true) {
        if ((integer % int(pow(10, n))) == integer) {
            break;
        }
        else {
            n++;
        }
    }
    for (int i = 0; i < n; i++) {
        int digit = 0;
        digit += integer % int(pow(10, i + 1));
        digit = digit / pow(10, i);
        if (i % 3 == 0 && i != 0) {
            numCommas += ',';
        }
        numCommas += char(digit + '0');
    }
    reverse(numCommas.begin(), numCommas.end());
    return numCommas;
}

void Bot::updateCastlingRights(bool blackLongCastle, bool blackShortCastle, bool whiteShortCastle, bool whiteLongCastle) {
    if (!blackLongCastle) {
        castlingAbility[0] = false;
    }
    if (!blackShortCastle) {
        castlingAbility[1] = false;
    }
    if (!whiteShortCastle) {
        castlingAbility[2] = false;
    }
    if (!whiteLongCastle) {
        castlingAbility[3] = false;
    }
}

void Bot::oldlegalMoves(int Board[64], vector<int>& legalMoves, int selectSquare, int enPassantCol) {
    int row = selectSquare / 8;
    row = row * 8;
    int col = selectSquare % 8;
    int ownColor = color(Board[selectSquare]);

    if (abs(Board[selectSquare]) == 100)
    {
        // enable king to move 1 square out from its square
        if (col < 7) {
            if (selectSquare < 55 && color(Board[selectSquare + 9]) != ownColor && !searchingKingAttack(Board, selectSquare + 9, ownColor)) {
                legalMoves.emplace_back(selectSquare + 9);
            }
            if (selectSquare < 63 && color(Board[selectSquare + 1]) != ownColor && !searchingKingAttack(Board, selectSquare + 1, ownColor)) {
                legalMoves.emplace_back(selectSquare + 1);
            }
            if (selectSquare > 6 && color(Board[selectSquare - 7]) != ownColor && !searchingKingAttack(Board, selectSquare - 7, ownColor)) {
                legalMoves.emplace_back(selectSquare - 7);
            }
        }
        if (col > 0) {
            if (selectSquare > 8 && color(Board[selectSquare - 9]) != ownColor && !searchingKingAttack(Board, selectSquare - 9, ownColor)) {
                legalMoves.emplace_back(selectSquare - 9);
            }
            if (selectSquare < 57 && color(Board[selectSquare + 7]) != ownColor && !searchingKingAttack(Board, selectSquare + 7, ownColor)) {
                legalMoves.emplace_back(selectSquare + 7);
            }
            if (selectSquare > 0 && color(Board[selectSquare - 1]) != ownColor && !searchingKingAttack(Board, selectSquare - 1, ownColor)) {
                legalMoves.emplace_back(selectSquare - 1);
            }
        }
        if (selectSquare < 56 && color(Board[selectSquare + 8]) != ownColor && !searchingKingAttack(Board, selectSquare + 8, ownColor)) {
            legalMoves.emplace_back(selectSquare + 8);
        }
        if (selectSquare > 7 && color(Board[selectSquare - 8]) != ownColor && !searchingKingAttack(Board, selectSquare - 8, ownColor)) {
            legalMoves.emplace_back(selectSquare - 8);
        }

        //castling
        if (!searchingKingAttack(Board, selectSquare, ownColor)) {
            //white castling
            if (ownColor == 1 && selectSquare == 60)
            {
                //short castle
                if (castlingAbility[2] && Board[61] == 0 && Board[62] == 0 && Board[63] == 5 && !castleThroughCheck_mm(Board, 62, 1))
                {
                    legalMoves.emplace_back(62);
                }
                //long castle
                if (castlingAbility[3] && Board[59] == 0 && Board[58] == 0 && Board[57] == 0 && Board[56] == 5 && !castleThroughCheck_mm(Board, 58, 1))
                {
                    legalMoves.emplace_back(58);
                }
            }
            //black castling
            else if (ownColor == -1 && selectSquare == 4 && !castleThroughCheck_mm(Board, 4, -1))
            {
                //short castle
                if (castlingAbility[1] && Board[5] == 0 && Board[6] == 0 && Board[7] == -5 && !castleThroughCheck_mm(Board, 6, -1))
                {
                    legalMoves.emplace_back(6);
                }
                //long castle
                if (castlingAbility[0] && Board[3] == 0 && Board[2] == 0 && Board[1] == 0 && Board[0] == -5 && !castleThroughCheck_mm(Board, 2, -1))
                {
                    legalMoves.emplace_back(2);
                }
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

