#include <map>
#include <memory>

namespace SimpleBoostSharedMemory
{
    template<class T>
    bool open_or_create(std::string shared_memory_id);

    template<class T>
    bool delete_sm(std::string shared_memory_id);

    template<class T>
    void get_shared_memory(std::string shared_memory_id, T& obj);

    template<class T>
    void update_shared_memory(std::string shared_memory_id, const T& update);

};
