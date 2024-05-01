#include "Subscriber\Subscriber.h"

int main(int argc, char* argv[])
{
    // Command line arguments: subscriberName portNumber shapeType attributes

    // subscriberName: Specify a name for this test instance.
    // portNumber: Enter a port number for this instance.
    // shapeType: Choose the shape(s) you want to receive. Available options: SQUARE, CIRCLE, or both SQUARE:CIRCLE.
    // attributes: Specify the attributes you want to receive. Available options: size, coordinates, colors.


    // Pass the arguments as an array to the constructor of Subscriber
    Subscriber subscriber;

    std::this_thread::sleep_for(std::chrono::seconds(1000));

    subscriber.stopPublishing();
    return 0;
}