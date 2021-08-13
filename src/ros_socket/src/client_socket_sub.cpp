#include "../include/client_socket.h"
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream>

using namespace cv;
using namespace std;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)  
{   
  Mat img;
  try {  
    cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::BGR8);
    img = cv_ptr->image;

    if (!img.empty()) {
      vector<uchar> data_encode;
      vector<int>quality = vector<int>(2);
      quality[0] = CV_IMWRITE_JPEG_QUALITY;
      quality[1] = 50;
      imencode(".jpg", img, data_encode, quality);
      int nSize = data_encode.size();
      sendto(client_socket, &data_encode[0], nSize, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }    
  }
  catch (cv_bridge::Exception& e) {  
    ROS_ERROR("cv_bridge exception: %s", e.what());  
    return;  
  }    
  imshow("LocalWindow", img);
  waitKey(2);
}  

int main(int argc, char **argv)
{
  ros::init(argc, argv, "client_socket_sub");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);  
  image_transport::Subscriber sub = it.subscribe("camera/color/image_raw", 1000, imageCallback);
  namedWindow("LocalWindow", CV_WINDOW_NORMAL);   
  startWindowThread();

  if ((client_socket = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
    perror("socket failed!\n");
    exit(1);
  }
  memset(&server_addr,0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(CLIENT_PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
  bzero(&(server_addr.sin_zero), 8);
  ros::spin();
  close(client_socket);
  destroyWindow("LocalWindow");   
}




