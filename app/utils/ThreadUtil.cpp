#include <pthread.h>
#include <string>
#include <iostream>
#include <cassert>
#include <cstring>
#include "ThreadUtil.h"

#if defined(__APPLE__)
#else
    #include <sys/prctl.h>
#endif


std::string get_thread_name()
{
    std::string name(16, '\0');

#if defined(__APPLE__)
    if (pthread_getname_np(pthread_self(), name.data(), name.size()))
    {
        cout<<"pthread_getname_np, failed to get thread name"<<end;
        throw;
    }
#else
    if (0 != prctl(PR_GET_NAME, name.data(), 0, 0, 0))
    {
        std::cout<<"prctl, failed to get thread name\n";
        throw;
    }
#endif
    name.resize(std::strlen(name.data()));
    return name;
}

void set_thread_name(const std::string &name)
{
    assert(name.size() <= 15);
#if defined(OS_DARWIN)
    if (0 != pthread_setname_np(name.c_str()))
    {
        std::cout<<"pthread_setname_np, failed to set thread name"<<end;
        throw;
    }
#else
    if (0 != prctl(PR_SET_NAME, name.c_str(), 0, 0, 0))
    {
        std::cout<<"prctl, failed to set thread name\n";
        throw;
    }
#endif

}
