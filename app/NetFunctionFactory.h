#pragma once
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>

class NetFunction
{
    std::set<uint32_t> motors;

public:
    virtual float operator()(float f) = 0;
    virtual ~NetFunction()
    {
    }
    void set_motors(const std::set<uint32_t> &_motors)
    {
        std::for_each(_motors.begin(), _motors.end(), [this](int n) {
            motors.insert(n);
        });
    }
    std::set<uint32_t> const get_motors()
    {
        return motors;
    }
};

class Bend : public NetFunction
{
public:
    float operator()(float f) override
    {
        if (f < 0.9)
            return f * 1.05f;
        else
            return f / 2;
    }
};

class Rotate : public NetFunction
{
public:
    float operator()(float f) override
    {
        if (f > 0.1)
            return std::sin(f);
        else
            return 1;
    }
};

class Dashboard : public NetFunction {
public:
    //class is just a simbol for depicting engines values dashboard using opencv
    float operator()(float f) override {
            //this method should not be called.
            abort();
            return 0.0f;
    }
};

class NetFunctionFactory
{
    static std::map<std::string, std::shared_ptr<NetFunction>>
        active_net_functions;
public:
    static std::shared_ptr<NetFunction> supportNetFunction(
        const std::string &net_function_name)
    {
        if (active_net_functions.find(net_function_name) ==
            active_net_functions.end())
        {
            if (net_function_name == "bend")
            {
                active_net_functions.emplace("bend", std::make_shared<Bend>());
            }
            else if (net_function_name == "rotate")
            {
                active_net_functions.emplace("rotate",
                                             std::make_shared<Rotate>());
            }
            else if (net_function_name == "dashboard")
            {
                active_net_functions.emplace("dashboard", 
                                             std::make_shared<Dashboard>());
            }
            else 
            {
                return nullptr;
            }
        }

        return active_net_functions[net_function_name];
    }

    static std::map<std::string, std::shared_ptr<NetFunction>>
        &get_active_net_functions()
    {
        return active_net_functions;
    }
};

std::map<std::string, std::shared_ptr<NetFunction>>
    NetFunctionFactory::active_net_functions;
