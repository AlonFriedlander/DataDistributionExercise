#include "SendingInfo.h"

SendingInfo::SendingInfo(int port, const std::string& ipAddress) {
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr); // Convert IP address from string to byte array
}

const sockaddr_in& SendingInfo::getAddress() const {
    return address;
}
