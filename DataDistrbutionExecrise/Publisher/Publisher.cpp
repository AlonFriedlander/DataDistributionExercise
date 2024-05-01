#include "Publisher.h"


// Constructor
Publisher::Publisher() : running(true) {
    // Initialize Winsock
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        throw std::runtime_error("Error initializing Winsock");
    }

    loadConfigurationFromJson();

    createSockets();

    initializeFunctionMap();
    // Initialize list of subscribers
    initializeList();
}

// Destructor
Publisher::~Publisher() {
    stopPublishing();
    WSACleanup();
}

void Publisher::createSockets() {
    multicastSocket = CommonSocketFunctions::createUdpSocket(true);
    CommonSocketFunctions::setSocketOptions(multicastSocket, true, 2000);
    sockaddr_in serverAddress = CommonSocketFunctions::setUpUnicastAddressStructure(multicastReceivingPort);
    CommonSocketFunctions::bindSocket(multicastSocket, serverAddress);
    CommonSocketFunctions::joinMulticastGroup(multicastSocket, multicastReceivingGroup);

    unicastSocket = CommonSocketFunctions::createUdpSocket(false);
}

//// Starts the publishing process
void Publisher::startPublishing() {
    std::thread listenerThread(&Publisher::subscriberRegistrar, this);
    std::thread eventManagerThread(&Publisher::eventManager, this);

    listenerThread.detach();
    eventManagerThread.detach();
}

// Stops the publishing process
void Publisher::stopPublishing() {
    std::cout << "stop publishing" << std::endl;
    running = false;
    closesocket(multicastSocket);
    closesocket(unicastSocket);
}


// Internal: Initializes the list of subscribers
void Publisher::initializeList() {
    for (ShapeEnum::ShapeType shapeType : ShapeEnum::AllTypes) {
        // Create a subscriber shape object
        SubscriberShapePtr subscriberShapePtr = std::make_shared<SubscriberShape>(shapeTypeToString(shapeType), getFrequency(shapeType));

        // Add the subscriber shape object to the list
        subscribersList.push_back(subscriberShapePtr);

        // Insert a pointer to the subscriber shape object into the map
        map.insert(std::make_pair(shapeTypeToString(shapeType), subscriberShapePtr));
    }
}

void Publisher::initializeFunctionMap() {
    functionMap["size"] = std::bind(&Publisher::generateSize, this);
    functionMap["coordinates"] = std::bind(&Publisher::generateCoordinates, this);
    functionMap["colors"] = std::bind(&Publisher::generateColors, this);
}


void Publisher::eventManager() {
    // Create a thread for handling circles
    std::thread circleThread(&Publisher::circleHandler, this);
    // Create a thread for handling squares
    std::thread squareThread(&Publisher::squareHandler, this);

    circleThread.detach();
    squareThread.detach();
}

//Gets the frequency for a given shape type
int Publisher::getFrequency(ShapeEnum::ShapeType shapeType) const {
    switch (shapeType) {
    case ShapeEnum::ShapeType::SQUARE: return 2; // 2 Hz
    case ShapeEnum::ShapeType::CIRCLE: return 3; // 3 Hz
    default: return 0;
    }
}

//Converts ShapeType enum value to string
std::string Publisher::shapeTypeToString(ShapeEnum::ShapeType shapeType) const {
    switch (shapeType) {
    case ShapeEnum::ShapeType::SQUARE: return "SQUARE";
    case ShapeEnum::ShapeType::CIRCLE: return "CIRCLE";
    default: return "";
    }
}

// Function to handle circles
void Publisher::circleHandler() {
    // Create a JSON object
    nlohmann::json circleJson;

    // Add the shapeType field
    circleJson["shapeType"] = "Circle";

    while (running) {
        for (const auto& entry : functionMap) {
            circleJson[entry.first] = entry.second();// Call the function and store the result in JSON
        }

        std::string jsonString = circleJson.dump();

        auto i = map.find("CIRCLE");
        SubscriberShapePtr subscriberShapePtr = i->second;

        for (const SendingInfo& sendingInfo : subscriberShapePtr->specificTypeList) {
            sendShapeString(jsonString, sendingInfo);
        }

        std::this_thread::sleep_for(circleFrequency); // Sleep for 0.333 seconds
    }
}

