#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <ostream>

using std::string;
using std::vector;
using std::ostream;
using std::shared_ptr;
using Clock = std::chrono::steady_clock;
using time_point = std::chrono::time_point<Clock>;

class PriorityQueueElement{
    //todo getters and setters friend function etc..
public:
    vector<string> target_motors;
    bool is_repeatable;
    uint64_t occurances;
    std::vector<float> values;
    std::vector<float> deltas;     //values
    uint64_t invokation_delta;     //for the array should be larget than the largest value
    int idx;
    time_point next_invokation;

    //https://stackoverflow.com/questions/9814345/cant-overload-operator-as-member-function


    PriorityQueueElement(std::initializer_list<string> _targets,
                        bool _is_repeatable,
                        uint64_t _occurances,
                        std::initializer_list<float> _values,
                        std::initializer_list<float> _deltas,
                        uint64_t _invokation_delta) :
                        target_motors(_targets),
                        is_repeatable(_is_repeatable),
                        occurances(_occurances),
                        values(_values),
                        deltas(_deltas),
                        invokation_delta(_invokation_delta)
    {
        idx = 0;
        next_invokation = Clock::now() + std::chrono::milliseconds(static_cast<int>(invokation_delta));
    }


    PriorityQueueElement(vector<string> _targets,
                        bool _is_repeatable,
                        uint64_t _occurances,
                        std::vector<float> _values,
                        std::vector<float> _deltas,
                        uint64_t _invokation_delta) :
                        target_motors(_targets),
                        is_repeatable(_is_repeatable),
                        occurances(_occurances),
                        values(_values),
                        deltas(_deltas),
                        invokation_delta(_invokation_delta)
    {
        idx = 0;
        next_invokation = Clock::now() + std::chrono::milliseconds(static_cast<int>(invokation_delta));
    }

};

ostream& operator<< (ostream& os,const PriorityQueueElement& element){
    os << "targets:\n";
    for (auto target: element.target_motors)
    {
        os << "\t:" << target;
    }
    os << "\n";
    os << "is_repeatable : " <<((element.is_repeatable)?"true":"false")<<"\n";
    os << "occurences : " <<element.occurances<<"\n";

    os << "values:\n";
    for (auto value: element.values)
    {
        os << "\t:" << value;
    }
    os << "\n";

    os << "values:\n";
    for (auto delta: element.deltas)
    {
        os << "\t:" << delta;
    }
    os << "\n";
    os << "invokation_delta : " <<element.invokation_delta<<"\n";


    return os;
};


class my_compare{
    //https://stackoverflow.com/questions/16111337/declaring-a-priority-queue-in-c-with-a-custom-comparator
public:
    bool operator() (const shared_ptr<PriorityQueueElement>& lhs, const shared_ptr<PriorityQueueElement>& rhs){
        return (lhs->next_invokation > rhs->next_invokation);
    };
};
