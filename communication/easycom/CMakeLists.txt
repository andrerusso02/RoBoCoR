cmake_minimum_required(VERSION 3.16)
project(fastserial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tests test/tests.cpp
        serial/Framer.h
        Config.h
        ../../storage/RingBuffer.h
        ../../storage/LinearBuffer.h
        Dispatcher.h
        EasyCom.h
        Messages.h
        TransportInterface.h)
