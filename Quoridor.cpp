#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <algorithm>

using namespace std;

const vector<char> wasd = {'w', 'a', 's', 'd'};

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printBoard(int boardSize, map<int, int>& playerCords, map<int, vector<vector<int>>>& playerWallsCords) {
    vector<string> wallSymbol = {"X", "A", "B", "C", "D"};
    
    for (int i = 0; i < boardSize; i++) {
        if (i % 2 != 0) {
            string row(boardSize, '-');
            for (auto& player : playerWallsCords) {
                for (auto& wallCords : player.second) {
                    for (auto wallCord : wallCords) {
                        if ((wallCord / boardSize) == i) {
                            if (row[wallCord % boardSize] == '-' || player.first == 0) {
                                row[wallCord % boardSize] = wallSymbol[player.first][0];
                            }
                        }
                    }
                }
            }
            cout << row << endl;
        } else {
            string row;
            for (int j = 0; j < boardSize; j++) {
                if (j % 2 == 0) {
                    row += ".";
                } else {
                    bool wallAdded = false;
                    for (auto& player : playerWallsCords) {
                        for (auto& wallCords : player.second) {
                            for (auto wallCord : wallCords) {
                                if ((wallCord / boardSize) == i && (wallCord % boardSize) == j) {
                                    if (row.back() == '|' || row.back() == '.' || player.first == 0) {
                                        if (player.first == 0 && row.back() != '|' && row.back() != '.') {
                                            row.pop_back();
                                        }
                                        row += wallSymbol[player.first];
                                        wallAdded = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!wallAdded) {
                        row += "|";
                    }
                }
            }
            for (auto& player : playerCords) {
                int rowPos = player.second % boardSize;
                int colPos = player.second / boardSize;
                if (colPos == i) {
                    row[rowPos] = '0' + player.first;
                }
            }
            cout << row << endl;
        }
    }
}

bool checkWallInPath(int move, int playerCount, map<int, vector<vector<int>>>& playerWallsCords, int target, int boardSize) {
    vector<int> moveFrontWall = {target + boardSize, target + 1, target - boardSize, target - 1};

    for (int i = 1; i <= playerCount; i++) {
        for (auto& wallCords : playerWallsCords[i]) {
            for (auto wallCord : wallCords) {
                if (wallCord == moveFrontWall[move]) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool isValidPlayerMove(map<int, int>& playerCords, int player, int move, int boardSize, set<int>& board, const vector<int>& moveDeltas, int playerCount, map<int, vector<vector<int>>>& playerWallsCords) {
    int target = playerCords[player] + moveDeltas[move];
    bool isWallInPath = checkWallInPath(move, playerCount, playerWallsCords, target, boardSize);
    
    if (move == 1 || move == 3) {
        bool sameRow = (playerCords[player] / boardSize) == (target / boardSize);
        return board.count(target) && sameRow && !isWallInPath;
    }
    return board.count(target) && !isWallInPath;
}

bool isValidPlayerJump(map<int, int>& playerCords, int player, int move, int boardSize, int playerCount, map<int, vector<vector<int>>>& playerWallsCords) {
    vector<vector<int>> moveChecks = {{-boardSize, -3 * boardSize}, {-1, -3}, {boardSize, 3 * boardSize}, {1, 3}};
    
    for (auto check : moveChecks[move]) {
        int targetCord = playerCords[player] + check;
        for (int i = 1; i <= playerCount; i++) {
            for (auto& wallCords : playerWallsCords[i]) {
                if (find(wallCords.begin(), wallCords.end(), targetCord) != wallCords.end()) {
                    return false;
                }
            }
        }
    }
    return true;
}

int getJumpPosition(map<int, int>& playerCords, int player, int move, int boardSize, set<int>& board, const vector<int>& moveDeltas) {
    int jumpPosition = playerCords[player] + 2 * moveDeltas[move];
    if (move == 1 || move == 3) {
        bool sameRow = (playerCords[player] / boardSize) == (jumpPosition / boardSize);
        return (board.count(jumpPosition) && sameRow) ? jumpPosition : -1;
    }
    return board.count(jumpPosition) ? jumpPosition : -1;
}

vector<int> getValidMoves(map<int, int>& playerCords, int player, int boardSize, set<int>& board, const vector<int>& moveDeltas, int playerCount, map<int, vector<vector<int>>>& playerWallsCords) {
    vector<int> validMoves;
    for (int move = 0; move < moveDeltas.size(); ++move) {
        int delta = moveDeltas[move];
        int target = playerCords[player] + delta;
        if (find_if(playerCords.begin(), playerCords.end(), [target](auto& p) { return p.second == target; }) != playerCords.end()) {
            int jumpPosition = getJumpPosition(playerCords, player, move, boardSize, board, moveDeltas);
            if (jumpPosition != -1 && find_if(playerCords.begin(), playerCords.end(), [jumpPosition](auto& p) { return p.second == jumpPosition; }) == playerCords.end()) {
                if (isValidPlayerJump(playerCords, player, move, boardSize, playerCount, playerWallsCords)) {
                    validMoves.push_back(move);
                }
            }
        } else if (isValidPlayerMove(playerCords, player, move, boardSize, board, moveDeltas, playerCount, playerWallsCords)) {
            validMoves.push_back(move);
        }
    }
    return validMoves;
}

int checkWinner(map<int, int>& playerCords, map<int, vector<int>>& playerGoals) {
    for (auto& player : playerCords) {
        if (find(playerGoals[player.first].begin(), playerGoals[player.first].end(), player.second) != playerGoals[player.first].end()) {
            return player.first;
        }
    }
    return -1;
}

bool isWithinBoard(vector<int>& cords, int boardSize) {
    for (auto cord : cords) {
        if (cord < 0 || cord >= boardSize * boardSize) {
            return false;
        }
    }
    return true;
}

pair<bool, vector<int>> wallMoveValidation(int move, map<int, vector<vector<int>>>& playerWallsCords, int boardSize) {
    vector<int> nextMove = {-boardSize * 2, -2, boardSize * 2, 2};

    int delta = nextMove[move];
    vector<int> X_cords = playerWallsCords[0][0];
    vector<int> targetCords = {X_cords[0] + delta, X_cords[1] + delta, X_cords[2] + delta};
    
    bool isValid;
    if (move == 1 || move == 3) {
        isValid = (X_cords[1] / boardSize) == (targetCords[1] / boardSize);
    } else {
        isValid = isWithinBoard(targetCords, boardSize);
    }
    return make_pair(isValid, targetCords);
}

bool checkXOnWall(map<int, vector<vector<int>>>& playerWallsCords, int playerCount) {
    vector<int> X_cords = playerWallsCords[0][0];
    for (auto cord : X_cords) {
        for (int i = 1; i <= playerCount; i++) {
            for (auto& wallCords : playerWallsCords[i]) {
                if (find(wallCords.begin(), wallCords.end(), cord) != wallCords.end()) {
                    return true;
                }
            }
        }
    }
    return false;
}

int wasdToInt(char word) {
    for (int i = 0; i < wasd.size(); i++) {
        if (word == wasd[i]) {
            return i;
        }
    }
    return -1;
}

char intToWasd(int move) {
    return wasd[move];
}

pair<int, int> convertToXY(int boardSize, int position) {
    return make_pair(position % boardSize, position / boardSize);
}

bool isWithinBoard2(int x, int y, int boardSize) {
    return 0 <= x && x < boardSize && 0 <= y && y < boardSize;
}

bool isWallBetween(int x1, int y1, int x2, int y2, map<int, vector<vector<int>>>& playerWallsCords, int boardSize) {
    int wall_x, wall_y;
    if (abs(x1 - x2) == 2 && y1 == y2) {
        wall_x = min(x1, x2) + 1;
        wall_y = y1;
    } else if (abs(y1 - y2) == 2 && x1 == x2) {
        wall_x = x1;
        wall_y = min(y1, y2) + 1;
    } else {
        return false;
    }

    int wall_pos = wall_y * boardSize + wall_x;
    for (auto& walls : playerWallsCords) {
        for (auto& wall : walls.second) {
            if (find(wall.begin(), wall.end(), wall_pos) != wall.end()) {
                return true;
            }
        }
    }
    return false;
}

vector<pair<int, int>> getNeighbors(int x, int y, int boardSize, vector<int>& allPlayersPlaces, map<int, vector<vector<int>>>& playerWallsCords) {
    vector<pair<int, int>> neighbors;
    vector<pair<int, int>> deltas = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
    for (auto& delta : deltas) {
        int nx = x + delta.first;
        int ny = y + delta.second;
        if (isWithinBoard2(nx, ny, boardSize)) {
            int neighbor_pos = ny * boardSize + nx;
            if (find(allPlayersPlaces.begin(), allPlayersPlaces.end(), neighbor_pos) != allPlayersPlaces.end() && 
                !isWallBetween(x, y, nx, ny, playerWallsCords, boardSize)) {
                neighbors.push_back(make_pair(nx, ny));
            }
        }
    }
    return neighbors;
}

bool bfs(pair<int, int> start, vector<pair<int, int>>& goals, int boardSize, vector<int>& allPlayersPlaces, map<int, vector<vector<int>>>& playerWallsCords) {
    deque<pair<int, int>> queue = {start};
    set<pair<int, int>> visited = {start};
    while (!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop_front();
        if (find(goals.begin(), goals.end(), make_pair(x, y)) != goals.end()) {
            return true;
        }
        for (auto& neighbor : getNeighbors(x, y, boardSize, allPlayersPlaces, playerWallsCords)) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                queue.push_back(neighbor);
            }
        }
    }
    return false;
}

bool isPathAvailable(int boardSize, map<int, int>& playerCords, map<int, vector<int>>& playerGoals, map<int, vector<vector<int>>>& playerWallsCords) {
    vector<int> allPlayersPlaces;
    for (int i = 0; i < boardSize; i += 2) {
        for (int j = 0; j < boardSize; j += 2) {
            allPlayersPlaces.push_back(j + i * boardSize);
        }
    }

    for (auto& player : playerCords) {
        auto [startX, startY] = convertToXY(boardSize, player.second);
        vector<pair<int, int>> goals;
        for (auto goal : playerGoals[player.first]) {
            goals.push_back(convertToXY(boardSize, goal));
        }
        if (!bfs(make_pair(startX, startY), goals, boardSize, allPlayersPlaces, playerWallsCords)) {
            return false;
        }
    }
    return true;
}

int main() {
    int playerCount = 0;
    while (true) {
        cout << "Enter number of players (2 or 4): ";
        cin >> playerCount;
        if (playerCount == 2 || playerCount == 4) {
            break;
        } else {
            cout << "Invalid input. Please enter 2 or 4." << endl;
        }
    }

    int boardSize = 17;
    set<int> board;
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            board.insert(j + i * boardSize);
        }
    }

    map<int, int> playerCords;
    map<int, vector<int>> playerGoals;

    if (playerCount >= 2) {
        playerCords[1] = boardSize / 2;
        for (int i = boardSize * (boardSize - 1); i < boardSize * boardSize; i += 2) {
            playerGoals[1].push_back(i);
        }

        playerCords[2] = (boardSize * boardSize) - (boardSize / 2) - 1;
        for (int i = 0; i < boardSize; i += 2) {
            playerGoals[2].push_back(i);
        }
    }

    if (playerCount == 4) {
        playerCords[3] = boardSize * (boardSize / 2);
        for (int i = 0; i < boardSize; ++i) {
            playerGoals[3].push_back(boardSize - 1 + i * boardSize);
        }

        playerCords[4] = boardSize * (boardSize / 2) + boardSize - 1;
        for (int i = 0; i < boardSize; ++i) {
            playerGoals[4].push_back(i * boardSize);
        }
    }

    vector<int> moveDeltas = {-boardSize * 2, -2, boardSize * 2, 2};

    int allWalls = 20;
    map<int, int> playerWalls;
    map<int, vector<vector<int>>> playerWallsCords;
    for (auto& player : playerCords) {
        playerWalls[player.first] = allWalls / playerCount;
        playerWallsCords[player.first] = {};
    }

    int currentPlayer = 1;

    while (true) {
        printBoard(boardSize, playerCords, playerWallsCords);
        int winner = checkWinner(playerCords, playerGoals);
        if (winner != -1) {
            cout << "Player " << winner << " wins!" << endl;
            cout << "Press Enter to exit...";
            cin.ignore();
            cin.get();
            return 0;
        }

        vector<int> validMoves = getValidMoves(playerCords, currentPlayer, boardSize, board, moveDeltas, playerCount, playerWallsCords);
        while (true) {
            cout << "Player " << currentPlayer << "\n1. Move\n2. Add wall\n3. Exit\n";
            string gameOptions;
            cin >> gameOptions;
            clearConsole();
            printBoard(boardSize, playerCords, playerWallsCords);
            if (gameOptions == "1") {
                if (!validMoves.empty()) {
                    vector<char> validMovesWasd;
                    for (auto move : validMoves) {
                        validMovesWasd.push_back(intToWasd(move));
                    }
                    cout << "Player " << currentPlayer << ", enter your move (";
                    for (auto move : validMovesWasd) {
                        cout << move << "/";
                    }
                    cout << "): ";
                    char playerMove;
                    cin >> playerMove;
                    int playerMoveInt = wasdToInt(playerMove);
                    if (find(validMoves.begin(), validMoves.end(), playerMoveInt) != validMoves.end()) {
                        int target = playerCords[currentPlayer] + moveDeltas[playerMoveInt];
                        if (find_if(playerCords.begin(), playerCords.end(), [target](auto& p) { return p.second == target; }) != playerCords.end()) {
                            int jumpPosition = getJumpPosition(playerCords, currentPlayer, playerMoveInt, boardSize, board, moveDeltas);
                            if (jumpPosition != -1 && find_if(playerCords.begin(), playerCords.end(), [jumpPosition](auto& p) { return p.second == jumpPosition; }) == playerCords.end()) {
                                playerCords[currentPlayer] = jumpPosition;
                                clearConsole();
                                break;
                            }
                        } else {
                            playerCords[currentPlayer] = target;
                            clearConsole();
                            break;
                        }
                    } else {
                        cout << "Invalid move. Please choose one of ";
                        for (auto move : validMovesWasd) {
                            cout << move << ",";
                        }
                        cout << "." << endl;
                    }
                } else {
                    cout << "No valid moves available." << endl;
                    break;
                }
            } else if (gameOptions == "2") {
                bool wallAdded = false;
                if (playerWalls[currentPlayer] > 0) {
                    int wallStartPos = ((boardSize * boardSize) / 2) - boardSize;
                    playerWallsCords[0] = {{wallStartPos, wallStartPos + 1, wallStartPos + 2}};
                    while (!wallAdded) {
                        clearConsole();
                        printBoard(boardSize, playerCords, playerWallsCords);
                        cout << "Player " << currentPlayer << ", enter wall move (w/a/s/d) or place (p) or vertical/horizontal (o): ";
                        string wallMove;
                        cin >> wallMove;

                        if (find(wasd.begin(), wasd.end(), wallMove[0]) != wasd.end()) {
                            int wallMoveInt = wasdToInt(wallMove[0]);
                            auto [isValid, targetCords] = wallMoveValidation(wallMoveInt, playerWallsCords, boardSize);

                            if (isValid) {
                                playerWallsCords[0][0] = targetCords;
                            }

                        } else if (wallMove == "o") {
                            auto& X_cords = playerWallsCords[0][0];
                            if (X_cords[0] != X_cords[1] - 1) {
                                playerWallsCords[0] = {{X_cords[1] - 1, X_cords[1], X_cords[1] + 1}};
                            } else {
                                playerWallsCords[0] = {{X_cords[1] - boardSize, X_cords[1], X_cords[1] + boardSize}};
                            }

                        } else if (wallMove == "p") {
                            bool isInvalid = checkXOnWall(playerWallsCords, playerCount);

                            if (!isInvalid && isPathAvailable(boardSize, playerCords, playerGoals, playerWallsCords)) {
                                playerWallsCords[currentPlayer].push_back(playerWallsCords[0][0]);
                                playerWalls[currentPlayer]--;
                                playerWallsCords[0][0].clear();
                                wallAdded = true;
                                break;
                            }

                        } else {
                            cout << "Invalid input. Please enter a valid move." << endl;
                        }
                    }

                    if (wallAdded) {
                        clearConsole();
                        break;
                    }

                } else {
                    cout << "No more walls available." << endl;
                }

            } else if (gameOptions == "3") {
                cout << "Goodbye!" << endl;
                cout << "Press Enter to exit...";
                cin.ignore();
                cin.get();
                return 0;
            } else {
                cout << "Invalid input. Please enter 1, 2, or 3." << endl;
            }
        }

        currentPlayer++;
        if (currentPlayer > playerCount) {
            currentPlayer = 1;
        }
    }

    return 0;
}
