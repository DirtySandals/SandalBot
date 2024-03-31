#include "Board.h"
#include "clickableArea.h"
using namespace std;
using namespace sf;

struct promotions {
    sf::Sprite sprite;
    int piece;
};

extern sf::RenderWindow window;
extern Board* board;
extern vector<promotions> promoting;
extern sf::Sprite _board[8][8];
extern vector<sf::CircleShape> legalCircles;
extern vector<sf::RectangleShape> highlights;
extern sf::RectangleShape checkking;
extern bool inGame;
extern sf::Sprite result;
extern sf::Sprite oldSelect;
extern sf::Sprite oldDestination;
extern sf::Sprite selectSquare;
extern sf::Sprite destinationSquare;
extern sf::Texture wpawntexture;
extern sf::Texture bpawntexture;
extern sf::Texture wcastletexture;
extern sf::Texture bcastletexture;
extern sf::Texture wknighttexture;
extern sf::Texture bknighttexture;
extern sf::Texture wbishoptexture;
extern sf::Texture bbishoptexture;
extern sf::Texture wqueentexture;
extern sf::Texture bqueentexture;
extern sf::Texture wkingtexture;
extern sf::Texture bkingtexture;
extern sf::Texture emptytexture;
extern Texture ySquare;
extern Texture wwin;
extern Texture bwin;
extern Texture rdraw;
extern Texture black1;
extern Texture sdraw;
extern Texture white1;
extern Texture black1;
extern Texture hvh1;
extern Texture hvb1;
extern Texture bvb1;
extern Texture wb1;
extern Texture one1;
extern Texture two1;
extern Texture thr1;
extern Texture four1;
extern Texture depth1;
extern Texture online;
extern Sprite spriteboard;
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
extern Texture boardtexture;
extern RectangleShape hoverSquare;
extern vector<clickableArea> clickableAreas;


inline void displayPromotion() {
    sf::RectangleShape rectangle(sf::Vector2f(525.f, 150.f));
    rectangle.setFillColor(sf::Color::White);
    rectangle.setPosition(137.5f, 325.5f);
    window.draw(rectangle);
    for (int i = 0; i < 8; i++)
    {
        if (promoting[i].piece == board->getwhiteMove() * 9)
        {
            promoting[i].sprite.setScale(1.6f, 1.6f);
            promoting[i].sprite.setPosition(162.5f, 350);
            window.draw(promoting[i].sprite);
        }
        else if (promoting[i].piece == board->getwhiteMove() * 5)
        {
            promoting[i].sprite.setScale(1.6f, 1.6f);
            promoting[i].sprite.setPosition(287.5f, 350);
            window.draw(promoting[i].sprite);
        }
        else if (promoting[i].piece == board->getwhiteMove() * 4)
        {
            promoting[i].sprite.setScale(1.6f, 1.6f);
            promoting[i].sprite.setPosition(412.5f, 350);
            window.draw(promoting[i].sprite);
        }
        else if (promoting[i].piece == board->getwhiteMove() * 3)
        {
            promoting[i].sprite.setScale(1.6f, 1.6f);
            promoting[i].sprite.setPosition(537.5f, 350);
            window.draw(promoting[i].sprite);
        }
    }
    return;
}

inline void boardToSprite() {
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board->board[i * 8 + j] == -1)
            {
                _board[i][j].setTexture(bpawntexture);
            }
            if (board->board[i * 8 + j] == 1)
            {
                _board[i][j].setTexture(wpawntexture);
            }
            if (board->board[i * 8 + j] == -100)
            {
                _board[i][j].setTexture(bkingtexture);
            }
            if (board->board[i * 8 + j] == 100)
            {
                _board[i][j].setTexture(wkingtexture);
            }
            if (board->board[i * 8 + j] == -9)
            {
                _board[i][j].setTexture(bqueentexture);
            }
            if (board->board[i * 8 + j] == 9)
            {
                _board[i][j].setTexture(wqueentexture);
            }
            if (board->board[i * 8 + j] == -5)
            {
                _board[i][j].setTexture(bcastletexture);
            }
            if (board->board[i * 8 + j] == 5)
            {
                _board[i][j].setTexture(wcastletexture);
            }
            if (board->board[i * 8 + j] == -4)
            {
                _board[i][j].setTexture(bbishoptexture);
            }
            if (board->board[i * 8 + j] == 4)
            {
                _board[i][j].setTexture(wbishoptexture);
            }
            if (board->board[i * 8 + j] == -3)
            {
                _board[i][j].setTexture(bknighttexture);
            }
            if (board->board[i * 8 + j] == 3)
            {
                _board[i][j].setTexture(wknighttexture);
            }
            if (board->board[i * 8 + j] == 0)
            {
                _board[i][j].setTexture(emptytexture);
            }
            // Scales the piece images up to a bigger size
            _board[i][j].setScale(1.6f, 1.6f);
            // Sets the position of the pieces to be in the right square
            _board[i][j].setPosition(100 * j, 100 * i);
        }
    }
    return;
}

inline void showHighlights() {
    for (size_t i = 0; i < highlights.size(); i++) {
        window.draw(highlights[i]);
    }
    return;
}

inline void locateKing() {
    if (board->isCheck()) {
        for (int square = 0; square < 64; square++) {
            if (board->board[square] == -100 * board->getwhiteMove()) {
                checkking.setPosition(100 * (square % 8), 100 * (square / 8));
            }
        }
        return;
    }
    else {
        checkking.setPosition(-100, -100);
        return;
    }
}

