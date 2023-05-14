#include <gtest/gtest.h>
#include <memory>
#include <map>
#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "SimpleBoostSharedMemory.h"
#include "string.h"
#include <cstring>
#include <string>
#include <variant>
#include <utility>

std::map<std::string, boost::interprocess::shared_memory_object> shared_memory_map_mock;

class my_class {
public:
    int i;
    my_class(){}
    my_class(int _i):i(_i){

    }
};
class my_complex_class {
public:
    std::variant<std::pair<int, std::string>, std::array<char, 100>> mem;

    my_complex_class() {}

    my_complex_class(const std::pair<int, std::string>& pair) : mem(pair) {}

    my_complex_class(const std::array<char, 100>& arr) : mem(arr) {}

    my_complex_class& operator=(const my_complex_class& other) {
        if (this != &other) {
            mem = other.mem;
        }
        return *this;
    }
};

template <>
bool SimpleBoostSharedMemory::open_or_create<my_class>(std::string shared_memory_id){

        if (shared_memory_map_mock.find(shared_memory_id) != shared_memory_map_mock.end())
            return false;

        shared_memory_map_mock.emplace(shared_memory_id,boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, shared_memory_id.c_str(), boost::interprocess::read_write));
        shared_memory_map_mock[shared_memory_id].truncate(sizeof(my_class));
        return true;
}

template <>
void SimpleBoostSharedMemory::get_shared_memory<my_class>(std::string shared_memory_id, my_class& obj){
    auto sm_element = shared_memory_map_mock.find(shared_memory_id);
    if (sm_element == shared_memory_map_mock.end())
        return;

    boost::interprocess::mapped_region region(sm_element->second, boost::interprocess::read_write);
    obj = *(static_cast<my_class*>(region.get_address()));
}

template <>
void SimpleBoostSharedMemory::update_shared_memory<my_class>(std::string shared_memory_id, const my_class& obj){
    auto sm_element = shared_memory_map_mock.find(shared_memory_id);
    if (sm_element == shared_memory_map_mock.end())
        return;

    // Map the shared memory object into the process's address space
    boost::interprocess::mapped_region region(shared_memory_map_mock[shared_memory_id], boost::interprocess::read_write);
    *(static_cast<my_class*>(region.get_address())) = obj;
}

template<>
bool SimpleBoostSharedMemory::delete_sm<my_class>(std::string shared_memory_id){
    if (shared_memory_map_mock.find(shared_memory_id) == shared_memory_map_mock.end())
        return false;

    shared_memory_map_mock.erase(shared_memory_id);
    return true;
}

TEST(SharedMemoryTest, test_basic)
{
    ASSERT_TRUE(SimpleBoostSharedMemory::open_or_create<my_class>("page1"));
    ASSERT_FALSE(SimpleBoostSharedMemory::open_or_create<my_class>("page1"));
}

TEST(SharedMemoryTest, test_read_write_and_verify)
{
    std::string s{"test for shared memory"};

    my_class shm, shm_get;
    SimpleBoostSharedMemory::get_shared_memory<my_class>("page1", shm);
    for (int i=1; i<10; i++)
    {
        shm.i = i;
        SimpleBoostSharedMemory::update_shared_memory<my_class>("page1", shm);

        SimpleBoostSharedMemory::get_shared_memory<my_class>("page1", shm_get);
        ASSERT_EQ(i, shm_get.i);
    }

    ASSERT_TRUE(SimpleBoostSharedMemory::open_or_create<my_class>("page2"));
    ASSERT_TRUE(SimpleBoostSharedMemory::delete_sm<my_class>("page1"));
    ASSERT_TRUE(SimpleBoostSharedMemory::delete_sm<my_class>("page2"));;
}


template <>
bool SimpleBoostSharedMemory::open_or_create<my_complex_class>(std::string shared_memory_id){

        if (shared_memory_map_mock.find(shared_memory_id) != shared_memory_map_mock.end())
            return false;

        shared_memory_map_mock.emplace(shared_memory_id,boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, shared_memory_id.c_str(), boost::interprocess::read_write));
        shared_memory_map_mock[shared_memory_id].truncate(sizeof(my_complex_class));
        return true;
}

template <>
void SimpleBoostSharedMemory::get_shared_memory<my_complex_class>(std::string shared_memory_id, my_complex_class& obj){
    auto sm_element = shared_memory_map_mock.find(shared_memory_id);
    if (sm_element == shared_memory_map_mock.end())
        return;

    boost::interprocess::mapped_region region(sm_element->second, boost::interprocess::read_write);
    obj = *(static_cast<my_complex_class*>(region.get_address()));
}

template <>
void SimpleBoostSharedMemory::update_shared_memory<my_complex_class>(std::string shared_memory_id, const my_complex_class& obj){
    auto sm_element = shared_memory_map_mock.find(shared_memory_id);
    if (sm_element == shared_memory_map_mock.end())
        return;

    // Map the shared memory object into the process's address space
    boost::interprocess::mapped_region region(shared_memory_map_mock[shared_memory_id], boost::interprocess::read_write);
    *(static_cast<my_complex_class*>(region.get_address())) = obj;
}

template<>
bool SimpleBoostSharedMemory::delete_sm<my_complex_class>(std::string shared_memory_id){
    if (shared_memory_map_mock.find(shared_memory_id) == shared_memory_map_mock.end())
        return false;

    shared_memory_map_mock.erase(shared_memory_id);
    return true;
}



TEST(SharedMemoryTest, test_read_write_and_verify_complex)
{
    my_complex_class shm, shm_get;
    ASSERT_TRUE(SimpleBoostSharedMemory::open_or_create<my_complex_class>("page1"));
    SimpleBoostSharedMemory::get_shared_memory<my_complex_class>("page1", shm);
    for (int i=1; i<10; i++)
    {
        auto p = std::get_if<std::pair<int, std::string>>(&shm.mem);
        p->first = i;
        p->second = std::string("element_")+std::to_string(i);

        SimpleBoostSharedMemory::update_shared_memory<my_complex_class>("page1", shm);

        SimpleBoostSharedMemory::get_shared_memory<my_complex_class>("page1", shm_get);

        auto p2 = std::get_if<std::pair<int, std::string>>(&shm_get.mem);
        ASSERT_EQ(i, p2->first);
        ASSERT_EQ(std::string("element_")+std::to_string(i), p2->second);
    }
    ASSERT_TRUE(SimpleBoostSharedMemory::delete_sm<my_class>("page1"));
}

