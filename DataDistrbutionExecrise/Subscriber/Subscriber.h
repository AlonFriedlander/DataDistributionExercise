#pragma once

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <thread>
#include "CommonLibrary\Common.h"
//#include "../../../external/argparse/include/argparse/argparse.hpp"
#include "../../../external/json/include/nlohmann/json.hpp"

// Include Winsock headers for Windows socket programming
#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Link against Ws2_32.lib

class Subscriber {
public:
    enum class ShapeType {
        CIRCLE,
        SQUARE
    };

    Subscriber();
    ~Subscriber();
    void stopPublishing();

private:
    void registerToPublisher();
    void receiveUnicastData();
    void createSockets();
    std::string serializeToJson() const;
    void loadConfigurationFromJson(); 


    SOCKET sendSocketDescriptor;
    SOCKET unicastSocket;
    std::string subscriberName;
    int portNumber;
    std::string multicastSendingGroup;
    int multicastSendingPort;
    bool flag = true;
    bool running;
    std::set<std::string> subscribedShapes;
    std::vector<std::string> attributes;
    nlohmann::json jsonConfig;
    sockaddr_in multicastSendingAddr;
};
