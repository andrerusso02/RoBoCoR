#pragma once
#include <memory>

#include "serial/Framer.h"
#include "TransportInterface.h"
#include "Messages.h"
#include "Dispatcher.h"


class EasyCom {

    template <typename T>
    using ReceiveFunc = void(*)(const T& data);

public:

    EasyCom() : framer_(Framer<FRAME_MAX_SIZE>(END_DELIMITER)) {
        transport_interface_.set_on_message_callback(
            std::bind(&Dispatcher::dispatch, &dispatcher_,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    template <typename T>
    void send(EasyComID id, T data) {
        transport_interface_.send_message(
            static_cast<uint8_t>(id), static_cast<uint8_t *>(data), sizeof(T));
    }


    template <typename T>
    void subscribe(EasyComID id, ReceiveFunc<T> func) {
        auto glue_func = [func](const T& data) {
            func(*static_cast<const T *>(data));
        };
        dispatcher_.register_callback(id, glue_func);
    }

    void receive(uint8_t byte) {

    }

private:
    Framer<FRAME_MAX_SIZE> framer_; // Move to serial
    TransportInterface transport_interface_;
    Dispatcher dispatcher_;


};

