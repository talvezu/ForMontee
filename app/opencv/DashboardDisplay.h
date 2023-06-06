#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>


namespace my_opencv
{

class DrawDashBoardInternals;

class DashBoard
{
protected:
    std::shared_ptr<DrawDashBoardInternals> draw_dashboard_internal_sptr;

public:
    void draw_dashboard();
    DashBoard() noexcept;
};

}; // namespace my_opencv
