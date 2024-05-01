// Shape.h

#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <iostream>

class Shape {
public:
    virtual ~Shape() = default;
};

class Square : public Shape {
public:
    Square(int size, const std::vector<int>& coordinates);
    int getSize() const { return size; } // Getter for size
    const std::vector<int>& getCoordinates() const { return coordinates; }

private:
    int size;
    std::vector<int> coordinates;
};

class Circle : public Shape {
public:
    Circle(int size, const std::vector<int>& coordinates);
    int getSize() const { return size; } // Getter for size
    const std::vector<int>& getCoordinates() const { return coordinates; }

private:
    int size;
    std::vector<int> coordinates;
};

#endif

