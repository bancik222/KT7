#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

bool valid(int x, int y) {
    return x >= 1 && x <= 8 && y >= 1 && y <= 8;
}

string checkAttack(int kx, int ky, int rx, int ry, int bx, int by) {
    if (!valid(kx, ky) || !valid(rx, ry) || !valid(bx, by)) {
        return "ERROR: Invalid coordinates";
    }

    if ((kx == rx && ky == ry) ||
        (kx == bx && ky == by) ||
        (rx == bx && ry == by)) {
        return "ERROR: Pieces overlap";
    }

    bool rook = (kx == rx || ky == ry);
    bool bishop = (abs(kx - bx) == abs(ky - by));

    // Проверяем, не перекрывает ли слон линию атаки ладьи.
    if (rook) {
        int dx = (kx > rx) - (kx < rx);
        int dy = (ky > ry) - (ky < ry);

        for (int x = rx + dx, y = ry + dy;
             x != kx || y != ky;
             x += dx, y += dy) {
            if (x == bx && y == by) {
                rook = false;
                break;
            }
        }
    }

    // Проверяем, не перекрывает ли ладья диагональ слона.
    if (bishop) {
        int dx = (kx > bx) - (kx < bx);
        int dy = (ky > by) - (ky < by);

        for (int x = bx + dx, y = by + dy;
             x != kx || y != ky;
             x += dx, y += dy) {
            if (x == rx && y == ry) {
                bishop = false;
                break;
            }
        }
    }

    if (rook && bishop) return "Rook attacks: true; Bishop attacks: true";
    if (rook) return "Rook attacks: true; Bishop attacks: false";
    if (bishop) return "Rook attacks: false; Bishop attacks: true";

    return "Rook attacks: false; Bishop attacks: false";
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(serverSocket, (sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Bind failed" << endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        cerr << "Listen failed" << endl;
        close(serverSocket);
        return 1;
    }

    cout << "Chess TCP server started on port 8080" << endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket < 0) {
            continue;
        }

        char buffer[1024] = {};
        int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytes > 0) {
            stringstream ss(string(buffer, bytes));
            int kx, ky, rx, ry, bx, by;
            string result;

            if (ss >> kx >> ky >> rx >> ry >> bx >> by) {
                string extra;
                if (ss >> extra) {
                    result = "ERROR: Expected exactly 6 coordinates";
                } else {
                    result = checkAttack(kx, ky, rx, ry, bx, by);
                }
            } else {
                result = "ERROR: Expected 6 coordinates";
            }

            send(clientSocket, result.c_str(), result.size(), 0);
            cout << "Response: " << result << endl;
        }

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}