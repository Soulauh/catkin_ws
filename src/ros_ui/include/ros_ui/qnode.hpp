#ifndef UI_VIEW_QNODE_HPP_
#define UI_VIEW_QNODE_HPP_

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <string>
#include <sstream>
#include <QThread>
#include <QImage>
#include <QStringListModel>
#include <std_msgs/String.h>
#include <ros_video/srvSwitchVideo.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

namespace ui_view {

class QNode : public QThread {
    Q_OBJECT
public:
	QNode(int argc, char** argv );
	virtual ~QNode();
	bool init();
	void run();

	QImage get_imageLocalVideo();
	QImage get_imageSocketVideo();	
	QString get_robot_dataString();
	QString get_user_dataString();
	
	struct videoArg {
		int16_t frames;
		int16_t fps;
		int16_t size[2];
	}arg;	

	void music_control(char cmdSelect);
	void video_control(char cmdSelect);
	void videoLocalCallback(const sensor_msgs::ImageConstPtr& msg);
	void videoSocketCallback(const sensor_msgs::ImageConstPtr& msg);
	void wakeupCallback(const std_msgs::String::ConstPtr& msg);
	void iatCallback(const std_msgs::String::ConstPtr& msg);
	void ttsCallback(const std_msgs::String::ConstPtr& msg);
	void ttsokCallback(const std_msgs::String::ConstPtr& msg);

Q_SIGNALS:
    void rosShutdown();
    void updataLocal();
	void updataSocket();
	void updataUserStr();
	void updataRoboyStr();
	void updataVideoArg();

private:
	int init_argc;
	char** init_argv;
	int video_index = 1;

	QImage imageLocalVideo;
	QImage imageSocketVideo;
	
	std::string robot_dataString;
	std::string user_dataString;

	ros_video::srvSwitchVideo SwitchVideo;
	
	ros::Publisher musicControl_pub;
	ros::Subscriber wakeup_sub;
	ros::Subscriber iat_sub;
	ros::Subscriber ttsok_sub;
	ros::Subscriber videoArg_sub;
	ros::ServiceClient video_client;

	image_transport::Subscriber videoLocal_sub;
	image_transport::Subscriber videoSocket_sub;

};

}  

#endif 
