#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Define the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6379); // Port 6379
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        close(sock);
        return 1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Connection Failed" << endl;
        close(sock);
        return 1;
    }

    // The message to send (using proper escape sequences for CR and LF)
    const char *message = "*2\r\n$4\r\nECHO\r\n$9\r\nraspberry\r\n";

    // Send the message
    ssize_t sentBytes = send(sock, message, strlen(message), 0);
    if (sentBytes < 0) {
        cerr << "Error sending message" << endl;
        close(sock);
        return 1;
    }
    cout << "Sent " << sentBytes << " bytes" << endl;

    // Receive the response
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t recvBytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (recvBytes < 0) {
        cerr << "Error receiving message" << endl;
    } else if (recvBytes == 0) {
        cout << "Server closed the connection" << endl;
    } else {
        cout << "Received: " << buffer << endl;
    }

    // Clean up
    close(sock);
    return 0;
}