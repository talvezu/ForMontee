#include "DashboardDisplay.h"
#include <map>
#include <string>

//mock
namespace my_opencv
{

class DashBoard_mock : public DashBoard

{
public:
    std::map<std::string, std::map<uint32_t, float>> data;
    DashBoard_mock()
    {
    }
    void init()
    {
        data["engines"].emplace(2, 0.2);
        data["engines"].emplace(3, 0.4);
        data["engines"].emplace(3123, 3.4);
    }


    void draw_dashboard();
};
} //namespace my_opencv
