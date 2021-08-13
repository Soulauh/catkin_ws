#include <ros/ros.h>  
#include <image_transport/image_transport.h>  
#include <opencv2/highgui/highgui.hpp>  
#include <cv_bridge/cv_bridge.h>  

void imageCallback(const sensor_msgs::ImageConstPtr& msg)  
{ 
  cv::Mat img;  
  try {  
    cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::BGR8);
    img = cv_ptr->image; 
  }
  catch (cv_bridge::Exception& e) {  
    ROS_ERROR("cv_bridge exception: %s", e.what());  
    return;  
  }    
  cv::imshow("LocalWindow", img);
  cv::waitKey(3);
}  

int main(int argc, char **argv) {  
  ros::init(argc, argv, "video_sub");  
  ros::NodeHandle nh;  
  cv::namedWindow("LocalWindow", CV_WINDOW_NORMAL);   
  cv::startWindowThread();  
  image_transport::ImageTransport it(nh);  
  image_transport::Subscriber sub = it.subscribe("image/local", 1000, imageCallback);
  ros::spin();  
  cv::destroyWindow("LocalWindow");  
  return 0;
} 
