#include "yaml_api.h"

//todo pimpl idium
void tasks_from_config_impl(workflow<float> &work_flow);

template <class T>
void tasks_from_config(workflow<T> &work_flow){
    tasks_from_config_impl(work_flow);
}

void join();

