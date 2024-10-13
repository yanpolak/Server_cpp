#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

class Server {
public:
    Server();
    void handleFatalError();
    void notifyOtherClients(int senderFd, const std::string &message);
    void registerNewClient(int fd);
    void removeClient(int fd);
    std::string joinStrings(const std::string &buf, const std::string &add);
    int extractMessage(std::string &buf, std::string &msg);
    void sendMessageToClients(int fd);
    int createServerSocket();
    void run(int port);

private:
    int clientCount;
    int maxFd;
    std::map<int, int> clientIds;
    std::map<int, std::string> clientMsgs;

    fd_set readFds;
    fd_set writeFds;
    fd_set allFds;
    char readBuffer[1001];
    char writeBuffer[42];
};

#endif // SERVER_HPP
