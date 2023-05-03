#include <memory>
#include "TasksFromConfig.h"
#include "PeriodicTask.h"

template <class T>
struct engine{
    T position;
};

engine<float>;

template <class T>
std::map<std::string, engine<T>> engines_map;

