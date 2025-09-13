#pragma once
#include <memory>

#include "serial/Framer.h"
#include "TransportInterface.h"
#include "Messages.h"
#include "Dispatcher.h"

class EasyCom {

    template <typename MsgType>
    using ReceiveFunc = void(*)(const MsgType& data);

public:

    /*
    If I see that the argument forwarding complexifies too much the logic, I can come back to something like:
        auto easycom = EasyCom(TransportCanLinux("can0"));
    */
    template <typename Transport, typename... Args>
    static EasyCom create(Args&&... args) {
        return EasyCom(std::make_unique<Transport>(std::forward<Args>(args)...));
    }


    template <typename MsgType>
    void send(EasyComID id, MsgType data) {
        transport_interface_->send_message(
            static_cast<uint8_t>(id), reinterpret_cast<uint8_t *>(&data), sizeof(MsgType));
    }


    template <typename MsgType>
    void subscribe(EasyComID id, ReceiveFunc<MsgType> func) {
        auto glue_func = [func](const uint8_t* data, size_t length) {
            if (length != sizeof(MsgType)) {
                std::cerr << "Error: Received message length " << length
                          << " does not match expected size " << sizeof(MsgType) << std::endl;
                return;
            }
            MsgType msg = *reinterpret_cast<const MsgType*>(data);
            func(msg);
        };
        dispatcher_.register_callback(id, glue_func);
    }

private:
    Framer<FRAME_MAX_SIZE> framer_; // Move to serial
    std::unique_ptr<TransportInterface> transport_interface_;
    Dispatcher dispatcher_;

    explicit EasyCom(std::unique_ptr<TransportInterface> transport)
    : framer_(Framer<FRAME_MAX_SIZE>(END_DELIMITER)),
      transport_interface_(std::move(transport))
    {
        transport_interface_->set_on_message_callback(
            std::bind(&Dispatcher::dispatch, &dispatcher_,
                      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }
};

