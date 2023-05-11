#include <unordered_map>
#include <set>
#include <string>
#include <mutex>
#include <functional>

class DB{
    std::function<void(std::unordered_map<std::string, uint32_t>)> set_components_callback;
    std::unordered_map<std::string, uint32_t> established_components;
    std::unordered_map<uint32_t, float> main_db;


    mutable std::mutex m;
public:
    DB(std::function<void(std::unordered_map<std::string, uint32_t>)> _set_components_callback):set_components_callback(_set_components_callback)
    {
        set_components_callback(established_components);
    }

    void update_db(std::set<std::pair<uint32_t, float>> new_vals)
    {
        for (auto &[comp, val]: new_vals)
        {
            main_db.at(comp) = val;
        }
    }
};
