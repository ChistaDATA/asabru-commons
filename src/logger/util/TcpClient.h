#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUFFER_SIZE = 1024;

class TcpClient
{
public:
    TcpClient(const char *domain, int port)
    {
        // Create a socket
        _clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (_clientSocket == -1)
        {
            std::cerr << "Error creating socket" << std::endl;
            throw std::runtime_error("Error creating socket");
        }

        // Specify the server address and port
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        inet_pton(AF_INET, domain, &(serverAddress.sin_addr));

        // Connect to the server
        if (connect(_clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            std::cerr << "Error connecting to the server" << std::endl;
            close(_clientSocket);
            throw std::runtime_error("Error connecting to the server");
        }
    }

    ~TcpClient() {
        // Close the socket
        close(_clientSocket);
    }

    bool sendTcpMessage(const char *content)
    {
        // Send a message to the server
        ssize_t bytesSent = send(_clientSocket, content, strlen(content), 0);
        if (bytesSent == -1)
        {
            std::cerr << "Error sending data to the server" << std::endl;
            // Close the socket
            close(_clientSocket);
        }

        return true;
    }

private:
    int _clientSocket;
};
