#pragma once

#include <mutex>
#include <unordered_map>
#include <memory>
#include <iostream>
#include "logging.h"


template <typename T>
concept HasActMethod = requires(T obj)
{
    { obj.act() } -> std::same_as<void>;
};

template<class HasActMethod >
class Storage{
public:
    mutable std::mutex m;
    static uint32_t total_count;
   	enum class component : uint32_t { engine_1 = 1 << 0, engine_2 = 1 << 1, engine_3 = 1 << 2 };
    std::unordered_map<uint32_t, HasActMethod > DB;
	void write_entry([[maybe_unused]] std::shared_ptr<HasActMethod > entry, [[maybe_unused]] uint32_t comp)
    {
        //DB[comp].counter++;
        DB[comp] = *entry;
    }

	void read_entry([[maybe_unused]] std::shared_ptr<HasActMethod > entry, [[maybe_unused]] uint32_t comp)
    {
        *entry = DB[comp];
    }

	void process([[maybe_unused]] std::shared_ptr<HasActMethod > entry, [[maybe_unused]] uint32_t comp)
    {
        entry->act();
        std::unique_lock<std::mutex> l(m);
        total_count += 1;
        if (logging::active)
            std::cout<<total_count<<std::endl;
    }

    uint32_t get_total_count()
    {
        return total_count;
    }

};

template<class HasActMethod >
uint32_t Storage<HasActMethod >::total_count = 0;
