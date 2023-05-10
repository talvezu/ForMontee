#pragma once
#include <iostream>
#include <set>
#include <optional>
#include <yaml-cpp/yaml.h>
//#include "point.h" //todon convert 1d to 3d using point


using YAML::Parser;

struct netAction{
    std::set<uint32_t> motors; //unused
    uint32_t times; //unused
    std::string action;
    netAction(std::set<uint32_t> _motors, uint32_t _times, std::string _action):
        motors(_motors),
        times(_times),
        action(_action)
    {

    }
};

template<class T>
class FunctionTask{
public:
    std::string task_name;
    std::set<uint32_t> target_motors;
    uint32_t times;
    uint32_t  delta_in_milliseconds;
    std::optional<std::vector<T>> values;
    std::optional<std::vector<T>> delta_values;
    bool is_infinite;
    FunctionTask(std::string _name,
                 std::set<uint32_t> _target_motors,
                 uint32_t _times,
                 uint32_t _delta_in_milliseconds,
                 std::optional<std::vector<T>> _values,
                 std::optional<std::vector<T>> _delta_values);

    friend std::ostream & operator<<(std::ostream& os, FunctionTask& obj){
        std::cout<<"task_name: "<<obj.task_name<<"\n";
        std::cout<<"motors: ";
        for (auto motor: obj.target_motors)
        {
            std::cout<<motor<<" ";
        }
        std::cout<<"\n";
        std::cout<<"times: "<<obj.times<<"\n";
        std::cout<<"delta_in_milliseconds: "<<obj.delta_in_milliseconds<<"\n";
        if (obj.values.has_value())
        {
            std::cout<<"values: ";
            for (auto value: obj.values.value())
            {
                std::cout<<value<<" ";
            }
        }
        std::cout<<"\n";
        if (obj.delta_values.has_value())
        {
            std::cout<<"delta_values: ";
            for (auto value: obj.delta_values.value())
            {
                std::cout<<value<<" ";
            }
        }
        std::cout<<"\n";
        std::cout<<"\n";
        return os;
    }
};


template <class T>
class workflow{
    std::string filename;
    std::set<uint32_t> gp_motors;
    std::set<uint32_t> net_func_motors;
    YAML::Node config;
    std::map<std::string, std::shared_ptr<FunctionTask<T>>> tasks;
    std::map<std::string, netAction> net_actions;
    uint16_t task_count;

    void constract_net_action(const std::string key, const YAML::Node &attr);
public:
    workflow(std::string _filename):filename(_filename)
    {
        task_count = 0;
    }
    bool init();

    bool load_work();

    std::shared_ptr<FunctionTask<T>> get_task_by_name(std::string name);
    uint8_t get_motors_count();
    const auto &get_tasks(){
        return tasks;
    }

    const auto &get_net_actions(){
        return net_actions;
    }

    std::set<uint32_t> get_gp_motors()
    {
        return gp_motors;
    }

    std::set<uint32_t> get_net_func_motors()
    {
        return net_func_motors;
    }

};

