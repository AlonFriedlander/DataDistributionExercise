#include "Subscriber.h"

std::string multicastSendingGroup = "234.5.6.7";
int multicastSendingPort = 8910;

Subscriber::Subscriber(const std::vector<std::string>& args) : running(true) {
    // Open the JSON file
    //std::ifstream file("../Configuration/SubscriberConfig.json")

    //    if (!file.is_open()) {
    //        std::cerr << "Error opening JSON file." << std::endl;
    //        return 1;
    //    }

    //jsonConfig = nlohmann::json::parse(file);



    subscriberName = args[0];
    portNumber = std::stoi(args[1]);

    // Parse shape types
    parseShapes(args[2]);

    // Create attribute list
    createAttributes(args);

    // Initialize Winsock
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        throw std::runtime_error("Error initializing Winsock");
    }

    createSockets();

    std::thread receiveThread(&Subscriber::receiveUnicastData, this);
    std::thread receiveThread2(&Subscriber::registerToPublisher, this);

    receiveThread.detach();
    receiveThread2.detach();
}

Subscriber::~Subscriber() {
    // Clean up resources
    closesocket(sendSocketDescriptor);
    closesocket(recvSocketDescriptor);
    closesocket(unicastSocket);
    WSACleanup();
}

// Stops the subscriber 
void Subscriber::stopPublishing() {
    std::cout << "stop subscribing" << std::endl;
    running = false;
    closesocket(sendSocketDescriptor);
    closesocket(unicastSocket);
}

void Subscriber::createSockets() {

    sendSocketDescriptor = CommonSocketFunctions::createUdpSocket(true);
    ////Create a UDP socket for sending multicast
    //sendSocketDescriptor = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    //if (sendSocketDescriptor == INVALID_SOCKET) {
    //    WSACleanup();
    //    throw std::runtime_error("Error creating socket");
    //}

    CommonSocketFunctions::setSocketOptions(sendSocketDescriptor, true, 0);
    //CommonSocketFunctions::allowMultipleSocket(sendSocketDescriptor);

    // Allow multiple sockets to use the same PORT number
    //int optval = 1;
    //if ((setsockopt(sendSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval))) < 0)
    //{
    //    closesocket(sendSocketDescriptor);
    //    WSACleanup();
    //    throw std::runtime_error("Error setting socket options");
    //}

    multicastSendingAddr = CommonSocketFunctions::setUpAddressStructure(multicastSendingGroup, multicastSendingPort);

    //multicastSendingAddr;
    //memset(&multicastSendingAddr, 0, sizeof(multicastSendingAddr));
    //multicastSendingAddr.sin_family = AF_INET;
    //inet_pton(AF_INET, (PCSTR)(multicastSendingGroup.c_str()), &multicastSendingAddr.sin_addr.s_addr);
    //multicastSendingAddr.sin_port = htons(multicastSendingPort);

    //................................................
    //// Create UDP socket for recive multicast
    //if ((recvSocketDescriptor = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0)) == INVALID_SOCKET) {
    //    WSACleanup();
    //    throw std::runtime_error("Error creating socket");
    //}

    //// Allow multiple sockets to use the same PORT number
    //if (setsockopt(recvSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&optval), sizeof(optval)) == SOCKET_ERROR) {
    //    closesocket(recvSocketDescriptor);
    //    WSACleanup();
    //    throw std::runtime_error("Error setting socket options");
    //}

    //// Set up the address structure for binding
    //sockaddr_in multicastReceivingAddr;
    //memset(&multicastReceivingAddr, 0, sizeof(multicastReceivingAddr));
    //multicastReceivingAddr.sin_family = AF_INET;
    //multicastReceivingAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //multicastReceivingAddr.sin_port = htons(multicastReceivingPort);

    //// Bind the socket to the specified port
    //if (bind(recvSocketDescriptor, reinterpret_cast<const sockaddr*>(&multicastReceivingAddr), sizeof(multicastReceivingAddr)) == SOCKET_ERROR) {
    //    closesocket(recvSocketDescriptor);
    //    WSACleanup();
    //    throw std::runtime_error("Error binding socket");
    //}

    //// Join the multicast group
    //ip_mreq multicastRequest;
    //inet_pton(AF_INET, (PCSTR)(multicastReceivingGroup.c_str()), &multicastRequest.imr_multiaddr.s_addr);
    //multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    //if (setsockopt(recvSocketDescriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<const char*>(&multicastRequest), sizeof(multicastRequest)) == SOCKET_ERROR) {
    //    closesocket(recvSocketDescriptor);
    //    WSACleanup();
    //    throw std::runtime_error("Error joining multicast group");
    //}

    //................................................
    // Create a socket for receiving unicast UDP
    unicastSocket = CommonSocketFunctions::createUdpSocket(false);

    //unicastSocket = socket(AF_INET, SOCK_DGRAM, 0);
    //if (unicastSocket == INVALID_SOCKET) {
    //    std::cerr << "Error creating unicast socket: " << WSAGetLastError() << std::endl;
    //    return;
    //}


    sockaddr_in serverHint = CommonSocketFunctions::setUpUnicastAddressStructure(portNumber);

    // Create a server hint structure for the server
    //sockaddr_in serverHint;
    //serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
    //serverHint.sin_family = AF_INET; // Address format is IPv4
    //serverHint.sin_port = htons(portNumber); // Convert from little to big endian

    CommonSocketFunctions::bindSocket(unicastSocket, serverHint);

    //Try and bind the socket to the IP and port
    //if (bind(unicastSocket, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
    //{
    //    std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
    //    return;
    //}

}

