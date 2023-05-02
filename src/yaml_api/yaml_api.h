#pragma once
#include <iostream>
#include <set>
#include <optional>
#include <yaml-cpp/yaml.h>
//#include "point.h" //todon convert 1d to 3d using point


using YAML::Parser;

template<class T>
class FunctionTask{
    std::string task_name;
    std::set<uint32_t> target_motors;
    uint32_t times;
    uint32_t  delta_in_milliseconds;
    std::optional<std::vector<T>> values;
    std::optional<std::vector<T>> delta_values;
    bool is_infinite;
public:
    FunctionTask(std::string _name,
                 std::set<uint32_t> &&_target_motors,
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
    std::set<int> motors;
    YAML::Node config;
    std::map<std::string, std::shared_ptr<FunctionTask<T>>> tasks;
public:
    workflow(std::string _filename):filename(_filename)
    {
    }
    bool init();

    bool load_work();
};


