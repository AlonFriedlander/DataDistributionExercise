#include "Publisher\Publisher.h" 

int main() {

    Publisher publisher;
    publisher.startPublishing();

    std::this_thread::sleep_for(std::chrono::seconds(700));
    publisher.stopPublishing();

    return 0;
}

