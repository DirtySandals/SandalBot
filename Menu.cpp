#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "Board.h"

using namespace sf;

extern bool selectGamemode;
extern bool selectSide;
extern bool selectDepth;
extern RenderWindow window;
extern Vector2i mousePosition;
extern Sprite hvh;
extern Sprite hvb;
extern Sprite bvb;
extern Sprite chesscom;
extern Sprite wb;
extern Sprite white;
extern Sprite black;
extern Sprite one;
extern Sprite two;
extern Sprite three;
extern Sprite four;
extern Sprite depth;
extern string answerBot;
extern char answerWhite;
extern Board* board;
extern void updateGamemode(string str);
extern bool inGame;
extern bool inMenu;
extern int depthInt;
extern bool hasLoggedIn();
extern void startOnlineGame();
extern string readServerInfo(int num);
extern void writeBotInfo(string str, int num);
extern string botColor();


inline void displayMenu() {
    if (selectGamemode) {
        window.draw(hvh);
        window.draw(hvb);
        window.draw(bvb);
        //window.draw(chesscom);
    }
    else if (selectSide) {
        window.draw(wb);
        window.draw(white);
        window.draw(black);
    }
    else if (selectDepth) {
        window.draw(one);
        window.draw(two);
        window.draw(three);
        window.draw(four);
        window.draw(depth);
    }
}

inline void Menu() {
    if (selectGamemode) {
        if (hvh.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            answerBot = "humans";
            board->assignPlayer1('H', 0);
            board->assignPlayer2('H', 0);
            //updateGamemode("2");
            selectGamemode = false;
            inGame = true;
            inMenu = false;
        }
        else if (hvb.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            answerBot = "humanbot";
            //updateGamemode("2");
            selectGamemode = false;
            selectSide = true;
        }
        else if (bvb.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            answerBot = "bots";
            //updateGamemode("2");
            selectGamemode = false;
            selectDepth = true;
        }
        /*
        else if (chesscom.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            answerBot = "chesscom";
            depthInt = 6;
            updateGamemode("1");
            this_thread::sleep_for(chrono::seconds(1));
            while (!hasLoggedIn()) {
                this_thread::sleep_for(chrono::seconds(1));
            }
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
            cout << "Starting match." << endl;
            selectGamemode = false;
            inMenu = false;
            inGame = true;
        }
        */
    }
    else if (selectSide) {
        if (white.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            answerWhite = 'W';
            board->assignPlayer1('H', 0);
            selectSide = false;
            selectDepth = true;
        }
        else if (black.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            answerWhite = 'B';
            board->assignPlayer2('H', 0);
            selectSide = false;
            selectDepth = true;
        }
    }
    else if (selectDepth) {
        if (one.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            depthInt = 1;
        }
        else if (two.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            depthInt = 2;
        }
        else if (three.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            depthInt = 3;
        }
        else if (four.getGlobalBounds().contains(static_cast<Vector2f>(mousePosition)))
        {
            depthInt = 6;
        }
        if (depthInt != 0) {
            if (answerWhite == 'W') {
                board->assignPlayer2('B', depthInt);
            }
            else if (answerWhite == 'B') {
                board->assignPlayer1('B', depthInt);
            }
            else {
                board->assignPlayer1('B', depthInt);
                board->assignPlayer2('B', depthInt);
            }
            selectDepth = false;
            inGame = true;
            inMenu = false;
        }
    }
}