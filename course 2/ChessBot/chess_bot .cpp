#include <vector>
#include <stdlib.h>
#include <string>
#include <map>
#include <random>
#include <iostream>

using namespace std;


bool destination_is_on_board(int x1, int delta_x, int delta_y) {
    return ((0 <= (x1 + delta_x + delta_y * 8)) && ((x1 + delta_x + delta_y * 8) < 64) && (0 <= (x1 % 8 + delta_x)) &&
            ((x1 % 8 + delta_x) < 8));
}

#include <vector>
#include <algorithm>
#include <random>


using namespace std;

bool cmp(pair<vector<vector<int>>, int> a, pair<vector<vector<int>>, int> b) {
    return a.second < b.second;
}

void printFiguresInfo() {
    cout << "King - король" << endl;
    cout << "Queen - королева" << endl;
    cout << "Bishop - слон" << endl;
    cout << "Rook - ладья" << endl;
    cout << "Pawn - пешка" << endl;
    cout << "Knight - конь" << endl;
}

class Chess {
    vector<int> start_board;
    vector<int> bestmoves;
    int best_try = -1;
    int totalMoves = 4;
    int P, B, N, R, wP, wB, wN, wR;

public:
    vector<int> board = vector<int>(64, 0);

    Chess(int P1, int B1, int N1, int R1, int wP1, int wB1, int wN1, int wR1) : P(P1), B(B1), N(N1), R(R1), wP(wP1),
                                                                                wB(wB1), wN(wN1),
                                                                                wR(wR1) { this->createBoard(); }

    void createBoard() {
    board = vector<int>(64, 0);
        for (int i = 0; i < wP; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 1;
            }
        }

