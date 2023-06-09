#define CATCH_CONFIG_MAIN
#include "DB.h"
#include <gtest/gtest.h>

void set_initial_data(auto &db)
{
    auto new_engine_elements =
        std::map<uint32_t, float>{{1, 0.1f}, {2, 0.2f}, {3, 0.3f}, {4, 0.4f}};
    auto new_loop_elements = std::map<uint32_t, float>{{1, 0.1f}};

    db.set_component_db("engines", new_engine_elements);
    db.set_component_db("loop", new_loop_elements);
}

TEST(DBTests, DatabaseBaseTest)
{
    utils::DB<uint32_t, float> db;
    set_initial_data(db);
    auto eng_comp_db = db.get_component_db("engines");
    ASSERT_FLOAT_EQ(eng_comp_db.at(1), 0.1f);
    ASSERT_FLOAT_EQ(eng_comp_db.at(4), 0.4f);
}
