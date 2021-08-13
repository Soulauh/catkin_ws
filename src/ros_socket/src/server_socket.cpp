#include "../include/server_socket.h"
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream>

cv::Mat image; 

int main(int argc, char **argv)
{
  ros::init(argc, argv, "server_socket");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("image/socket", 1);
  sensor_msgs::ImagePtr imageMsg;

  memset(&server_addr,0,sizeof(server_addr));
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(SERVER_PORT);
  if ((server_socket = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
    perror("socket failed!\n");
    exit(1);
  }
  std::cout<<"creat socket successful! socket="<<server_socket<<std::endl;
  if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
    perror("bind error!\n");
    exit(1);
  } 
  listen(server_socket, LENGTH_OF_LISTEN_QUEUE);
  std::cout << "Listening..." << std::endl;
  ros::Rate loop_rate(10);
  while (ros::ok())
  {
    length = sizeof(client_socket);
    int n=recvfrom(server_socket,buffer,sizeof(buffer),0,(struct sockaddr*)&client_socket,&length);
    if ( n < 0) {
      printf("Server Accept Failed!\n");
      break;
    }
    decodeImage(n);
    imageMsg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image).toImageMsg();
    pub.publish(imageMsg);
    ros::spinOnce();
    loop_rate.sleep();   
  }  
}

void decodeImage(int n)
{      
  std::vector<uchar> decode(&buffer[0], &buffer[n]);	
  image = cv::imdecode(decode, CV_LOAD_IMAGE_COLOR);
  memset(buffer, 0, sizeof(buffer));
  // cv::imshow("server_recvive",image);
  // cv::waitKey(1);
}