void Subscriber::parseShapes(const std::string& shapeType) {
    std::string shape;
    for (char c : shapeType) {
        if (c == ':') {
            // Add extracted shape to the set
            subscribedShapes.insert(shape);
            shape.clear();
        }
        else {
            shape.push_back(c);
        }
    }
    // Add the last shape to the set
    subscribedShapes.insert(shape);
}

void Subscriber::createAttributes(const std::vector<std::string>& args) {
    for (size_t i = 3; i < args.size(); ++i) {
        attributes.push_back(args[i]);
    }
}

//void Subscriber::subscribe(ShapeType shapeType, const std::string& publisherAddress) {
//    flag = true;
//
//    // Start the thread to receive data
//    std::thread receiveThread(&Subscriber::registerToPublisher, this);
//    receiveThread.detach();
//
//    // Create the message string in the format "SHAPE_TYPE:PORT_NUMBER"
//    std::string data = shapeTypeToString(shapeType) + ":" + std::to_string(portNumber);
//
//    // Loop until approved
//    while (flag) {
//        std::cout << "sending multicast" << std::endl;
//        int iResult = sendto(sendSocketDescriptor, data.c_str(), data.size(), 0, reinterpret_cast<const sockaddr*>(&multicastSendingAddr), sizeof(multicastSendingAddr));
//
//        if (iResult == SOCKET_ERROR) {
//            std::cout << "Sendto failed with error: <" << WSAGetLastError() << ">\n";
//            closesocket(sendSocketDescriptor);
//            WSACleanup();
//        }
//
//        std::this_thread::sleep_for(std::chrono::seconds(4));
//    }
//}


//std::string Subscriber::shapeTypeToString(ShapeType shapeType) {
//    switch (shapeType) {
//    case ShapeType::CIRCLE:
//        return "CIRCLE";
//    case ShapeType::SQUARE:
//        return "SQUARE";
//    default:
//        throw std::invalid_argument("Invalid shape type");
//    }
//}


void Subscriber::registerToPublisher() {
    std::string jsonString = serializeToJson();

    while (running) {
        std::cout << "sending multicast" << std::endl;
        int iResult = sendto(sendSocketDescriptor, jsonString.c_str(), jsonString.size(), 0, reinterpret_cast<const sockaddr*>(&multicastSendingAddr), sizeof(multicastSendingAddr));

        if (iResult == SOCKET_ERROR) {
            std::cout << "Sendto failed with error: <" << WSAGetLastError() << ">\n";
            closesocket(sendSocketDescriptor);
            WSACleanup();
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

//void Subscriber::receiveUnicastData() {
//    char receiveData[1024];
//    sockaddr_in senderAddress;
//    int senderAddressSize = sizeof(senderAddress);
//
//    while (true) {
//        int bytesReceived = recvfrom(unicastSocket, receiveData, sizeof(receiveData), 0, reinterpret_cast<sockaddr*>(&senderAddress), &senderAddressSize);
//
//        if (bytesReceived != SOCKET_ERROR) {
//            // Print the received shape string
//            std::string shapeString(receiveData, bytesReceived);
//            std::cout << "subscriber: " << subscriberName << " get : " << shapeString << std::endl;
//        }
//    }
//    closesocket(unicastSocket);
//}

void Subscriber::receiveUnicastData() {
    char receiveData[1024];
    sockaddr_in senderAddress;
    int senderAddressSize = sizeof(senderAddress);

    while (running) {
        int bytesReceived = recvfrom(unicastSocket, receiveData, sizeof(receiveData), 0, reinterpret_cast<sockaddr*>(&senderAddress), &senderAddressSize);

        if (bytesReceived != SOCKET_ERROR) {
            // Parse received JSON data
            std::string jsonData(receiveData, bytesReceived);
            auto parsedData = nlohmann::json::parse(jsonData);

            // Extract and print shape type and subscriber name
            std::cout << subscriberName << " recive: " << std::endl;
            std::cout << "Shape Type: " << parsedData["shapeType"] << std::endl;

            // Extract and print additional attributes
            for (const auto& attribute : attributes) {
                // Check if the attribute exists in the parsed JSON data
                if (parsedData.find(attribute) != parsedData.end()) {
                    std::cout << attribute << ": " << parsedData[attribute] << std::endl;
                }
            }
            std::cout << "_______________________ " << std::endl;
        }
    }
    closesocket(unicastSocket);
}

std::string Subscriber::serializeToJson() const {
    // Convert set of shapes to vector
    std::vector<std::string> shapeTypesVector(subscribedShapes.begin(), subscribedShapes.end());

    // Create JSON object
    nlohmann::json jsonData;
    jsonData["portNumber"] = portNumber;
    jsonData["shapeTypes"] = shapeTypesVector;

    // Serialize JSON to string
    return jsonData.dump();
}
