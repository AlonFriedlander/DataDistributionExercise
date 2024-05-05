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
    
    // Initialize list and map of subscribers
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
    running = false;
    closesocket(multicastSocket);
    closesocket(unicastSocket);
}


// Internal: Initializes the list of subscribers
void Publisher::initializeList() {
    for (ShapeEnum::ShapeType shapeType : ShapeEnum::AllTypes) {
        // Create a subscriber shape object
        SubscriberShapePtr subscriberShapePtr = std::make_shared<std::vector<SendingInfo>>();

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
    ThreadPool threadPool(jsonConfig["numOfThreads"]);
    // Add tasks to the thread pool
    threadPool.enqueue([this] { circleHandler(); });
    threadPool.enqueue([this] { squareHandler(); });
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
    circleJson["shapeType"] = shapeTypeToString(ShapeEnum::ShapeType::CIRCLE);

    while (running) {
        for (const auto& entry : functionMap) {
            circleJson[entry.first] = entry.second();// Call the function and store the result in JSON
        }

        std::string jsonString = circleJson.dump();

        auto i = map.find(shapeTypeToString(ShapeEnum::ShapeType::CIRCLE));
        SubscriberShapePtr subscriberShapePtr = i->second;

        for (const SendingInfo& sendingInfo : *subscriberShapePtr) {
            sendShape(jsonString, sendingInfo);
        }

        std::this_thread::sleep_for(circleFrequency); 
    }
}

// Function to handle squares
void Publisher::squareHandler() {

    // Create a JSON object
    nlohmann::json squareJson;

    // Add the shapeType field
    squareJson["shapeType"] = shapeTypeToString(ShapeEnum::ShapeType::SQUARE);

    while (running) {
        for (const auto& entry : functionMap) {
            squareJson[entry.first] = entry.second();// Call the function and store the result in JSON
        }

        std::string jsonString = squareJson.dump();

        auto i = map.find(shapeTypeToString(ShapeEnum::ShapeType::SQUARE));
        SubscriberShapePtr subscriberShapePtr = i->second;

        for (const SendingInfo& sendingInfo : *subscriberShapePtr) {
            sendShape(jsonString, sendingInfo);
        }

        std::this_thread::sleep_for(squareFrequency); 
    }
}

// Generates a random size for the shape 
std::string Publisher::generateSize() {
    int size = (rand() % 100) + 1; // Random size between 1 and 100
    return std::to_string(size);
}

// Generates random coordinates for the shape 
std::string Publisher::generateCoordinates() {
    std::vector<int> coordinates(3);
    for (int i = 0; i < 3; ++i) {
        coordinates[i] = rand() % 1500; // Random coordinate
    }

    std::string coordinatesString = "[" + std::to_string(coordinates[0]) + ", " +
        std::to_string(coordinates[1]) + ", " +
        std::to_string(coordinates[2]) + "]";
    return coordinatesString;
}

// Generates random colors for the shape 
std::string Publisher::generateColors() {
    std::vector<std::string> colors = { "Red", "Blue", "Green", "Yellow", "Purple" };
    int randomIndex = rand() % colors.size(); // Generate a random index within the range of colors vector
    return colors[randomIndex]; // Return the color at the random index
}

//Sends a shape to subscriber
void Publisher::sendShape(const std::string& shapeString, const SendingInfo& sendingInfo) {
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

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);

            //Create SendingInfo
            SendingInfo sendingInfo(portNumber,clientIP);

            //Iterate over shape types and update SubscriberShape
            for (const auto& shapeType : jsonData["shapeTypes"]) {
                // Update subscriber shape in the map
                if (map.count(shapeType)) {
                    auto it = map.find(shapeType);
                    SubscriberShapePtr subscriberShapePtr = it->second;
                    subscriberShapePtr->push_back(sendingInfo);

                    std::cout << "Registered subscriber for shape type: " << shapeType << std::endl;
                    std::cout << "Total subscribers for " << shapeType << ": " << subscriberShapePtr->size() << std::endl;
                }
                else {
                    std::cout << "This shape is not avalible: " << shapeType << std::endl;
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
    return std::chrono::milliseconds(1000 / frequencyHz);
}




//try with thread pool that each thread execute the task once and back to the pool...
//...................................................................................
//void Publisher::eventManager() {
//    ThreadPool pool(jsonConfig["numOfThreads"]); // Create the thread pool
//    std::thread circleThread(&Publisher::circleTask, this, std::ref(pool)); // Pass pool by reference
//    std::thread squareThread(&Publisher::squareTask, this, std::ref(pool)); // Pass pool by reference
//    circleThread.join();
//    squareThread.join();
//}
//
//void Publisher::circleTask(ThreadPool& pool) {
//    auto i = map.find(shapeTypeToString(ShapeEnum::ShapeType::CIRCLE));
//    SubscriberShapePtr subscriberShapePtr = i->second;
//    // Create a JSON object
//    nlohmann::json circleJson;
//    // Add the shapeType field
//    circleJson["shapeType"] = shapeTypeToString(ShapeEnum::ShapeType::CIRCLE);
//    while (running) {
//        for (const auto& entry : functionMap) {
//            circleJson[entry.first] = entry.second(); // Call the function and store the result in JSON
//        }
//        std::string jsonString = circleJson.dump();
//        std::this_thread::sleep_for(circleFrequency);
//        if (!subscriberShapePtr->empty()) {
//            pool.enqueue([jsonString, subscriberShapePtr, this] { handler(jsonString, subscriberShapePtr); });
//
//        }
//    }
//}
//
//void Publisher::squareTask(ThreadPool& pool) {
//    auto i = map.find(shapeTypeToString(ShapeEnum::ShapeType::SQUARE));
//    SubscriberShapePtr subscriberShapePtr = i->second;
//    // Create a JSON object
//    nlohmann::json squareJson;
//    // Add the shapeType field
//    squareJson["shapeType"] = shapeTypeToString(ShapeEnum::ShapeType::SQUARE);
//    while (running) {
//        for (const auto& entry : functionMap) {
//            squareJson[entry.first] = entry.second(); // Call the function and store the result in JSON
//        }
//        std::string jsonString = squareJson.dump();
//        std::this_thread::sleep_for(squareFrequency);
//        if (!subscriberShapePtr->empty()) {
//            pool.enqueue([jsonString, subscriberShapePtr, this] { handler(jsonString, subscriberShapePtr); });
//        }
//    }
//}
//
////task
//void Publisher::handler(const std::string& jsonString, const SubscriberShapePtr& subscriberShapePtr) {
//    for (const SendingInfo& sendingInfo : *subscriberShapePtr) {
//        sendShape(jsonString, sendingInfo);
//    }
//}





