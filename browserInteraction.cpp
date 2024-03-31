#pragma once
#include <fstream>
#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::experimental::filesystem;

inline void writeBotInfo(string data, int lineNumber) {
    if (lineNumber < 1 || lineNumber > 4) {
        cout << "invalid line write." << endl;
        return;
    }
    start:
    ifstream inFile("botInfo.txt");
    vector<string> lines;
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();
        lines[lineNumber - 1] = data;
        ofstream outFile("botInfo.txt");
        if (outFile.is_open()) {
            for (const auto & l : lines) {
                outFile << l << endl;
            }
            outFile.close();
        }
        else {
            cerr << "botInfo writing error" << endl;
            goto start;
        }
    }
    else {
        cerr << "botInfo reading error" << endl;
        goto start;
    }
    return;
}

inline string readServerInfo(int line) {
    if (line < 1 || line > 6) {
        cout << "invalid read line" << endl;
        return "invalid";
    }
    fs::path currentPath = fs::current_path();
    fs::path adjacentPath = currentPath.parent_path() / "browserInterface";
    fs::path filePath = adjacentPath / "serverInfo.txt";
    ifstream file(filePath);
    string fileLine;

    if (file.is_open()) {

        for (int i = 1; i < line; i++) {
            if (!getline(file, fileLine)) {
                cout << "failed to read server info." << endl;
                return "";
            }
        }

        getline(file, fileLine);
        file.close();
        return fileLine;
    }
    else {
        cerr << "Unable to open file." << endl;
    }
    return "";
}

inline void closeBrowserInterface() {
    writeBotInfo("1", 1);
    return;
}

inline bool hasLoggedIn() {
    string result = readServerInfo(1);
    if (result == "1") {
        cout << "logged in" << endl;
        return true;
    }
    else if (result == "0") {
        cout << "not logged in" << endl;
        return false;
    }
    cout << "login error: " << result << endl;
    return false;
}

inline void startOnlineGame() {
    writeBotInfo("1", 2);
    return;
}

inline string botColor() {
    return readServerInfo(3);
}

inline bool isOurMove() {
    string whoseMove = readServerInfo(4);
    string ourMove = botColor();
    if (whoseMove == ourMove) {
        return true;
        cout << "bot's move." << endl;
    }
    else {
        cout << "opponent move ..." << endl;
        return false;
    }
}

inline bool grabBoard(short int Board[64]) {
    string boardInfo = readServerInfo(5);
    cout << "server board: " << boardInfo << endl;
    if (boardInfo == "0") {
        cout << "board hasn't been written." << endl;
        return false;
    }
    int tempBoard[8][8] = { 0 };
    int multiplier = 1;
    int pieceValue = 0;
    for (int i = 0; i < boardInfo.size(); i += 4) {
        if (boardInfo[i] == 'b') {
            multiplier = -1;
        }
        else {
            multiplier = 1;
        }
        if (boardInfo[i + 1] == 'r') {
            pieceValue = 5;
        } else if (boardInfo[i + 1] == 'q') {
            pieceValue = 9;
        }
        else if (boardInfo[i + 1] == 'k') {
            pieceValue = 100;
        }
        else if (boardInfo[i + 1] == 'p') {
            pieceValue = 1;
        }
        else if (boardInfo[i + 1] == 'b') {
            pieceValue = 4;
        }
        else if (boardInfo[i + 1] == 'n') {
            pieceValue = 3;
        }
        tempBoard[8 - (boardInfo[i + 3] - '0')][boardInfo[i + 2] - '0' - 1] = multiplier * pieceValue;
    }
    cout << "tempBoard" << endl;
    int validityCounter = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << tempBoard[i][j] << " ";
            if (tempBoard[i][j] != Board[i * 8 + j]) {
                validityCounter++;
            }
        }
        cout << endl;
    }
    if (validityCounter == 64) {
        cout << "board is competely different" << endl;
        return false;
    }
    if (validityCounter > 3) {
        cout << "invalid board read" << endl;
        return false;
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Board[i * 8 + j] = tempBoard[i][j];
        }
    }
    cout << "board is different, returning true." << endl;
    return true;
}

inline void makeOnlineMove(short int Board[64], string move) {
    cout << "making online move." << endl;
    cout << "move: " << move << endl;
    string side;
    string piece;
    string selectSquare;
    string landingSquare;
    string selectPrefix = "piece ";
    string capturePrefix = "hint square-";
    if (Board[(move[2] - '0') * 8 + move[3] - '0'] > 0) {
        side = "w";
    }
    else {
        side = "b";
    }
    switch (abs(Board[(move[2] - '0') * 8 + move[3] - '0'])) {
    case 1:
        piece = "p";
        break;
    case 3:
        piece = "n";
        break;
    case 4:
        piece = "b";
        break;
    case 5:
        piece = "r";
        break;
    case 9:
        piece = "q";
        break;
    case 100:
        piece = "k";
        break;
    }
    char selectCol = move[1] + 1;
    char selectRow = '8' - move[0] + '0';
    char landingcolumn = move[3] + 1;
    char landingrow = '8' - move[2] + '0';
    selectSquare = string(1, selectCol) + string(1, selectRow);
    landingSquare = string(1, landingcolumn) + string(1, landingrow);
    selectPrefix = selectPrefix + side + piece + " square-" + selectSquare;
    capturePrefix = capturePrefix + landingSquare;
    if (move[4] != '0') {
        string promotion = "promotion-piece ";
        int movePromotion = move[4] - '0';
        promotion = promotion + side;
        if (movePromotion == 9) {
            promotion = promotion + "q";
        }
        else if (movePromotion == 5) {
            promotion = promotion + "r";
        }
        else if (movePromotion == 4) {
            promotion = promotion + "b";
        }
        else if (movePromotion == 3) {
            promotion = promotion + "n";
        }
        writeBotInfo(selectPrefix + '_' + capturePrefix + '_' + promotion, 3);
        return;
    }
    writeBotInfo(selectPrefix + '_' + capturePrefix, 3);
    return;
}

inline bool isGameOver() {
    string result = readServerInfo(6);
    if (result == "1") {
        return true;
    }
    else if (result == "0") {
        return false;
    }
}

inline void setupBotInfo() {
    ofstream outFile("botInfo.txt");
    if (!outFile.is_open()) {
        cerr << "Failed to open botInfo.txt" << endl;
        setupBotInfo();
        return;
    }

    if (outFile.is_open()) {
        for (int i = 0; i < 4; i++) {
            outFile << "0" << endl;
        }
        outFile.close();
    }

    return;
}

inline void updateGamemode(string num) {
    writeBotInfo(num, 4);
    return;
}