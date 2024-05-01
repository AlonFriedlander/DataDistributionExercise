#pragma once

#include <WinSock2.h> // For sockaddr_in
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")


class SendingInfo {
public:
    SendingInfo(int port);
    const sockaddr_in& getAddress() const;
private:
    sockaddr_in address;
};
