#include <ros/ros.h>
#include <ros_video/video.h>
#include <ros_video/srvSwitchVideo.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream> 

using namespace cv;
using namespace std;

bool flag = false;
Mat frame;
VideoCapture capture;

bool srvSwitchVideoCallback(ros_video::srvSwitchVideo::Request &req, ros_video::srvSwitchVideo::Response &res) {
    ROS_INFO("Request: index %d ", req.index);
    flag = req.flag;
    switch (req.index) {
        case 0:
            capture.open(0);
            break;
        case 1:
            capture.open("/home/soulauh/test.avi");
            break;
        case 2:
            capture.open("/home/soulauh/tree.avi");
            break;
    }
    if(!capture.isOpened()) {
        ROS_ERROR("play video failed!");
		res.feedback = "play video failed!";
		return false;
	}
    res.frames = capture.get(CAP_PROP_FRAME_COUNT);
	res.fps = capture.get(CAP_PROP_FPS);
    res.width = capture.get(CAP_PROP_FRAME_WIDTH);
    res.height = capture.get(CAP_PROP_FRAME_HEIGHT);

    return true;
}


int main(int argc, char** argv) {

    ros::init(argc, argv, "video_pub");
    ros::NodeHandle n;
    image_transport::ImageTransport it(n);
    ros::Publisher arg_pub = n.advertise<ros_video::video>("image/arg", 10);
    ros::ServiceServer service = n.advertiseService("ros_video/switch_video", srvSwitchVideoCallback);
    image_transport::Publisher pub = it.advertise("image/local", 100);
    sensor_msgs::ImagePtr msg;

    ros::Rate loop_rate(20);
    while (ros::ok()) {
        if (flag)
            capture >> frame; 
        if (!frame.empty()) {  
            msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();  
            pub.publish(msg);  
        }    
        ros::spinOnce();  
        loop_rate.sleep();
    }
}
