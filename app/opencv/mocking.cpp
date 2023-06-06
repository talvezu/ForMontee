//mock
#include "mocking.h"
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

void DashBoard_mock::draw_dashboard()
{
    // Blank Image
    int radius = 75;
    int count = 5;

    int sizex = 4 * radius;
    int sizey = 4 * radius + 3 * (count - 1) * radius;

    Mat img(sizex, sizey, CV_8UC3, Scalar(255, 255, 255));

    int posy = sizex / 2;

    for (int i = 0; i < count; i++)
    {
        int posx = radius * 2 + i * 3 * radius;
        circle(img, Point(posx, posy), radius, Scalar(0, 69, 255), cv::FILLED);

        int rect_height = 0.4 * radius;
        int rect_width = radius / 2;
        int rect_x_start = posx - (0.6 * radius);
        int rect_y_start = (0.1 * radius) + posy;
        cv::rectangle(
            img,
            Point(rect_x_start, rect_y_start),
            Point(rect_x_start + 1.2 * radius, rect_y_start + rect_height),
            Scalar(255, 255, 255),
            cv::FILLED);
        std::string text = to_string(rand());
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
                1);
    }

    imshow("Image", img);
    //unmark for actually have the option to look at the dashboard generated.
    //cv::waitKey(0);
}


} //namespace my_opencv
