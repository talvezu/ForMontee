#include <opencv2/opencv.hpp>
#include "VideoCapture.h"
#include "../utils/global_settings.h"

namespace my_opencv{
class VideoCaptureInternals{
public:
    std::string outfile;
    std::shared_ptr<cv::VideoCapture> cap;
    std::shared_ptr<cv::VideoWriter> writer;
    int frameWidth;
    int frameHeight;
    cv::Mat curr_frame;
    bool initialized;

    bool init(std::string _outfile){
        outfile = _outfile;
        cap = std::make_shared<cv::VideoCapture>(0);
        if (!cap->isOpened()) {
            std::cout << "Failed to open the camera!" << std::endl;
            return false;
        }
        // Get the default camera's resolution
        frameWidth = static_cast<int>(cap->get(cv::CAP_PROP_FRAME_WIDTH));
        frameHeight = static_cast<int>(cap->get(cv::CAP_PROP_FRAME_HEIGHT));
        writer = std::make_shared<cv::VideoWriter>(outfile, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25.0, cv::Size(frameWidth, frameHeight));

        return true;
    }
    ~VideoCaptureInternals()
    {
        // Release the video capture and writer objects
        cap->release();
        writer->release();

        // Close all OpenCV windows
        //cv::destroyAllWindows();
    }
    VideoCaptureInternals(){
        initialized = false;
    }
};

VideoCapture::VideoCapture(std::string _outfile):outfile(_outfile)
{
    video_capture_sptr = std::make_shared<VideoCaptureInternals>();
}

int VideoCapture::capture_video_from_camera(std::optional<tp> timeout) {

    // Open the default camera
    if (!video_capture_sptr->initialized && !video_capture_sptr->init(outfile))
        return -1;

    auto cap = *video_capture_sptr->cap;
    if (!cap.isOpened()) {
        std::cout << "Failed to open the camera!" << std::endl;
        return -1;
    }

    // Create a MPEG video writer object 25fps
    auto writer = *video_capture_sptr->writer;

    if (!writer.isOpened()) {
        std::cout << "Failed to create the output video file!" << std::endl;
        return -1;
    }

    // Start capturing and recording
    auto &frame = video_capture_sptr->curr_frame;

    while (true) {
        if (timeout.has_value())
        {
            if(timeout.value() > clock::now())
                break;
        }
        // Read a frame from the camera
        cap.read(frame);

        // Check if the frame is empty (end of video stream)
        if (frame.empty())
            break;

        // Write the frame to the output video file
        writer.write(frame);

        // Display the captured frame
        cv::imshow("Camera", frame);

        // Check for 'q' key press to exit
        if (cv::waitKey(1) == 'q')
            break;
    }


    return 0;
}

bool VideoCapture::capture_single_frame_from_camera() {
    // Open the default camera
    if (!video_capture_sptr->initialized && !video_capture_sptr->init(outfile))
        return false;

    auto &cap = *video_capture_sptr->cap;
    if (!cap.isOpened()) {
        std::cout << "Failed to open the camera!" << std::endl;
        return false;
    }


    // Get the default camera's resolution
    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    // Create a MPEG video writer object 25fps
    auto &writer = *video_capture_sptr->writer;

    if (!writer.isOpened()) {
        std::cout << "Failed to create the output video file!" << std::endl;
        return -1;
    }

    auto &frame = video_capture_sptr->curr_frame;
    cap.read(frame);
    // Check if the frame is empty (end of video stream)
    if (frame.empty())
        return false;

    return true;
}

}; //namespace my_opencv
