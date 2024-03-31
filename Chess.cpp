#include "Board.cpp"
#include "browserInteraction.cpp"
#include "Graphics.cpp"
#include "notationFunctions.cpp"
#include "Menu.cpp"
#include "pieceSelection.cpp"
#include "gameOver.cpp"
#include "clickableArea.h"

using namespace std;
using namespace sf;

RenderWindow window(VideoMode(800, 800), "Chess", Style::Titlebar | Style::Close);
Vector2i mousePosition = Mouse::getPosition(window);
Texture boardtexture;
Texture wpawntexture;
Texture bpawntexture;
Texture wcastletexture;
Texture bcastletexture;
Texture wknighttexture;
Texture bknighttexture;
Texture wbishoptexture;
Texture bbishoptexture;
Texture wqueentexture;
Texture bqueentexture;
Texture wkingtexture;
Texture bkingtexture;
Texture emptytexture;
Texture ySquare;
Texture wwin;
Texture bwin;
Texture rdraw;
Texture sdraw;
Texture white1;
Texture black1;
Texture hvh1;
Texture hvb1;
Texture bvb1;
Texture wb1;
Texture one1;
Texture two1;
Texture thr1;
Texture four1;
Texture depth1;
Texture online;
// Creates sprite called result, doesn't set texture yet as the texture
// depends on game result
Sprite result;
Sprite spriteboard;
Sprite selectSquare;
Sprite destinationSquare;
Sprite oldSelect;
Sprite oldDestination;
Sprite _board[8][8];
Sprite white;
Sprite black;
Sprite hvh;
Sprite hvb;
Sprite bvb;
Sprite wb;
Sprite two;
Sprite one;
Sprite three;
Sprite four;
Sprite depth;
Sprite chesscom;
RectangleShape checkking(Vector2f(100, 100));
RectangleShape hoverSquare(Vector2f(100, 100));
Color hoverColor(255, 255, 255, 100);

// intialise variables
int isCheckmate = 2;
int pieceSelectRow = -1;
int pieceSelectCol = -1;
int destinationRow = -1;
int destinationCol = -1;
int check = 0;
int stillCheck = 1;
string coordinateSelect;
unsigned short int promotion;
string answerBot = "-";
char answerWhite = '-';
int oldprow;
int oldpcol;
int olddrow;
int olddcol;
int depthInt = 0;
Board* board = new Board();
bool selecting = true;
bool placing = false;
bool selectingPromotionPiece = false;
bool inMenu = true;
bool inGame = false;
bool botMoving = false;
bool botFinished = false;
bool selectGamemode = true;
bool selectSide = false;
bool selectDepth = false;

vector<clickableArea> clickableAreas;
vector<clickableArea> validSquares;
vector<promotions> promoting;
vector<CircleShape> legalCircles;
vector<RectangleShape> highlights;

