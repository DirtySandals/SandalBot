#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "Board.h"

using namespace sf;

extern RenderWindow window;
extern void drawBoard();
extern string answerBot;
extern bool inGame;
extern Board* board;
extern string botColor();
extern void writeBotInfo(string str, int num);
extern void startOnlineGame();
extern string readServerInfo(int num);
extern Sprite spriteboard;
extern Sprite result;
extern int depthInt;
extern bool botMoving;
extern bool botFinished;
extern vector<RectangleShape> highlights;

inline void gameOver() {
    window.draw(spriteboard);
    drawBoard();
    if (answerBot != "chesscom") {
        window.draw(result);
        window.display();
        this_thread::sleep_for(chrono::seconds(6));
        window.close();
    }
    else {
        cout << "game ended." << endl;
        delete board;
        board = new Board();
        inGame = true;
        startOnlineGame();
        while (readServerInfo(2) == "0") {
            this_thread::sleep_for(chrono::seconds(1));
        }
        this_thread::sleep_for(chrono::seconds(1));
        writeBotInfo("0", 2);
        string side = botColor();
        while (true) {
            if (side == "w") {
                cout << "bot is playing white" << endl;
                board->assignPlayer1('B', depthInt);
                board->assignPlayer2('O', 0);
                break;
            }
            else if (side == "b") {
                cout << "bot is playing black" << endl;
                board->assignPlayer1('O', 0);
                board->assignPlayer2('B', depthInt);
                break;
            }
            else {
                cout << "failed to detect side" << endl;
            }
        }
        botMoving = false;
        botFinished = false;
        if (highlights.size() > 0) {
            highlights.clear();
        }
    }
}