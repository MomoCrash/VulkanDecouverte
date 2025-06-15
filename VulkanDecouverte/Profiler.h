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
    Profiler();
    void NewTask(std::string name);
    void EndTask();
private:
    Task mTask;
};   
