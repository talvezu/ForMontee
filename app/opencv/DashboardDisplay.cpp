#include "DashboardDisplay.h"
#include "DB.h"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
using Mat = cv::Mat;
using Point = cv::Point;
using Scalar = cv::Scalar;
using namespace std;


namespace my_opencv
{

class DrawDashBoardInternals
{
public:
    std::shared_ptr<utils::DB<uint32_t, float>> db_sptr;
    DrawDashBoardInternals()
    {
        db_sptr = utils::get_db<uint32_t, float>();
    }
};

DashBoard::DashBoard() noexcept
{
    draw_dashboard_internal_sptr = std::make_shared<DrawDashBoardInternals>();
}

void DashBoard::draw_dashboard()
{
    // Blank Image
    //auto db = draw_dashboard_internal_sptr->db_sptr->get_component_db("engines");
    auto db = draw_dashboard_internal_sptr->db_sptr->get_all_values();
    int radius = 75;
    int i = 0;
    if (auto iter = db.find("Engines"); iter != db.end())
    {

        int count = iter->second.size();
        int sizex = 4 * radius;
        int sizey = 4 * radius + 3 * (count - 1) * radius;
        int posy = sizex / 2;

        Mat img(sizex, sizey, CV_8UC3, Scalar(255, 255, 255));
        for (auto &[k, v] : iter->second)
        {
            int posx = radius * 2 + i * 3 * radius;
            circle(img,
                   Point(posx, posy),
                   radius,
                   Scalar(0, 69, 255),
                   cv::FILLED);

            int rect_height = 0.4 * radius;
            int rect_width = radius / 2;
            int rect_x_start = posx - (0.7 * radius);
            int rect_y_start = (0.1 * radius) + posy;
            cv::rectangle(
                img,
                Point(rect_x_start, rect_y_start),
                Point(rect_x_start + 1.4 * radius, rect_y_start + rect_height),
                Scalar(255, 255, 255),
                cv::FILLED);
            //std::string text = to_string(k) + string(" : ") + to_string(v).substr(0, 8);
            std::string text = to_string(v);
            putText(img,
                    text,
                    Point(rect_x_start, rect_y_start * 1.1),
                    cv::FONT_HERSHEY_DUPLEX,
                    0.5,
                    Scalar(0, 0, 0),
                    0.2);

            text = "engine: " + to_string(i);
            putText(img,
                    text,
                    Point(posx - radius * 0.5, posy),
                    cv::FONT_HERSHEY_DUPLEX,
                    0.7,
                    Scalar(0, 0, 0),
                    2);
            i++;
        }
        imshow("Dashboard", img);
        int key = cv::waitKey(1);
    }

} //class DashBoard
} //namespace my_opencv
