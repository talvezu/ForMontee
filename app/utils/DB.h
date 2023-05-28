#include <unordered_map>
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <functional>
#include <iostream>
#include "global_settings.h"

namespace utils{

    template<class K, class V>
    class DB{
        class ComponentDB{
            std::mutex m;
            std::map<K, V> my_map;
            friend class DB;
        };

        std::map<std::string, ComponentDB> main_db;
        mutable std::mutex table_m;
        std::string sm_dev_location;
        //static constexpr std::string_view shared_memory_page = "db_on_dev";

    public:
        DB():sm_dev_location("") {}

        DB& operator=(const DB& other) {
            if (this != &other) {
                //std::scoped_lock<std::mutex> l(other.table_m);
                for (auto &[comp, comp_db]: other.main_db){
                    std::scoped_lock<std::mutex> l_(main_db[comp].m);
                    for (auto &[k, v]: comp_db.my_map)
                    {
                        main_db[comp].my_map.emplace(k,v);
                    }
                }
            }
            return *this;
        }

        DB(const DB& other) {
            //std::scoped_lock<std::mutex> l(other.table_m);
            for (auto &[comp, comp_db]: other.main_db){
                std::scoped_lock<std::mutex> l_(main_db[comp].m);
                for (auto &[k, v]: comp_db.my_map)
                {
                    main_db[comp].my_map.emplace(k,v);
                }
            }
        }

        std::map<K, V> get_component_db(const std::string &component_required)
        {
            std::map<K, V> ret;
            if (main_db.find(component_required) != main_db.end())
            {
                for (auto &[id, value] : main_db[component_required].my_map)
                    ret.emplace(id, value);
            }
            return ret;
        }

        void set_component_db(const std::string &component, const std::map<K, V> &new_vals)
        {
            {
                std::scoped_lock<std::mutex> l(main_db[component].m);
                for (auto &[id, val]: new_vals)
                {
                    main_db[component].my_map[id] = val;
                }
            }
            if (settings::use_shared_memory::active && sm_dev_location.size())
                update_sm();
        }

        void update_sm();
        bool init_sm(std::string shared_mem_id);
        std::map<std::string, std::map<K, V>> get_all_values();

        std::map<std::string, std::map<K, V>> python_api_read_main_db(std::string _shared_mem_id);
    };


    template <class K, class V>
    std::shared_ptr<DB<K, V>> get_db();


};

template class utils::DB<uint32_t, float>;
