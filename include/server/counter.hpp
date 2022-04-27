#pragma once

#include<string>
#include<mutex>

class Counter{
    private:
        int value;

        std::mutex valueMutex;

    public:
        Counter(int value): value(value){}

        // getters
        int getValue();
        void decrementValue();

        void setValue(int val);
};