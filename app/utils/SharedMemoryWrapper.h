#pragma once
#include <string>

template <class T>
class SharedMemoryWrapper
{
public:
    bool init(std::string shm_id);
    bool update(T db);
    T retrieve(std::string _shared_mem_id);
};
