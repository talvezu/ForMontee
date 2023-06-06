#define CATCH_CONFIG_MAIN
#include "yaml_api.h"
#include <gtest/gtest.h>

TEST(YamlTests, YamlPrintFromFile)
{
    workflow<float> f("../tests/text.yaml");
    f.init();
    ASSERT_EQ(static_cast<int>(f.load_work()), 1);

    const auto &actions = f.get_net_actions();

    ASSERT_EQ(actions.at("net_action_1").action, "rotate");
    ASSERT_EQ(actions.at("net_action_2").action, "bend");

    auto tasks = f.get_tasks();

    std::set<int> motors{1, 2};
    for (auto motor : tasks["net_function_1"]->target_motors)
    {
        motors.erase(motor);
    }
    ASSERT_EQ(motors.size(), 0);

    std::set<float> values{0.5f, 0.4f, 0.3f, 0.6f};
    for (float delta_value : *(tasks["net_function_2"]->values))
    {
        values.erase(delta_value);
    }
    ASSERT_EQ(values.size(), 0);
}
/*
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS;
}
*/
