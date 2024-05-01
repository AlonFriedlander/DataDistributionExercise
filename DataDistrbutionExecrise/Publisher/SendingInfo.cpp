#include "SendingInfo.h"

SendingInfo::SendingInfo(int port) {
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr); // Convert from string to byte array
}

const sockaddr_in& SendingInfo::getAddress() const {
    return address;
}
