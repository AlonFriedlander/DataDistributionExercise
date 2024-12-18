#include "Common.h"

void addition(int a, int b)
{
    std::cout << a + b << std::endl;
}

SOCKET CommonSocketFunctions::createUdpSocket(bool isMulticast) {
    SOCKET newSocket;
    if (isMulticast) {
        newSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
    }
    else {
        newSocket = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (newSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Error creating socket");
    }
    return newSocket;
}

void CommonSocketFunctions::setSocketOptions(SOCKET socket, bool reuseAddress, DWORD receiveTimeout) {
    if (reuseAddress) {
        int yes = 1;
        if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes), sizeof(yes)) == SOCKET_ERROR) {
            closesocket(socket);
            WSACleanup();
            throw std::runtime_error("Error setting socket options");
        }
    }

    // Set receive timeout if receiveTimeout is greater than 0
    if (receiveTimeout > 0) {
        if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&receiveTimeout), sizeof(receiveTimeout)) == SOCKET_ERROR) {
            closesocket(socket);
            WSACleanup();
            throw std::runtime_error("Error setting receive timeout");
        }
    }
}


sockaddr_in CommonSocketFunctions::setUpAddressStructure(const std::string& ipAddress, int port) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, (PCSTR)(ipAddress.c_str()), &address.sin_addr.s_addr);
    address.sin_port = htons(port);
    return address;
}

sockaddr_in CommonSocketFunctions::setUpUnicastAddressStructure(int port) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
    address.sin_port = htons(port); // Convert port to network byte order

    return address;
}

void CommonSocketFunctions::bindSocket(SOCKET sock, const sockaddr_in& address) {
    if (bind(sock, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
        return;
    }
}


void CommonSocketFunctions::joinMulticastGroup(SOCKET socket, const std::string& multicastGroup) {
    ip_mreq multicastRequest;
    inet_pton(AF_INET, (PCSTR)(multicastGroup.c_str()), &multicastRequest.imr_multiaddr.s_addr);
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<const char*>(&multicastRequest), sizeof(multicastRequest)) == SOCKET_ERROR) {
        closesocket(socket);
        WSACleanup();
        throw std::runtime_error("Error joining multicast group");
    }
}

