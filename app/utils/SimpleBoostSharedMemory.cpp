#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "SimpleBoostSharedMemory.h"
#include "DB.h"

std::map<std::string, boost::interprocess::shared_memory_object> shared_memory_map;

namespace SimpleBoostSharedMemory
{
    template<class T>
    bool open_or_create(std::string shared_memory_id){
        if (shared_memory_map.find(shared_memory_id) != shared_memory_map.end())
            return false;

        //boost::interprocess::shared_memory_object shm = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, "my_shared_memory", boost::interprocess::read_write);
        shared_memory_map.emplace(shared_memory_id,boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, shared_memory_id.c_str(), boost::interprocess::read_write));
        //shared_memory_map.emplace(shared_memory_id, boost::interprocess::open_or_create, shared_memory_id.c_str(), boost::interprocess::read_write);
        shared_memory_map[shared_memory_id].truncate(sizeof(T));
        return true;
    }

    template<class T>
    bool delete_sm(std::string shared_memory_id){
        if (shared_memory_map.find(shared_memory_id) == shared_memory_map.end())
            return false;

        shared_memory_map.erase(shared_memory_id);
        return true;
    }

    template <typename T>
    void get_shared_memory(std::string shared_memory_id, T& obj)
    {
        auto sm_element = shared_memory_map.find(shared_memory_id);
        if (sm_element == shared_memory_map.end())
            return;

        // Map the shared memory object into the process's address space
        boost::interprocess::mapped_region region(shared_memory_map[shared_memory_id], boost::interprocess::read_write);
        obj = *(static_cast<T*>(region.get_address()));
    }

    template<class T>
    void update_shared_memory(std::string shared_memory_id, const T& update)
    {
        auto sm_element = shared_memory_map.find(shared_memory_id);
        if (sm_element == shared_memory_map.end())
            return;

        // Map the shared memory object into the process's address space
        boost::interprocess::mapped_region region(shared_memory_map[shared_memory_id], boost::interprocess::read_write);
        *(static_cast<T*>(region.get_address())) = update;
    }
    template bool open_or_create<utils::DB<uint32_t, float>>(std::string);
    template bool delete_sm<utils::DB<uint32_t, float>>(std::string shared_memory_id);
    template void get_shared_memory<utils::DB<uint32_t, float>>(std::string shared_memory_id, utils::DB<uint32_t, float>& obj);
    template void update_shared_memory<utils::DB<uint32_t, float>>(std::string shared_memory_id, const utils::DB<uint32_t, float>& update);

};