inline void drawBoard() {
    boardToSprite();
    window.draw(oldSelect);
    window.draw(oldDestination);
    window.draw(selectSquare);
    window.draw(destinationSquare);
    window.draw(checkking);
    showHighlights();
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            window.draw(_board[i][j]);
        }
    }
    for (size_t i = 0; i < legalCircles.size(); i++) {
        window.draw(legalCircles[i]);
    }
    return;
}

inline void gameEnd() {
    result.setScale(0.6f, 0.6f);
    result.setPosition(118, 287);
    inGame = false;
    return;
}
inline void loadTextures() {
    boardtexture.loadFromFile("board.png");
    wpawntexture.loadFromFile("whitepawn.png");
    bpawntexture.loadFromFile("blackpawn.png");
    wcastletexture.loadFromFile("whitecastle.png");
    bcastletexture.loadFromFile("blackcastle.png");
    wknighttexture.loadFromFile("whiteknight.png");
    bknighttexture.loadFromFile("blackknight.png");
    wbishoptexture.loadFromFile("whitebishop.png");
    bbishoptexture.loadFromFile("blackbishop.png");
    wqueentexture.loadFromFile("whitequeen.png");
    bqueentexture.loadFromFile("blackqueen.png");
    wkingtexture.loadFromFile("whiteking.png");
    bkingtexture.loadFromFile("blackking.png");
    emptytexture.loadFromFile("blank.png");
    ySquare.loadFromFile("ysquare.png");
    wwin.loadFromFile("wcheckmate.png");
    bwin.loadFromFile("bcheckmate.png");
    rdraw.loadFromFile("drawrep.png");
    sdraw.loadFromFile("drawstale.png");
    white1.loadFromFile("white.png");
    black1.loadFromFile("black.png");
    hvh1.loadFromFile("humanvhuman.png");
    hvb1.loadFromFile("humanvbot.png");
    bvb1.loadFromFile("botvbot.png");
    wb1.loadFromFile("whiteblack.png");
    one1.loadFromFile("1.png");
    two1.loadFromFile("2.png");
    thr1.loadFromFile("3.png");
    four1.loadFromFile("4.png");
    depth1.loadFromFile("depth.png");
    online.loadFromFile("chesscom.png");
}

inline void adjustSprites() {
    white.setScale(0.7f, 0.7f);
    white.setPosition(150, 270);
    black.setScale(0.7f, 0.7f);
    black.setPosition(430, 270);
    hvh.setScale(0.4f, 0.4f);
    hvh.setPosition(200, 170);
    hvb.setScale(0.4f, 0.4f);
    hvb.setPosition(200, 300);
    bvb.setScale(0.4f, 0.4f);
    bvb.setPosition(205, 450);
    wb.setScale(0.7f, 0.7f);
    wb.setPosition(50, 100);
    one.setScale(0.6f, 0.6f);
    one.setPosition(85, 320);
    two.setScale(0.6f, 0.6f);
    two.setPosition(240, 320);
    three.setScale(0.6f, 0.6f);
    three.setPosition(420, 320);
    four.setScale(0.6f, 0.6f);
    four.setPosition(573, 320);
    depth.setScale(0.7f, 0.7f);
    depth.setPosition(75, 102);
    chesscom.setScale(0.4f, 0.4f);
    chesscom.setPosition(210, 600);
    //-----------------------------
    selectSquare.setTexture(ySquare);
    selectSquare.setColor(Color(255, 255, 255, 200));
    selectSquare.setPosition(-100, -100);
    destinationSquare.setTexture(ySquare);
    destinationSquare.setColor(Color(255, 255, 255, 200));
    destinationSquare.setPosition(-100, -100);
    oldSelect.setTexture(ySquare);
    oldSelect.setColor(Color(255, 255, 255, 200));
    oldSelect.setPosition(-100, -100);
    oldDestination.setTexture(ySquare);
    oldDestination.setColor(Color(255, 255, 255, 200));
    oldDestination.setPosition(-100, -100);
    spriteboard.setTexture(boardtexture);
    checkking.setPosition(-100, -100);
    Color kingColor(255, 0, 0, 200);
    checkking.setFillColor(kingColor);
    Color hoverColor(255, 255, 255, 100);
    hoverSquare.setFillColor(hoverColor);
    //--------------------------------
    white.setTexture(white1);
    black.setTexture(black1);
    hvh.setTexture(hvh1);
    hvb.setTexture(hvb1);
    bvb.setTexture(bvb1);
    wb.setTexture(wb1);
    two.setTexture(two1);
    one.setTexture(one1);
    three.setTexture(thr1);
    four.setTexture(four1);
    depth.setTexture(depth1);
    chesscom.setTexture(online);
    
}

inline void initVectors() {
    promoting.push_back({ Sprite(wcastletexture), 5 });
    promoting.push_back({ Sprite(wbishoptexture), 4 });
    promoting.push_back({ Sprite(wqueentexture), 9 });
    promoting.push_back({ Sprite(wknighttexture), 3 });
    promoting.push_back({ Sprite(bcastletexture), -5 });
    promoting.push_back({ Sprite(bqueentexture), -9 });
    promoting.push_back({ Sprite(bbishoptexture), -4 });
    promoting.push_back({ Sprite(bknighttexture), -3 });

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            clickableAreas.push_back({ {j * 100, i * 100, 100, 100}, to_string(i) + to_string(j) });
        }
    }
}
inline void initGraphics() {
    loadTextures();
    adjustSprites();
    initVectors();
}
