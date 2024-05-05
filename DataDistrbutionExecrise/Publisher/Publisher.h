#pragma once

#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <functional>
#include <cstring>
#include <set>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <map>
#include <thread>
#include <memory>
#include <chrono>
#include <stdexcept>

#include "ThreadPool.h"
#include "CommonLibrary\Common.h"
#include "SendingInfo.h"
#include "../../../external/json/include/nlohmann/json.hpp"



// Include Winsock headers for Windows socket programming
#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Link against Ws2_32.lib

// Forward declarations
class Shape;
class ThreadPool;

namespace ShapeEnum
{
    // Define ShapeType enum
    enum class ShapeType {
        SQUARE = 2,
        CIRCLE = 3
    };

    // Define static array containing all shape types
    static const ShapeType AllTypes[] = { ShapeType::SQUARE, ShapeType::CIRCLE };
}
// Define a type alias for the subscriber shape pointer
using SubscriberShapePtr = std::shared_ptr<std::vector<SendingInfo>>;
// Define Publisher class
class Publisher {
public:
    Publisher();
    ~Publisher();

    void startPublishing();
    void stopPublishing();

private:
    using FunctionPtr = std::function<std::string()>;


    // Private member functions
    void initializeList();
    std::string shapeTypeToString(ShapeEnum::ShapeType shapeType) const;
    void eventManager();
    void sendShape(const std::string& shapeString, const SendingInfo& sendingInfo);
    void subscriberRegistrar();
    std::string generateSize();
    std::string generateCoordinates();
    std::string generateColors();
    void circleHandler();
    void squareHandler();
    void initializeFunctionMap();
    void loadConfigurationFromJson(); 
    std::chrono::milliseconds hertzToMilliseconds(int frequencyHz);
    void createSockets();
    //void circleTask(ThreadPool& pool);
    //void squareTask(ThreadPool& pool);
    //void handler(const std::string& jsonString, const SubscriberShapePtr& subscriberShapePtr);


    // Private data members
    std::vector<SendingInfo> specificTypeList;
    bool running;
    std::string multicastReceivingGroup;
    int multicastReceivingPort;
    nlohmann::json jsonConfig;
    std::chrono::milliseconds squareFrequency;
    std::chrono::milliseconds circleFrequency;
    SOCKET multicastSocket;
    SOCKET unicastSocket;
    SOCKET sendApprovedSocket;
    sockaddr_in multicastSendingAddr;
    std::vector<SubscriberShapePtr> subscribersList;
    std::map<std::string, SubscriberShapePtr> map;
    std::mutex functionMapMutex;

    // Define the map with function pointers
    //using FunctionPtr = std::function<std::string()>;
    std::map<std::string, FunctionPtr> functionMap;
    std::set<int> registeredPortNumbers; // Set to store registered port numbers
};

#endif // PUBLISHER_H

// Include cross-platform headers for socket programming
//#ifdef _WIN32
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "ws2_32.lib") // Link against with ws2_32.lib
//#else
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//#endif