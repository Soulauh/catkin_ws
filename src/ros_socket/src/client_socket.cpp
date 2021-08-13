#include "../include/client_socket.h"
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream>

using namespace cv;
using namespace std;

Mat image; 

int main(int argc, char **argv)
{
  ros::init(argc, argv, "client_socket");
  ros::NodeHandle nh;

  cv::VideoCapture capture(0);
  if(!capture.isOpened()) {
		std::cout << "video failed. " << std::endl;
		exit(1);
	}
  if ((client_socket = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
    perror("socket failed!\n");
    exit(1);
  }
  memset(&server_addr,0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(CLIENT_PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
  bzero(&(server_addr.sin_zero), 8);
  
  ros::Rate loop_rate(10);
  while (ros::ok())
  {
    capture >> image;
    if (!image.empty()) {
      vector<uchar> data_encode;
      vector<int>quality = vector<int>(2);
      quality[0] = CV_IMWRITE_JPEG_QUALITY;
      quality[1] = 50;
      imencode(".jpg", image, data_encode, quality);
      int nSize = data_encode.size();
      sendto(client_socket, &data_encode[0], nSize, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }  
    ros::spinOnce();
    loop_rate.sleep();   
  }
  close(client_socket);  
}