        for (int i = 0; i < wB; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 2;
            }
        }

        for (int i = 0; i < wR; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 3;
            }
        }

        for (int i = 0; i < wN; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 4;
            }
        }

        auto it = find(board.begin(), board.end(), 0);
        if (it != board.end()) {
            *it = 10;
        }

        for (int i = 0; i < P; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 100;
            }
        }

        for (int i = 0; i < B; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 301;
            }
        }

        for (int i = 0; i < R; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 500;
            }
        }

        for (int i = 0; i < N; ++i) {
            auto it = find(board.begin(), board.end(), 0);
            if (it != board.end()) {
                *it = 300;
            }
        }

        it = find(board.begin(), board.end(), 0);
        if (it != board.end()) {
            *it = 100000000;
        }

        random_device rd;
        mt19937 g(rd());
        shuffle(board.begin(), board.end(), g);

        start_board = board;
    }

    void printChessboard() {
        cout << "  a b c d e f g h" << endl;
        cout << "  ---------------" << endl;

        for (int row = 0; row < 8; ++row) {
            cout << 8 - row << "|";
            for (int col = 0; col < 8; ++col) {
                int index = row * 8 + col;
                int piece = board[index];

                char symbol = ' ';

                switch (piece) {
                    case 1:
                        symbol = 'P';
                        break;
                    case 2:
                        symbol = 'B';
                        break;
                    case 3:
                        symbol = 'R';
                        break;
                    case 4:
                        symbol = 'N';
                        break;
                    case 10:
                        symbol = 'K';
                        break;
                    case 100000000:
                        symbol = 'k';
                        break;
                    case 100:
                        symbol = 'p';
                        break;
                    case 301:
                        symbol = 'b';
                        break;
                    case 500:
                        symbol = 'r';
                        break;
                    case 300:
                        symbol = 'n';
                        break;
                }

                cout << symbol << '|';
            }
            cout << " " << 8 - row << endl;
        }

        cout << "  ---------------" << endl;
        cout << "  a b c d e f g h" << endl;
    }


    void domove(int move) {
        board[move % 100] = board[move / 100];
        board[move / 100] = 0;
    }

    std::pair<char, int> indexToChessCoordinates(int index) {

        if (index < 0 || index >= 64) {
            throw std::out_of_range("Недопустимый индекс");
        }


        char letter = 'a' + (index % 8);


        int number = 8 - (index / 8);

        return std::make_pair(letter, number);
    }
    vector<pair<vector<vector<int>>, int>>
            bestMoveSequence;


    void findMoves(int remainingMoves, vector<int> hist, vector<int> b) {
        int totalValue = accumulate(b.begin(), b.end(), 0);
        if (totalValue < 1000000) {
            totalValue = -totalValue - pow(10000, remainingMoves);
            bestMoveSequence.push_back({{hist}, totalValue});
            best_try = remainingMoves;
            return;
        }
        if (remainingMoves < best_try) return;
        if (remainingMoves == 0) {
            bestMoveSequence.push_back({{hist}, totalValue});
            return;
        }
        for (int i = 0; i < 64; i++) {
            if (b[i] == 1) {
                Pawn(i, remainingMoves, hist, b);
            } else if (b[i] == 2) {
                Bishop(i, remainingMoves, hist, b);
            } else if (b[i] == 3) {
                Rook(i, remainingMoves, hist, b);
            } else if (b[i] == 4) {
                Knight(i, remainingMoves, hist, b);
            } else if (b[i] == 10) {
                King(i, remainingMoves, hist, b);
            }
        }
    }


    void printBestMoveSequence() {
        vector<int> bbb;
        findMoves(this->totalMoves, bbb, this->board);
        printChessboard();
        std::sort(bestMoveSequence.begin(), bestMoveSequence.end(), cmp);
        vector<vector<int>> bMS = bestMoveSequence[0].first;
        if (bMS[0].size() == 1){
            this->createBoard();
            printBestMoveSequence();
            return;
        }
        for (const vector<int> &move: bMS) {
            for (int element: move) {
                domove(element);
                cout << indexToChessCoordinates(element / 100).first
                     << indexToChessCoordinates(element / 100).second
                     << "-->" << indexToChessCoordinates(element % 100).first
                     << indexToChessCoordinates(element % 100).second << " " << " " << endl;
                printChessboard();
            }
            cout << "------------" << endl;
            board = start_board;
        }
    }

    void try_to_move(int cord, int destination, int move, vector<int> hist, vector<int> b) {
        if ((b)[destination] > 99 or (b)[destination] == 0) {
            vector<int> hist1 = hist;
            hist1.push_back(cord * 100 + destination);
            vector<int> b1 = b;
            b1[destination] = b1[cord];
            b1[cord] = 0;
            findMoves(move - 1, hist1, b1);
        }
    }

    void Rook(int cord, int move, vector<int> hist, vector<int> b) {
        for (int i = cord + 1; destination_is_on_board(i - 1, 1, 0); i++) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;
        }
        for (int i = cord + 8; destination_is_on_board(i - 8, 0, 1); i += 8) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;
        }
        for (int i = cord - 8; destination_is_on_board(i + 8, 0, -1); i -= 8) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;
        }
        for (int i = cord - 1; destination_is_on_board(i + 1, -1, 0); i--) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;
        }
    }

    void Bishop(int cord, int move, vector<int> hist, vector<int> b) {
        int dest = 0;
        for (int i = cord + 9; destination_is_on_board(i - 9, 1, 1); i += 9) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;
        }
        for (int i = cord - 7; destination_is_on_board(i + 7, 1, -1); i -= 7) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;

        }
        for (int i = cord - 9; destination_is_on_board(i + 9, -1, -1); i -= 9) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;
        }
        for (int i = cord + 7; destination_is_on_board(i - 7, -1, 1); i += 7) {
            this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            if (b[i] != 0) break;
        }

    }

    void Pawn(int cord, int move, vector<int> hist, vector<int> b) {
        if (destination_is_on_board(cord, 0, 1)) {
            int i = cord + 0;
            if (b[i] > 99) {
                this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            }

        }
        if (destination_is_on_board(cord, 1, 1)) {
            int i = cord + 9;
            if (b[i] > 99) {
                this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            }

        }
        if (destination_is_on_board(cord, -1, 1)) {
            int i = cord + 7;
            if (b[i] > 99) {
                this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            }
        }
    }

    void Knight(int cord, int move, vector<int> hist, vector<int> b) {
        int dest[] = {-1, 2, -1, -2, 1, 2, 1, -2, 1, -2, 1, 2, -1, -2, -1, 2};
        for (int j = 0; j < 16; j += 2) {
            if (destination_is_on_board(cord, dest[j], dest[j + 1])) {
                int i = cord + dest[j] + dest[j + 1] * 8;
                this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            }
        }
    }

    void King(int cord, int move, vector<int> hist, vector<int> b) {
        int dest[] = {-1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1};
        for (int j = 0; j < 16; j += 2) {
            if (destination_is_on_board(cord, dest[j], dest[j + 1])) {
                int i = cord + dest[j] + dest[j + 1] * 8;
                this->try_to_move(cord = cord, i, move = move, hist = hist, b = b);
            }
        }
    }

};

int main() {
    printFiguresInfo();
    Chess party(2, 2, 2, 2, 2, 2, 2, 2);
    party.printBestMoveSequence();
    return 0;
}
