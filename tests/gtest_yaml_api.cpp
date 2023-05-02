#define CATCH_CONFIG_MAIN
#include <gtest/gtest.h>
#include "yaml_api.h"

TEST(YamlTests, YamlPrintFromFile)
{
    workflow<float> f("text.yaml");
    f.init();
    ASSERT_EQ (static_cast<int>(f.load_work()), 1);
}
/*
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS;
}
*/
