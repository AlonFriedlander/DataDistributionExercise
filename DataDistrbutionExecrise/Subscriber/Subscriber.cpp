#include "Subscriber.h"


Subscriber::Subscriber() : running(true) {

    // Initialize Winsock
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        throw std::runtime_error("Error initializing Winsock");
    }

    loadConfigurationFromJson();

    createSockets();

    std::thread receiveThread(&Subscriber::receiveUnicastData, this);
    std::thread receiveThread2(&Subscriber::registerToPublisher, this);

    receiveThread.detach();
    receiveThread2.detach();
}


Subscriber::~Subscriber() {
    // Clean up resources
    stopPublishing();
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
    CommonSocketFunctions::setSocketOptions(sendSocketDescriptor, true, 0);
    multicastSendingAddr = CommonSocketFunctions::setUpAddressStructure(multicastSendingGroup, multicastSendingPort);
    
    // Create a socket for receiving unicast UDP
    unicastSocket = CommonSocketFunctions::createUdpSocket(false);
    sockaddr_in serverHint = CommonSocketFunctions::setUpUnicastAddressStructure(portNumber);
    CommonSocketFunctions::bindSocket(unicastSocket, serverHint);
}


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

        std::this_thread::sleep_for(std::chrono::seconds(jsonConfig["registrationFrequency"]));
    }
}


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


void Subscriber::loadConfigurationFromJson() {
    // Open the JSON file
    std::ifstream file("../../Configuration/Config.json");

    if (!file.is_open()) {
        std::cerr << "Error opening JSON file." << std::endl;
        return;
    }

    jsonConfig = nlohmann::json::parse(file);

    // Extract parameters
    subscriberName = jsonConfig["subscriberName"];
    portNumber = jsonConfig["portNumber"];
    subscribedShapes = jsonConfig["shapeType"];
    attributes = jsonConfig["attributes"];
    multicastSendingGroup = jsonConfig["multicastSendingGroup"];
    multicastSendingPort = jsonConfig["multicastSendingPort"];
}
