#include "server.hpp"

Server::Server()
    : clientCount(0), maxFd(0) 
{
    FD_ZERO(&readFds);
    FD_ZERO(&writeFds);
    FD_ZERO(&allFds);
    std::memset(readBuffer, 0, sizeof(readBuffer));
    std::memset(writeBuffer, 0, sizeof(writeBuffer));
}

void Server::handleFatalError() 
{
    write(2, "Fatal error\n", 12);
    exit(1);
}

void Server::notifyOtherClients(int senderFd, const std::string &message)
{
    for (int fd = 0; fd <= maxFd; fd++)
    {
        if (FD_ISSET(fd, &writeFds) && fd != senderFd)
        {
            send(fd, message.c_str(), message.length(), 0);
        }
    }
}

void Server::registerNewClient(int fd) 
{
    maxFd = fd > maxFd ? fd : maxFd;
    clientIds[fd] = clientCount++;
    clientMsgs[fd] = "";
    FD_SET(fd, &allFds);
    snprintf(writeBuffer, sizeof(writeBuffer), "server: client %d just arrived\n", clientIds[fd]);
    notifyOtherClients(fd, writeBuffer);
}

void Server::removeClient(int fd) {
    snprintf(writeBuffer, sizeof(writeBuffer), "server: client %d just left\n", clientIds[fd]);
    notifyOtherClients(fd, writeBuffer);
    clientMsgs.erase(fd);
    FD_CLR(fd, &allFds);
    close(fd);
}

std::string Server::joinStrings(const std::string &buf, const std::string &add) 
{
    return buf + add;
}

int Server::extractMessage(std::string &buf, std::string &msg)
{
    size_t pos = buf.find('\n');
    if (pos == std::string::npos)
    {
        return 0;
    }
    msg = buf.substr(0, pos + 1);
    buf = buf.substr(pos + 1);
    return 1;
}

void Server::sendMessageToClients(int fd)
{
    std::string msg;
    while (extractMessage(clientMsgs[fd], msg))
    {
        snprintf(writeBuffer, sizeof(writeBuffer), "client %d: ", clientIds[fd]);
        notifyOtherClients(fd, writeBuffer);
        notifyOtherClients(fd, msg);
    }
}

int Server::createServerSocket()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) 
    {
        handleFatalError();
    }
    FD_SET(serverSocket, &allFds);
    maxFd = serverSocket;
    return serverSocket;
}

void Server::run(int port) 
{
    int serverSocket = createServerSocket();

    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        handleFatalError();
    }
    if (listen(serverSocket, SOMAXCONN) < 0) 
    {
        handleFatalError();
    }

    while (1) 
    {
        readFds = writeFds = allFds;

        if (select(maxFd + 1, &readFds, &writeFds, NULL, NULL) < 0) 
        {
            handleFatalError();
        }

        for (int fd = 0; fd <= maxFd; fd++) 
        {
            if (!FD_ISSET(fd, &readFds)) 
            {
                continue;
            }

            if (fd == serverSocket) 
            {
                socklen_t addrLen = sizeof(serverAddr);
                int clientFd = accept(serverSocket, (struct sockaddr *)&serverAddr, &addrLen);
                if (clientFd >= 0) 
                {
                    registerNewClient(clientFd);
                }
            }
             else 
             {
                int bytesRead = recv(fd, readBuffer, 1000, 0);
                if (bytesRead <= 0) 
                {
                    removeClient(fd);
                } 
                else 
                {
                    readBuffer[bytesRead] = '\0';
                    clientMsgs[fd] = joinStrings(clientMsgs[fd], readBuffer);
                    sendMessageToClients(fd);
                }
            }
        }
    }
}
