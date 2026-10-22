#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << "Usage: ./client kx ky rx ry bx by" << endl;
        return 1;
    }

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddress,
                sizeof(serverAddress)) < 0) {
        cerr << "Connection failed" << endl;
        close(clientSocket);
        return 1;
    }

    string request;

    for (int i = 1; i < argc; i++) {
        if (i > 1) request += " ";
        request += argv[i];
    }

    send(clientSocket, request.c_str(), request.size(), 0);

    char buffer[1024] = {};
    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytes > 0) {
        cout << "Server response: "
             << string(buffer, bytes) << endl;
    } else {
        cerr << "No response from server" << endl;
    }

    close(clientSocket);
    return 0;
}