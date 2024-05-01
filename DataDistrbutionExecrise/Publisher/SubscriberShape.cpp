#include "SubscriberShape.h"

SubscriberShape::SubscriberShape(const std::string& shapeType, int frequencyInHZ)
    : shapeType(shapeType), frequency(convertFrequency(frequencyInHZ)) {}

bool SubscriberShape::checkUpdateTime(int time) {
    return (time % (int)frequency) == 0;
}

double SubscriberShape::convertFrequency(int frequencyInHZ) {
    return std::floor(10.0 / frequencyInHZ);
}
