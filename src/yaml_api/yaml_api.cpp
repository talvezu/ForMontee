#include <iostream>
#include <set>
#include <optional>
#include <yaml-cpp/yaml.h>
#include <re2/re2.h>
#include "yaml_api.h"
#include "point.h" //todon convert 1d to 3d using point


using YAML::Parser;


template <typename T>
FunctionTask<T>::FunctionTask(std::string _name,
                           std::set<uint32_t> _target_motors,
                           uint32_t _times,
                           uint32_t _delta_in_milliseconds,
                           std::optional<std::vector<T>> _values,
                           std::optional<std::vector<T>> _delta_values):
                           task_name(_name),
                           target_motors(_target_motors),
                           times(_times),
                           delta_in_milliseconds(_delta_in_milliseconds),
                           values(_values),
                           delta_values(_delta_values)
{
        is_infinite = (times==0)?true:false;
}


template <typename T>
bool workflow<T>::init()
{
    try
    {
        config = YAML::LoadFile(filename);
    }
    catch (const YAML::ParserException& e) {
        std::cout << "ParserException parse config file failed:" << e.msg << std::endl;
        return false;
    } catch (const YAML::BadFile e) {
        std::cout << "parse config file failed:" << e.what() << std::endl;
        return false;
    };
    return true;
}

template <typename T>
void workflow<T>::constract_net_action(const std::string key,const YAML::Node &attr)
{
    for (auto item: attr["target_motors"])
    {
        net_func_motors.insert(item.as<std::uint32_t>());
    };

    net_actions.emplace(key, netAction(net_func_motors,
                                       attr["times"].as<uint32_t>(),
                                       attr["action"].as<std::string>()));

}

/*
//overkill, for fun.
bool partialmatch_for_net_action_re(const std::string &str){
  re2::StringPiece input(str);
  re2::RE2 re(".*");
  std::string t,s;
  if(re2::RE2::PartialMatch(input, re , &s, &t))
  {
    std::cout << "s:"<< s << " t:"<< t << "\n";
    return true;
  }
  return false;
}
*/
template <typename T>
bool workflow<T>::load_work()
{
    for(YAML::const_iterator it=config.begin(); it!=config.end(); ++it){
        const std::string &key=it->first.as<std::string>();
        std::cout<<key<<"\n";
        if (key.find("net_action") != std::string::npos)
        {
            constract_net_action(key, it->second);
            continue;
        }
        //auto obj = it->second.as<YAML::Node<FunctionTask<float>>>();
        YAML::Node attr = it->second;
        for (auto item: attr["target_motors"])
        {
            //std::cout<<item<<"\n";
            gp_motors.insert(item.as<uint32_t>());
        }

        std::optional<std::vector<T>> values = std::nullopt;
        for (auto item: attr["values"])
        {
            if (!values)
            {
                values.emplace();
            }
            values->emplace_back(item.as<T>());
        }

        std::optional<std::vector<T>> delta_values = std::nullopt;
        for (auto item: attr["delta_values"])
        {
            if (!delta_values)
            {
                delta_values.emplace();
            }
            delta_values->emplace_back(item.as<T>());
        }


        tasks[key] = std::make_shared<FunctionTask<float>>(
                key,
                gp_motors,
                attr["times"].as<uint32_t>(),
                attr["delta_in_milliseconds"].as<uint32_t>(),
                std::move(values),
                std::move(delta_values)
                );
        std::cout<<*tasks[key].get();

        continue;

    }
    return true;
}

template <typename T>
std::shared_ptr<FunctionTask<T>> workflow<T>::get_task_by_name(std::string name)
{
    auto it = tasks.find(name);
    if ( it == tasks.end())
        return std::shared_ptr<FunctionTask<T>>();
    else
        return it->second;
}

template <typename T>
uint8_t workflow<T>::get_motors_count()
{
    return static_cast<uint8_t> (gp_motors.size() + net_func_motors.size());
}


template class workflow<float>;
template class FunctionTask<float>;

/*
int main()
{
    workflow<float> f("text.yaml");
    f.init();
    return (int)(f.load_work());
}
*/

