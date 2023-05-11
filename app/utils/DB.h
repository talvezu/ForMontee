#include <unordered_map>
#include <map>
#include <string>
#include <mutex>
#include <functional>

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
    public:
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
            std::scoped_lock<std::mutex> l(main_db[component].m);
            for (auto &[id, val]: new_vals)
            {
                main_db[component].my_map[id] = val;
            }
        }
    };

};
