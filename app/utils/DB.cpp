#include "DB.h"
#include "SharedMemoryWrapper.h"
#include "global_settings.h"
#include <iostream>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>


auto main_db = std::make_shared<utils::DB<uint32_t, float>>();
SharedMemoryWrapper<std::map<std::string, std::map<uint32_t, float>>> smw;
namespace utils
{

// Definition of the explicit specialization
template bool DB<uint32_t, float>::init_sm(std::string shared_mem_id);
template void DB<uint32_t, float>::update_sm();
template std::shared_ptr<DB<uint32_t, float>> get_db();

template <class K, class V>
std::shared_ptr<DB<K, V>> get_db()
{
    return main_db;
}

template <class K, class V>
std::map<std::string, std::map<K, V>> utils::DB<K, V>::get_all_values()
{
    std::map<std::string, std::map<K, V>> ret;
    for (auto &[comp, comp_db] : main_db)
    {
        std::scoped_lock<std::mutex> l(comp_db.m);

        for (auto &[k, v] : comp_db.my_map)
        {
            ret[comp].emplace(k, v);
        }
    }
    return ret;
}

template <class K, class V>
void utils::DB<K, V>::update_sm()
{
    smw.update(get_all_values());
}

template <class K, class V>
std::map<std::string, std::map<K, V>> utils::DB<K, V>::python_api_read_main_db(
    std::string _shared_mem_id)
{
    return smw.retrieve(_shared_mem_id);
}

template <class K, class V>
bool utils::DB<K, V>::init_sm(std::string shared_mem_id)
{
    if (sm_dev_location.size())
    {
        if (settings::logging::active)
        {
            std::cout << "shared memory already initialized at:"
                      << sm_dev_location << "\n";
        }
        return false;
    }

    smw.init(shared_mem_id);
    sm_dev_location = shared_mem_id;
    return true;
}
}; //namespace utils


// shared pointer handle. when an instance of this class is created in Python,
// it will be wrapped in a shared pointer.
PYBIND11_MODULE(python_api_shared_data, m)
{

    pybind11::class_<utils::DB<uint32_t, float>,
                     std::shared_ptr<utils::DB<uint32_t, float>>>(m, "DB")
        .def(pybind11::init())
        .def("get_vals", &utils::DB<uint32_t, float>::python_api_read_main_db);

    // Expose the main_db object so no object created in python.
    m.attr("main_db") = main_db;
}