string commaSeparator(int integer) {
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

bool isOpeningMove(string move) {
    if (move.size() != 5) {
        return true;
    }
    for (int i = 0; i < move.size(); i++) {
        if (move[i] < '0' || move[i] > '9') {
            return true;
        }
    }
    return false;
}

void selectPromotion() {
    for (const auto& promotions : promoting)
    {
        if (promotions.sprite.getGlobalBounds().contains(
            static_cast<Vector2f>(mousePosition)))
        {
            if (abs(promotions.piece) == 9)
            {
                board->board[destinationRow * 8 + destinationCol] = board->getwhiteMove() * 9;
                promotion = 9;
            }
            else if (abs(promotions.piece) == 5)
            {
                board->board[destinationRow * 8 + destinationCol] = board->getwhiteMove() * 5;
                promotion = 5;
            }
            else if (abs(promotions.piece) == 4)
            {
                board->board[destinationRow * 8 + destinationCol] = board->getwhiteMove() * 4;
                promotion = 4;
            }
            else if (abs(promotions.piece) == 3)
            {
                board->board[destinationRow * 8 + destinationCol] = board->getwhiteMove() * 3;
                promotion = 3;
            }
        }
    }
    return;
}

void generateMoves() {
    vector<int> legalmoves;
    board->legalMoves(legalmoves, pieceSelectRow * 8 + pieceSelectCol);
    for (size_t it = 0; it < legalmoves.size(); it++) {
        unsigned short int targetSquare = legalmoves[it];
        short int tempBoard[64];
        board->copyBoard(tempBoard);
        int takenpiece = tempBoard[targetSquare];
        int movingpiece = tempBoard[pieceSelectRow * 8 + pieceSelectCol];
        tempBoard[targetSquare] = movingpiece;
        tempBoard[pieceSelectRow * 8 + pieceSelectCol] = 0;
        // enpassant
        if (abs(movingpiece) == 1 && pieceSelectCol - (targetSquare) % 8 != 0 && takenpiece == 0) {
            tempBoard[pieceSelectRow * 8 + targetSquare % 8] = 0;
        }
        //castle
        if (abs(movingpiece) == 100 && abs(pieceSelectRow * 8 + pieceSelectCol - targetSquare) == 2) {
            if (targetSquare % 8 > 4) {
                tempBoard[pieceSelectRow * 8 + 7] = 0;
                tempBoard[pieceSelectRow * 8 + 4] = 5 * board->getwhiteMove();
            }
            else {
                tempBoard[pieceSelectRow * 8] = 0;
                tempBoard[pieceSelectRow * 8 + 3] = 5 * board->getwhiteMove();
            }
        }

        if (!board->isIllegalCheck(tempBoard)) {
            validSquares.push_back({ {(targetSquare % 8) * 100, (targetSquare / 8) * 100, 100, 100}, to_string(targetSquare / 8) + to_string(targetSquare % 8) });
        }
    }
    return;
}

void gameResult(int side) {
    if (side == 1) {
        result.setTexture(wwin);
        board->setMoveList("  1-0");
    }
    else if (side == -1) {
        result.setTexture(bwin);
        board->setMoveList("  0-1");
    }
    else if (side == 0) {
        result.setTexture(sdraw);
        board->setMoveList("  1/2-1/2");
    }
    else if (side == 2) {
        result.setTexture(rdraw);
        board->setMoveList("  1/2-1/2");
    }
    result.setColor(Color(255, 255, 255, 240));
    gameEnd();
    writeMoves();
    return;
}

void switchTurn() {
    selectSquare.setPosition(100 * pieceSelectCol, 100 * pieceSelectRow);
    destinationSquare.setPosition(100 * destinationCol, 100 * destinationRow);
    oldSelect = selectSquare;
    oldDestination = destinationSquare;
    highlights.clear();
    board->setMove();
    board->setDisplayMove();
    board->setMoveList(moveToNotation(to_string(pieceSelectRow) + to_string(pieceSelectCol) + to_string(destinationRow) + to_string(destinationCol)));
    board->updateHistory();
    locateKing();
    if (board->isCheck() && board->checkmate()) {
        gameResult(board->getwhiteMove());
        return;
    }
    else if (!board->isCheck() && board->isStalemate()) {
        gameResult(0);
        return;
    }
    else if (board->isRepitition()) {
        gameResult(2);
        return;
    }
    else if (board->insuffienctMaterial()) {
        gameResult(0);
        return;
    }
    board->setwhiteMove();
    selecting = true;
    placing = false;
    selectingPromotionPiece = false;
    promotion = 0;
    printf("---------------------------------\n");
    return;
}

int main() {
    //setupBotInfo();
    window.setFramerateLimit(75);
    
    initGraphics();
    
    window.setPosition(Vector2i(1000, 100));
    vector<thread> botThread;
    string move;
    while (window.isOpen()) {
        Event event;
        Clock clock;
        window.clear();
        window.draw(spriteboard);
        mousePosition = Mouse::getPosition(window);

        for (const auto& area : clickableAreas)
        {
            if (area.rect.contains(mousePosition)) {
                hoverSquare.setPosition(100 * (area.name[1] - '0'), 100 * (area.name[0] - '0'));
                window.draw(hoverSquare);
                break;
            }
        }

        if (inGame) {
            drawBoard();
            if (placing && selectingPromotionPiece) {
                displayPromotion();
            }
            /*
            if (board->isMove('O')) {
                //----------------
                if (botThread.size() < 1) {
                    makeOnlineMove(board->board, move);
                    this_thread::sleep_for(chrono::milliseconds(200));
                    botMoving = true;
                    cout << "waiting for opponent move" << endl;
                    botThread.emplace_back([]() {
                        while (true) {
                            if (isGameOver()) {
                                break;
                            }
                            if (isOurMove() && grabBoard(board->board)) {
                                break;
                            }
                        }
                        botFinished = true;
                        botMoving = false;
                        });
                }
                if (botFinished) {
                    botThread[0].join();
                    botThread.clear();
                    cout << "thread joined." << endl;
                    botFinished = false;
                    if (isGameOver()) {
                        inMenu = false;
                        inGame = false;
                    }
                    switchTurn();

                }
            }
            */
            if ((botFinished || !botMoving) && board->isMove('B')) {
                if (botThread.size() < 1) {
                    botMoving = true;
                    cout << "move no.: " << board->getMove() << endl;
                    cout << "thread started." << endl; cout << endl;

                    botThread.emplace_back([&move]() {
                        board->updateBotCastlingRights();
                        move = board->makeMove();
                        botFinished = true;
                        botMoving = false;
                        });

                }
                if (botFinished) {
                    botThread[0].join();
                    botThread.clear();
                    cout << "move before: " << move << endl;
                    pieceSelectRow = move[0] - '0';
                    pieceSelectCol = move[1] - '0';
                    destinationRow = move[2] - '0';
                    destinationCol = move[3] - '0';
                    promotion = move[4] - '0';
                    cout << pieceSelectRow << pieceSelectCol << destinationRow << destinationCol << promotion << endl;
                    board->changeBoard(pieceSelectRow * 8 + pieceSelectCol, destinationRow * 8 + destinationCol, promotion);
                    switchTurn();
                    botFinished = false;
                    move = "-";
                }

            }
        }
        else if (inMenu) {
            displayMenu();
        }
        if (!inGame && !inMenu) {
            gameOver();
        }
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                //closeBrowserInterface();
            }
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                if (inMenu) {
                    Menu();
                }
                else if (inGame) {
                    pieceSelection();
                }
            }
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
                for (const auto& area : clickableAreas)
                {
                    if (area.rect.contains(mousePosition)) {
                        RectangleShape highlightSquare(Vector2f(100, 100));
                        highlightSquare.setFillColor(Color(255, 165, 0, 170));
                        highlightSquare.setPosition(100 * (area.name[1] - '0'), 100 * (area.name[0] - '0'));
                        highlights.push_back(highlightSquare);
                        validSquares.clear();
                        legalCircles.clear();
                        selectSquare.setPosition(-100, -100);
                        placing = false;
                        selecting = true;
                        break;
                    }
                }
            }
        }
        window.display();
    }
    //closeBrowserInterface();
    for (auto& thread : botThread) {
        thread.join();
    }
    botThread.clear();
    delete board;
    return 0;
}