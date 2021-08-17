#include "../include/ros_ui/qnode.hpp"
#include "sensor_msgs/image_encodings.h"

namespace ui_view {

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
	{}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); 
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {
	ros::init(init_argc,init_argv,"ui_view");
	ros::start(); 
	ros::NodeHandle n;
	image_transport::ImageTransport it(n);
  musicControl_pub = n.advertise<std_msgs::String>("music/control", 10);
	videoLocal_sub  = it.subscribe("image/local", 100, &QNode::videoLocalCallback, this);
  videoSocket_sub  = it.subscribe("image/socket", 100, &QNode::videoSocketCallback, this);
  videoArg_sub  = n.subscribe("image/arg", 10, &QNode::videoArgCallback, this);
	wakeup_sub = n.subscribe("rob_awaken_wakeup", 10, &QNode::wakeupCallback, this);
  iat_sub    = n.subscribe("rob_iat_txt", 10, &QNode::iatCallback, this);
  ttsok_sub  = n.subscribe("rob_answer_txt", 10, &QNode::ttsokCallback, this); 
  video_client = n.serviceClient<ros_video::srvSwitchVideo>("ros_video/switch_video",this);

	start();
	return true;
}

void QNode::run() { 
  ros::spin();
  std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
  Q_EMIT rosShutdown(); 
}

QImage QNode::get_imageLocalVideo() {
    return imageLocalVideo;
}

QImage QNode::get_imageSocketVideo() {
    return imageSocketVideo;
}

QString QNode::get_robot_dataString() {
    return QString::fromUtf8(robot_dataString.c_str());
}
QString QNode::get_user_dataString() {
    return QString::fromUtf8(user_dataString.c_str());
}

void QNode::music_control(char cmdSelect) {
  
  std::string cmd;
  std_msgs::String cmd_msg;
  switch (cmdSelect) {
    case 'p': cmd = "music_op"; break;
    case 's': cmd = "music_stop"; break;
    case 'u': cmd = "music_up"; break;
    case 'd': cmd = "music_down"; break;
  }
  cmd_msg.data = cmd;
  std::cout << cmd << std::endl;
  musicControl_pub.publish(cmd_msg);
}

void QNode::video_control(char cmdSelect) {
  SwitchVideo.request.flag = true;
  switch (cmdSelect) {
    case 'v': 
      video_index = 1; 
      break;
    case 'c': 
      video_index = 0;  
      break;
    case 'u': 
      video_index--; 
      if (video_index < 1) video_index = 4;
      break;
    case 'd':  
      video_index++; 
      if (video_index > 4) video_index = 1;
      break;
  }
  SwitchVideo.request.index = video_index;
  if (video_client.call(SwitchVideo)) {
    arg.frames = SwitchVideo.response.frames;
    arg.fps = SwitchVideo.response.fps;
    arg.size[0] = SwitchVideo.response.width;
    arg.size[1] = SwitchVideo.response.height;
    Q_EMIT updataVideoArg();
  } else 
      std::cout << SwitchVideo.response.feedback << std::endl;
}

void QNode::videoLocalCallback(const sensor_msgs::ImageConstPtr &msg) {
  cv::Mat img;
  try {
    cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::RGB8);
    img = cv_ptr->image;
    imageLocalVideo = QImage(img.data,img.cols,img.rows,img.step[0],QImage::Format_RGB888);
    Q_EMIT updataLocal();
  }
  catch (cv_bridge::Exception& e) {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

void QNode::videoSocketCallback(const sensor_msgs::ImageConstPtr &msg) {
  cv::Mat img;
  try {
    cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::RGB8);
    img = cv_ptr->image;
    imageSocketVideo = QImage(img.data,img.cols,img.rows,img.step[0],QImage::Format_RGB888);
    Q_EMIT updataSocket();
  }
  catch (cv_bridge::Exception& e) {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

void QNode::videoArgCallback(const ros_video::video::ConstPtr& msg) {
  arg.frames = msg->frames;
  arg.fps = msg->fps;
  arg.size[0] = msg->size[0];
  arg.size[1] = msg->size[1];
  // std::cout << arg.frames << "  " << arg.fps << "  " << arg.size[0] <<  std::endl;
  Q_EMIT updataVideoArg();
}

void QNode::wakeupCallback(const std_msgs::String::ConstPtr& msg) {
    user_dataString = "小花小花";
    robot_dataString = "我在";
    Q_EMIT updataUserStr();
    Q_EMIT updataRoboyStr();
}

void QNode::iatCallback(const std_msgs::String::ConstPtr& msg) {
    user_dataString = msg->data;
    // std::cout << user_dataString.c_str() << std::endl;
    Q_EMIT updataUserStr();
}

void QNode::ttsokCallback(const std_msgs::String::ConstPtr& msg) {
    robot_dataString = msg->data;
    // std::cout << robot_dataString.c_str() << std::endl;
    Q_EMIT updataRoboyStr();
}



}  