// Function to handle squares
void Publisher::squareHandler() {

    // Create a JSON object
    nlohmann::json squareJson;

    // Add the shapeType field
    squareJson["shapeType"] = "Square";

    while (running) {
        for (const auto& entry : functionMap) {
            squareJson[entry.first] = entry.second();// Call the function and store the result in JSON
        }

        std::string jsonString = squareJson.dump();

        auto i = map.find("SQUARE");
        SubscriberShapePtr subscriberShapePtr = i->second;

        for (const SendingInfo& sendingInfo : subscriberShapePtr->specificTypeList) {
            sendShapeString(jsonString, sendingInfo);
        }

        std::this_thread::sleep_for(squareFrequency); // Sleep for 0.5 seconds
    }
}

// Generates a random size for the shape and returns it as a string
std::string Publisher::generateSize() {
    int size = (rand() % 100) + 1; // Random size between 1 and 100
    return std::to_string(size);
}

// Generates random coordinates for the shape and returns them as a string
std::string Publisher::generateCoordinates() {
    std::vector<int> coordinates(3);
    for (int i = 0; i < 3; ++i) {
        coordinates[i] = rand() % 1500; // Random coordinate
    }
    // Convert coordinates to string format
    std::string coordinatesString = "[" + std::to_string(coordinates[0]) + ", " +
        std::to_string(coordinates[1]) + ", " +
        std::to_string(coordinates[2]) + "]";
    return coordinatesString;
}

// Generates random colors for the shape and returns a string
std::string Publisher::generateColors() {
    std::vector<std::string> colors = { "Red", "Blue", "Green", "Yellow", "Purple" };
    int randomIndex = rand() % colors.size(); // Generate a random index within the range of colors vector
    return colors[randomIndex]; // Return the color at the random index
}


//Sends a string representation of a shape to a subscriber
void Publisher::sendShapeString(const std::string& shapeString, const SendingInfo& sendingInfo) {
    const sockaddr_in& addr = sendingInfo.getAddress();
    sendto(unicastSocket, shapeString.c_str(), shapeString.length(), 0, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
}


//Listens for new subscribers and manages their subscription
void Publisher::subscriberRegistrar() {
    char receiveData[1024];
    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    std::string delimiter = ":";

    while (running) {
        int bytesReceived = recvfrom(multicastSocket, receiveData, 1024, 0, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);

        if (bytesReceived != SOCKET_ERROR) {
            std::string jsonStr(receiveData, bytesReceived);
            auto jsonData = nlohmann::json::parse(jsonStr);

            int portNumber = jsonData["portNumber"];

            if (registeredPortNumbers.count(portNumber)) {
                continue;
            }

            // Register the port number
            registeredPortNumbers.insert(portNumber);

            //Create SendingInfo
            SendingInfo sendingInfo(portNumber);

            //Iterate over shape types and update SubscriberShape
            for (const auto& shapeType : jsonData["shapeTypes"]) {
                // Update subscriber shape in the map
                if (map.count(shapeType)) {
                    auto it = map.find(shapeType);
                    SubscriberShapePtr subscriberShapePtr = it->second;
                    subscriberShapePtr->specificTypeList.push_back(sendingInfo);

                    std::cout << "Registered subscriber for shape type: " << shapeType << std::endl;
                    std::cout << "Total subscribers for " << shapeType << ": " << subscriberShapePtr->specificTypeList.size() << std::endl;
                }
                else {
                    // Handle case when shape type is not found in map
                }
            }
        }
    }
}

void Publisher::loadConfigurationFromJson() {
    // Open the JSON file
    std::ifstream file("../../Configuration/Config.json");

    if (!file.is_open()) {
        std::cerr << "Error opening JSON file." << std::endl;
        return;
    }

    jsonConfig = nlohmann::json::parse(file);

    // Extract parameters
    multicastReceivingGroup = jsonConfig["multicastSendingGroup"];
    multicastReceivingPort = jsonConfig["multicastSendingPort"];
    squareFrequency = hertzToMilliseconds(jsonConfig["sqaureFrequency"]);
    circleFrequency = hertzToMilliseconds(jsonConfig["circleFrequency"]);
}

std::chrono::milliseconds Publisher::hertzToMilliseconds(int frequencyHz) {
    return std::chrono::milliseconds(static_cast<long long>(1000) / frequencyHz);
}











