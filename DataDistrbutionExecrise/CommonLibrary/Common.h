#pragma once
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Link against Ws2_32.lib

class CommonSocketFunctions {
public:
    static SOCKET createUdpSocket(bool isMulticast);
    static void allowMultipleSocket(SOCKET socket);
    static void setSocketOptions(SOCKET socket, bool reuseAddress, DWORD receiveTimeout);

    static sockaddr_in setUpAddressStructure(const std::string& ipAddress, int port);
    static sockaddr_in setUpUnicastAddressStructure(int port);
    static void bindSocket(SOCKET socket, const sockaddr_in& address);
    static void joinMulticastGroup(SOCKET socket, const std::string& multicastGroup);
};
