#pragma once


#include <cstddef>
#include <stdexcept>
#include <iostream>

template <typename T, std::size_t Capacity>
class LinearBuffer {
private:
    T buffer_[Capacity];   // fixed-size array
    std::size_t size_ = 0; // current number of elements

public:
    LinearBuffer() = default;

    // Add element to the end
    void push_back(const T& item) {
        if (size_ >= Capacity) {
            throw std::overflow_error("Buffer is full");
        }
        buffer_[size_++] = item;
    }


    T* head_ptr() {
        return buffer_;
    }

    // Access element at index
    T& operator[](std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return buffer_[index];
    }

    // Current number of elements
    std::size_t size() const {
        return size_;
    }

    // Maximum capacity
    std::size_t capacity() const {
        return Capacity;
    }

    // Check if buffer is empty
    bool empty() const {
        return size_ == 0;
    }

    // Check if buffer is full
    bool full() const {
        return size_ == Capacity;
    }

    void clear() {
        size_ = 0;
    }
};
