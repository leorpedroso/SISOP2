#pragma once

#include<string>
#include<mutex>

class Counter{
    private:
        // curent value
        int value;
        // mutex for value
        std::mutex valueMutex;

    public:
        Counter(int value): value(value){}

        // getters
        int getValue();
        // setters
        void setValue(int val);
        void decrementValue();
};