#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>   
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "ros/ros.h"
#include "std_msgs/String.h"

const int que_ans_num = 1024;//问题数目
const std::string txtfilename = "/home/soulauh/catkin_ws/src/ros_voice/data/que_ans_data.txt";//问答文件路径

bool wakeupflag = false;
bool iatokflag = false;
bool tulingokflag = false;

std::string wakeup_dataString;
std::string iat_dataString;
std::string tuling_dataString;

//特殊命令-----------------------------------------------------------//
const std::string iat_con_strdata[] = {"播放音乐","下一首","上一首","停止","000"};
const std::string iat_con_resdata[] = {"正在播放","正在播放","正在播放","已停止","000"};
const std::string iat_con_condata[] = {"music_op","music_down","music_up","music_stop","000"};

static void sleep_ms(size_t ms) {
	usleep(ms*1000);
}

void wakeupFlag(const std_msgs::String::ConstPtr& msg) {
	wakeup_dataString = "我在";
	wakeupflag = true;
}

void iatFlag(const std_msgs::String::ConstPtr& msg) {
	iat_dataString = msg->data;
	iatokflag = true;
}

void tulingFlag(const std_msgs::String::ConstPtr& msg) {
	tuling_dataString = msg->data;
	tulingokflag = true;
}


void flietxt_in(std::string filename, std::string iat_question[], std::string iat_answer[]) {
	std::ifstream ifile(filename.c_str());
	if(ifile.is_open()) {
		int i = 0;
		int num = 0;
		char fdataCK;
		char fdata[256] = {};
		while((fdataCK = ifile.peek()) != ';') {
			while((fdataCK = ifile.peek()) != ':')
				ifile.get(fdata[i++]);
			iat_question[num] = std::string(fdata);
			ifile.get(fdata[i]);
			memset(fdata,'\0',sizeof(fdata));
			i = 0;
			while((fdataCK = ifile.peek()) != ',')
				ifile.get(fdata[i++]);
			iat_answer[num++] = std::string(fdata);
			ifile.get(fdata[i]);
			memset(fdata,'\0',sizeof(fdata));
			i = 0;
			if(num>que_ans_num)
				break;
		}
	}
	ifile.close();
}

std::string to_string(int val) {
    char buf[20];
    sprintf(buf, "%d", val);
    return std::string(buf);
}

std::string iat_timeget(void) {
    struct tm *ptm; 
    long ts; 
    ts = time(NULL); 
    ptm = localtime(&ts); 
    std::string string = "现在是北京时间" + to_string(ptm-> tm_hour) + "点" + to_string(ptm-> tm_min) + "分";
	return std::string(string);
}

std::string iat_datahandle(std::string stringData, std::string iat_stringData[], std::string iat_resultString[]) {
	std::string::size_type resstring = 0;
	std::string iatresult; 
	for(int i = 0;iat_stringData[i] != "000";i++) {
		if ((resstring = stringData.find(iat_stringData[i])) != std::string::npos) {
			if(i==0) iatresult = iat_timeget();
			else iatresult = iat_resultString[i];	

			return iatresult;			
		} else
			iatresult = "notFound";
	}
	return iatresult;
}

std::string con_datahandle(const std::string iat_controlString[], int consele) {
	std::string conresult;
	conresult = iat_controlString[consele];
	return conresult;
}

std::string iat_controlhandle(std::string stringData, const std::string iat_stringData[], const std::string iat_resultString[], int *sele) {
	std::string::size_type resstring = 0;
	std::string iatresult; 
	for(int i = 0; iat_stringData[i] != "000"; i++) {
		if((resstring = stringData.find(iat_stringData[i])) != std::string::npos) {
			iatresult = iat_resultString[i];
			*sele = i;
			return iatresult;
		} else
			iatresult = "notFound";
	}
	return iatresult;
}


int main(int argc, char* argv[])
{
    ros::init(argc, argv, "rob_control");
	int selectnum;
	ros::NodeHandle n;	
	std::string answer_str;
	std::string control_str;
	std::string iat_questdata[que_ans_num];
	std::string iat_ansdata[que_ans_num];
	std_msgs::String answer_msg;
	std_msgs::String control_msg;

	ros::Subscriber wakeup_sub = n.subscribe("rob_awaken_wakeup", 10, wakeupFlag);
	ros::Subscriber iat_sub = n.subscribe("rob_iat_txt", 10, iatFlag);
	ros::Subscriber tuling_sub = n.subscribe("rob_tuling_txt", 10, tulingFlag);
	ros::Publisher answer_pub = n.advertise<std_msgs::String>("rob_answer_txt", 10);
	ros::Publisher music_pub = n.advertise<std_msgs::String>("rob_music_control", 10);

	flietxt_in(txtfilename, iat_questdata, iat_ansdata);	
	std::cout << "现在用‘小花小花’即可唤醒我" << std::endl;

	ros::Rate loop_rate(10);
	while(ros::ok()) 
	{
		if (wakeupflag) {
			wakeupflag = false;
			answer_msg.data = wakeup_dataString;
			std::cout << "robot say: " << wakeup_dataString << std::endl;
			answer_pub.publish(answer_msg);
		} 
		if (iatokflag) {
			iatokflag = false;
			answer_str = iat_datahandle(iat_dataString, iat_questdata, iat_ansdata);

			if (answer_str == "notFound") {
				answer_str = iat_controlhandle(iat_dataString, iat_con_strdata, iat_con_resdata, &selectnum);
				if (answer_str != "notFound") {
					control_str = con_datahandle(iat_con_condata, selectnum);					
    				control_msg.data = control_str.c_str();					
					if (selectnum <= 4)
						music_pub.publish(control_msg);									
				} else {
					sleep_ms(500);
					ros::spinOnce();
					if (tulingokflag) {
						tulingokflag = false;
						answer_str = tuling_dataString;
					} else 
						answer_str = "这个问题我还没学会哦";				
				}					
			}
			std::cout << "user  say: " << iat_dataString << std::endl;
			std::cout << "robot say: " << answer_str << std::endl;
    		answer_msg.data = answer_str.c_str();
			answer_pub.publish(answer_msg);									
		}	
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}

