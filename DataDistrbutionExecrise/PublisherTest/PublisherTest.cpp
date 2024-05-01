#include "Publisher\Publisher.h" 

int main() {

    Publisher publisher;
    publisher.startPublishing();

    std::this_thread::sleep_for(std::chrono::seconds(1000));
    std::cout << "im arrive here?" << std::endl;
    publisher.stopPublishing();

    return 0;
}

