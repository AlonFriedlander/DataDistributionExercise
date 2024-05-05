#pragma once

#include <string>
#include <WinSock2.h> // For sockaddr_in
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")


class SendingInfo {
public:
    SendingInfo(int port, const std::string& ipAddress); 

    const sockaddr_in& getAddress() const;
private:
    sockaddr_in address;
};
