#include "Board.h"
#include "clickableArea.h"
using namespace sf;

extern Board* board;

extern vector<clickableArea> clickableAreas;
extern vector<clickableArea> validSquares;
extern vector<RectangleShape> highlights;
extern vector<CircleShape> legalCircles;
extern bool selecting;
extern bool placing;
extern int pieceSelectRow;
extern int pieceSelectCol;
extern int destinationRow;
extern int destinationCol;
extern Vector2i mousePosition;
extern void generateMoves();
extern Sprite destinationSquare;
extern Sprite selectSquare;
extern bool selectingPromotionPiece;
extern void switchTurn();
extern void selectPromotion();



inline void pieceSelection() {
    if (selecting && board->isMove('H')) {
        for (const auto& area : clickableAreas)
        {
            if (area.rect.contains(mousePosition)) {
                highlights.clear();
                break;
            }
        }
        for (const auto& area : clickableAreas)
        {
            if (area.rect.contains(mousePosition) && board->color(board->board[(area.name[0] - '0') * 8 + area.name[1] - '0']) == board->getwhiteMove())
            {
                pieceSelectRow = area.name[0] - '0';
                pieceSelectCol = area.name[1] - '0';
                generateMoves();
                if (validSquares.size() > 0) {
                    placing = true;
                    selecting = false;
                    destinationSquare.setPosition(-100, -100);
                    selectSquare.setPosition(100 * pieceSelectCol, 100 * pieceSelectRow);
                    break;
                }
            }
        }
        for (const auto& area : validSquares)
        {
            Color color(0, 0, 0, 90);
            if (board->board[(area.name[0] - '0') * 8 + area.name[1] - '0'] == 0) {
                CircleShape circle(15);
                circle.setPosition(100 * (area.name[1] - '0') + 35, 100 * (area.name[0] - '0') + 35);
                circle.setFillColor(color);
                legalCircles.push_back(circle);
            }
            else {
                CircleShape hollowCircle(40);
                hollowCircle.setPosition(100 * (area.name[1] - '0') + 10, 100 * (area.name[0] - '0') + 10);
                hollowCircle.setFillColor(Color::Transparent);
                hollowCircle.setOutlineThickness(10);
                hollowCircle.setOutlineColor(color);
                legalCircles.push_back(hollowCircle);
            }
        }
    }
    else if (placing && board->isMove('H')) {
        if (selectingPromotionPiece) {
            selectPromotion();
            switchTurn();
        }
        else {
            bool madeMove = false;
            for (const auto& area : validSquares)
            {
                if (area.rect.contains(mousePosition))
                {
                    madeMove = true;
                    destinationRow = area.name[0] - '0';
                    destinationCol = area.name[1] - '0';
                    board->changeBoard(pieceSelectRow * 8 + pieceSelectCol, destinationRow * 8 + destinationCol, 0);
                    if (abs(board->board[destinationRow * 8 + destinationCol]) == 1 && (destinationRow == 7 || destinationRow == 0)) {
                        selectingPromotionPiece = true;
                    }
                    else {
                        switchTurn();
                    }
                    break;
                }
            }
            if (madeMove == false) {
                selectSquare.setPosition(-100, -100);
                placing = false;
                selecting = true;
            }
        }
        validSquares.clear();
        legalCircles.clear();
    }
}