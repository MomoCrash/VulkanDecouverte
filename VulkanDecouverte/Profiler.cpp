#include "Profiler.h"

#define DEEP_PROFILER

Profiler::Profiler() : mTask(Task()) {}

void Profiler::NewTask(std::string name)
{
#ifdef DEEP_PROFILER
    mTask.name = name;
    mTask.mStart = std::chrono::high_resolution_clock::now();
#endif
}

void Profiler::EndTask()
{
#ifdef DEEP_PROFILER
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mTask.mStart).count();
    printf("[PROFILER] \033[31m Your task : %s took %lld nanoseconds | %f seconds \033[0m\n", mTask.name.c_str(), duration, duration / 1000000000.0f);
#endif
}
