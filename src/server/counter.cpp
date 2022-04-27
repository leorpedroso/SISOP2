#include"../../include/server/counter.hpp"

int Counter::getValue(){
    std::unique_lock<std::mutex> mlock(valueMutex);
    return value;
}
void Counter::decrementValue(){
    std::unique_lock<std::mutex> mlock(valueMutex);
    --value;
}
void Counter::setValue(int val){
    std::unique_lock<std::mutex> mlock(valueMutex);
    value = val;
}