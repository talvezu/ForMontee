#pragma once
#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace my_opencv
{

class VideoCaptureInternals;
using clock = std::chrono::steady_clock;
using tp = clock::time_point;
class VideoCapture
{
    // Define the output video file name
    std::string outfile;
    std::shared_ptr<VideoCaptureInternals> video_capture_sptr;

public:
    VideoCapture(std::string _outfile);
    int capture_video_from_camera(std::optional<tp> timeout);
    bool capture_single_frame_from_camera();
};

}; // namespace my_opencv
