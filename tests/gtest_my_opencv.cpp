#include "VideoCapture.h"
#include "config.hpp"
#include "mocking.h"
#include <gtest/gtest.h>

TEST(OpenCVTests, VideoCapture)
{
    if (wsl_env)
        GTEST_SKIP() << "Skipping when on wsl";

    auto vc = my_opencv::VideoCapture("temp_capture.avi");
    //vc.capture_video_from_camera(std::nullopt);
    auto tp = my_opencv::clock::now() + std::chrono::milliseconds(100);
    vc.capture_video_from_camera(tp);
    GTEST_ASSERT_EQ(1, 1);
}


TEST(OpenCVTests, displayDashboard)
{
    auto dashb = my_opencv::DashBoard_mock();
    dashb.draw_dashboard();
    GTEST_ASSERT_EQ(1, 1);
}
