#include <iostream>
#include <string>
#include <map>
#include <memory>

class NetFunction {
public:
    virtual float operator()(float f) = 0;
    virtual ~NetFunction() {}
};

class Bend : public NetFunction {
public:
    float operator()(float f) override {
        return -f;
    }
};

class Rotate : public NetFunction {
public:
    float operator()(float f) override {
        return f * f;
    }
};

class NetFunctionFactory {
    static std::map<std::string, std::shared_ptr<NetFunction>> active_net_functions;
public:
    static std::shared_ptr<NetFunction> supportNetFunction(const std::string& net_function_name) {
        if (active_net_functions.find(net_function_name) == active_net_functions.end()) {
            if (net_function_name == "bend") {
                active_net_functions.emplace("bend", std::make_shared<Bend>());
            } else if (net_function_name == "rotate") {
                active_net_functions.emplace("rotate", std::make_shared<Rotate>());
            } else {
                return nullptr;
            }
        }

        return active_net_functions[net_function_name];
    }
};

std::map<std::string, std::shared_ptr<NetFunction>> NetFunctionFactory::active_net_functions;
