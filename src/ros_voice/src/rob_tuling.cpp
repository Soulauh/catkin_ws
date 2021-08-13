#include<ros/ros.h>
#include<std_msgs/String.h>
#include<sstream>
#include<jsoncpp/json/json.h>
#include<curl/curl.h>
#include<string>
#include<exception>

using namespace std;

int flag = false;
string result;

int writer(char *data,size_t size,size_t nmemb,string *writerData) {
	if (writerData == NULL) {
		return -1;
	}
	int len = size*nmemb;
	writerData->append(data,len);
	return len;
}

int parseJsonResonse(string input) {
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(input,root);

	if (!parsingSuccessful) {
		cout<<"!!! Failed to parse the response data" <<endl;
		return 1;
	}
	const Json::Value code=root["code"];
	const Json::Value text=root["text"];
	result = text.asString();
	flag = true;
	cout << "response code: " << code << endl;
	cout << "response text: " << text << endl;
	return 0;
}

int HttpPostRequest(string input)
{
	string buffer;

	std::string strJson = "{";
	strJson += "\"key\":\"552f543c47fd48408a720f205591850d\",";
	strJson += "\"info\":";
	strJson += "\"";
	strJson += input;
	strJson += "\"";
	strJson += "}";

	cout<<"post json string: "<<strJson<<endl;
	try {
		CURL *pCurl = NULL;
		CURLcode res;
		curl_global_init(CURL_GLOBAL_ALL);
		pCurl = curl_easy_init();
		if (pCurl!=NULL) {
			curl_easy_setopt(pCurl,CURLOPT_TIMEOUT,5);
			curl_easy_setopt(pCurl,CURLOPT_URL,"http://www.tuling123.com/openapi/api");
			curl_slist *plist=curl_slist_append(NULL,"Content-Type:application/json;charset=UTF-8");
			curl_easy_setopt(pCurl,CURLOPT_HTTPHEADER,plist);
			curl_easy_setopt(pCurl,CURLOPT_POSTFIELDS,strJson.c_str());
			curl_easy_setopt(pCurl,CURLOPT_WRITEFUNCTION,writer);
			curl_easy_setopt(pCurl,CURLOPT_WRITEDATA,&buffer);
			res=curl_easy_perform(pCurl);
			if (res != CURLE_OK) {
				printf("curl_easy_perform() failed:%s\n",curl_easy_strerror(res));
			}
			curl_easy_cleanup(pCurl);
		}
		curl_global_cleanup();
	}
	catch (std::exception &ex) {
		printf("!!! curl exception %s.\n",ex.what());
	}
	if(buffer.empty())
		cout << "!!! ERROR The Tuling server response NULL" << endl;
	else
		parseJsonResonse(buffer);

	return 0;
}

void nluCallback(const std_msgs::String::ConstPtr& msg) {
	std::cout << "your question is:" << msg->data << endl;
	HttpPostRequest(msg->data);
}

int main(int argc,char **argv) {
	ros::init(argc,argv,"rob_tuling");
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("rob_iat_txt",10,nluCallback);
	ros::Publisher pub = n.advertise<std_msgs::String>("rob_tuling_txt",10);
	ros::Rate loop_rate(10);
	while(ros::ok())
	{
		if(flag) {
			std_msgs::String msg;
			msg.data = result;
			pub.publish(msg);
			flag = false;
		}
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
