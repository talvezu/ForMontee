#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <ostream>
#include <optional>
#include <typeinfo>
using std::string;
using std::vector;
using std::ostream;
using std::shared_ptr;
using Clock = std::chrono::steady_clock;
using time_point = std::chrono::time_point<Clock>;

class PriorityQueueElement: public std::enable_shared_from_this< PriorityQueueElement >{
    //todo getters and setters friend function etc..
public:
    vector<string> target_motors;
    int64_t occurrences; // -1 for endless
    std::vector<float> values;
    std::vector<float> deltas;     //values
    uint64_t invokation_delta;     //for the array should be larget than the largest value
    uint32_t idx;
    time_point next_invokation;

    //https://stackoverflow.com/questions/9814345/cant-overload-operator-as-member-function


    PriorityQueueElement(std::initializer_list<string> _targets,
                        int64_t _occurrences,
                        std::initializer_list<float> _values,
                        std::initializer_list<float> _deltas,
                        uint64_t _invokation_delta) :
                        target_motors(_targets),
                        occurrences(_occurrences),
                        values(_values),
                        deltas(_deltas),
                        invokation_delta(_invokation_delta)
    {
        idx = 0;
        next_invokation = Clock::now() + std::chrono::milliseconds(static_cast<int>(invokation_delta));
    }



    PriorityQueueElement(vector<string> _targets,
                        int64_t _occurrences, //
                        std::vector<float> _values,
                        std::vector<float> _deltas,
                        uint64_t _invokation_delta) :
                        target_motors(_targets),
                        occurrences(_occurrences),
                        values(_values),
                        deltas(_deltas),
                        invokation_delta(_invokation_delta)
    {
        idx = 0;
        next_invokation = Clock::now() + std::chrono::milliseconds(static_cast<int>(invokation_delta));
    }

    //std::optional<time_point> get_next_invokation()
    shared_ptr<PriorityQueueElement> get_rescheduled_item_if_any()
    {
        if (occurrences == -1 || (occurrences>0 && --occurrences))
        {
            /*consider instead of += Clock::now()*/
            next_invokation += std::chrono::milliseconds(static_cast<int>(invokation_delta));
            return shared_from_this();
        }
        return shared_ptr<PriorityQueueElement>();
    }

    float get_value(){
        //for setting to engine
        auto &ret = values[idx];
        if ( deltas.size()){
            for(unsigned int i=0; i<static_cast<unsigned int>(values.size()); i++) {
               values[i] += deltas[i];
            }
        }

        if ( ++idx == static_cast<uint32_t>(values.size()))
            idx = 0;

        return ret;
    }
};

ostream& operator<< (ostream& os,const PriorityQueueElement& element){
    os << "targets:\n";
    for (auto target: element.target_motors)
    {
        os << "\t:" << target;
    }
    os << "\n";
    os << "occurences : " <<element.occurrences<<"\n";

    os << "values:\n";
    for (auto value: element.values)
    {
        os << "\t:" << value;
    }
    os << "\n";

    if (element.deltas.size())
    {
        os << "deltas values:\n";
        for (auto delta: element.deltas)
        {
            os << "\t:" << delta;
        }
        os << "\n";
    }
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
