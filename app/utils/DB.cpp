#include <map>
#include <string>
#include "DB.h"
//#include "SimpleBoostSharedMemory.h"
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "logging.h"

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

auto main_db = std::make_shared<utils::DB<uint32_t, float>>();
bip::managed_shared_memory segment_;

//shared memory
std::string mutex_name;
std::unique_ptr<bip::named_mutex> sm_name_mutex;
SharedMap *shared_main_db;

namespace utils{

    // Definition of the explicit specialization
    template bool DB<uint32_t, float>::init_sm(std::string shared_mem_id);
    template std::shared_ptr<DB<uint32_t, float>> get_db();

    //template std::map<std::string,std::map<uint32_t, float>> utils::DB<uint32_t, float>::get_all_values();
    template <class K, class V>
    std::shared_ptr<DB<K, V>> get_db()
    {
        return main_db;
    }

    template <class K, class V>
    std::map<std::string, std::map<K, V>> utils::DB<K,V>::get_all_values()
    {
        std::map<std::string, std::map<K, V>> ret;
        for (auto &[comp, comp_db]: main_db){
            std::scoped_lock<std::mutex> l(comp_db.m);

            for (auto &[k, v]: comp_db.my_map)
            {
                ret[comp].emplace(k,v);
            }
        }
        return ret;
    }

    template<class K, class V>
    void utils::DB<K, V>::update_sm()
    {
        auto db = get_all_values();
        MutexType* mutex = segment_.find_or_construct<MutexType>("Mutex")();
        {
            boost::interprocess::scoped_lock<MutexType> lock(*mutex);
            SharedMap* shared_main_db = segment_.find_or_construct<SharedMap>(sm_dev_location.c_str())(segment_.get_allocator<OuterAllocator>());
            shared_main_db->clear();
            for (auto &[comp, inner_map]: db)
            {
                std::map<K, V> inner;

                for (auto &[id, val]: inner_map)
                {
                    inner.insert({id, val});
                }

                auto in_ner = InnerMap(inner.begin(), inner.end(), segment_.get_allocator<InnerAllocator>());
                SharedString component(comp.c_str(), shared_main_db->get_allocator());
                shared_main_db->insert(std::make_pair(component, in_ner));
            }
        }
    }

    template<>
    std::map<std::string, std::map<uint32_t, float>> utils::DB<uint32_t, float>::python_api_read_main_db(std::string _shared_mem_id)
    {
        boost::interprocess::managed_shared_memory segment;
        segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, _shared_mem_id.c_str(), 65536);

        MutexType* mutex = segment.find_or_construct<MutexType>("Mutex")();
        std::map<std::string, std::map<uint32_t, float>> api_db;
        {
            boost::interprocess::scoped_lock<MutexType> lock(*mutex);
            SharedMap* shared_main_db = segment.find_or_construct<SharedMap>(_shared_mem_id.c_str())(segment.get_allocator<OuterAllocator>());

            for (auto &[comp, comp_db]: *shared_main_db)
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

    template<class K, class V>
    bool utils::DB<K, V>::init_sm(std::string shared_mem_id)
    {
        if (sm_dev_location.size())
        {
            if (logging::active)
            {
                std::cout<<"shared memory already initialized at:"<<sm_dev_location<<"\n";
            }
            return false;
        }

        try {
            segment_ = bip::managed_shared_memory(bip::open_or_create, shared_mem_id.c_str(), 65536);
        } catch (const bip::interprocess_exception& ex) {
            std::cout << "Failed to open or create shared memory segment: " << ex.what() << std::endl;
            return false;
        }
        sm_dev_location = shared_mem_id;
        return true;
    }
};//namespace utils

PYBIND11_MODULE(python_api_shared_data, m) {
    pybind11::class_<utils::DB<uint32_t, float>, std::shared_ptr<utils::DB<uint32_t, float>>>(m, "DB")
        .def(pybind11::init())
        .def("get_vals", &utils::DB<uint32_t, float>::python_api_read_main_db);

    // Expose the main_db object
    m.attr("main_db") = main_db;
}
