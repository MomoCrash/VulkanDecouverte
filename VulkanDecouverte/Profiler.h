#pragma once
#include <chrono>
#include <string>


struct Task
{
    std::string name;
    std::chrono::high_resolution_clock::time_point mStart;
};

class Profiler
{
public:
    ~Profiler();
    static void NewTask(std::string name);
    static void EndTask();
private:
    static inline Task* mTask = new Task();
};   
