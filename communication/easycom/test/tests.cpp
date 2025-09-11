#include <array>
#include <iostream>
#include <ostream>

// #include "../serial/cobs.h"
// #include "../EasyCom.h"

#include "../can/TransportCanLinux.h"


void on_message(int id, uint8_t* message, std::size_t length) {
    std::cout << "Received message with ID: " << id << " and length: " << length << std::endl;
    std::cout << "Data: ";
    for (std::size_t i = 0; i < length; ++i) {
        std::cout << std::hex << static_cast<int>(message[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {

    // EasyCom easycom = EasyCom();

    TransportCanLinux transport("can0");

    transport.set_on_message_callback(on_message);

    transport.send_message(4, (uint8_t *)"12345", 5);





    return 0;
}
