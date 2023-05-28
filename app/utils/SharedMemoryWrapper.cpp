#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "SharedMemoryWrapper.h"

namespace bip = boost::interprocess;

using InnerAllocator = bip::allocator<std::pair<const uint32_t, float>,
                                          bip::managed_shared_memory::segment_manager>;

using InnerMap = bip::map<uint32_t, float, std::less<uint32_t>, InnerAllocator>;

using CharAllocator = bip::allocator<char, bip::managed_shared_memory::segment_manager>;

using SharedString = bip::basic_string<char, std::char_traits<char>, CharAllocator>;

using OuterAllocator = bip::allocator<std::pair<const SharedString, InnerMap>,
                                          bip::managed_shared_memory::segment_manager>;

using SharedMap = bip::map<SharedString, InnerMap, std::less<SharedString>, OuterAllocator>;

using MutexType = boost::interprocess::interprocess_mutex;

SharedMap *shared_main_db;
std::string shared_mem_id;
bip::managed_shared_memory segment_;
std::string mutex_name;
std::unique_ptr<bip::named_mutex> sm_name_mutex;

template<class T>
bool SharedMemoryWrapper<T>::init(std::string shm_id)
{
    try {
        segment_ = bip::managed_shared_memory(bip::open_or_create, shm_id.c_str(), 65536);
        shared_mem_id = shm_id;
    } catch (const bip::interprocess_exception& ex) {
        std::cout << "Failed to open or create shared memory segment: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

template<class T>
bool SharedMemoryWrapper<T>::update(T db){
    MutexType* mutex = segment_.find_or_construct<MutexType>("Mutex")();
    {
        boost::interprocess::scoped_lock<MutexType> lock(*mutex);
        SharedMap* local_shared_main_db = segment_.find_or_construct<SharedMap>(shared_mem_id.c_str())(segment_.get_allocator<OuterAllocator>());
        local_shared_main_db->clear();
        for (auto &[comp, inner_map]: db)
        {
            //std::map<K, V> inner;
            std::map<uint32_t, float> inner;

            for (auto &[id, val]: inner_map)
            {
                inner.insert({id, val});
            }

            auto in_ner = InnerMap(inner.begin(), inner.end(), segment_.get_allocator<InnerAllocator>());
            SharedString component(comp.c_str(), local_shared_main_db->get_allocator());
            local_shared_main_db->insert(std::make_pair(component, in_ner));
        }
        return true;
    }
}

template<class T>
T SharedMemoryWrapper<T>::retrieve(std::string _shared_mem_id){

    shared_mem_id = _shared_mem_id;
    boost::interprocess::managed_shared_memory segment;
    segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, shared_mem_id.c_str(), 65536);

    MutexType* mutex = segment.find_or_construct<MutexType>("Mutex")();
    std::map<std::string, std::map<uint32_t, float>> api_db;
    {
        boost::interprocess::scoped_lock<MutexType> lock(*mutex);
        SharedMap* local_shared_main_db = segment.find_or_construct<SharedMap>(shared_mem_id.c_str())(segment.get_allocator<OuterAllocator>());

        for (auto &[comp, comp_db]: *local_shared_main_db)
        {
            for (auto &[k, v]: comp_db)
            {
                api_db[std::string(comp.c_str())].insert(std::make_pair(k,v));
            }
        }
    }
    // Unlock the mutex (automatically released when 'lock' goes out of scope)
    return api_db;
}

template class SharedMemoryWrapper<std::map<std::string, std::map<uint32_t, float>>>;
