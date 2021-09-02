#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <ros/ros.h> 
#include <std_msgs/String.h>

using namespace std;

bool music_open = false;
bool music_playing = false;
int music_con = 0;

vector<string> file_name;
string path = "/home/soulauh/.material/music";

void GetFileNames(string path, vector<string>& filenames) {
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str())))
        return;
    while((ptr = readdir(pDir))!=0) {
        if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
            filenames.push_back("mplayer -quiet -slave -input file=/tmp/cmd_music " + path + "/" + ptr->d_name);
    }
    closedir(pDir);
}



void music_play_data(const std_msgs::String::ConstPtr& msg) {
    string musicString = msg->data;
    if(musicString.compare("music_op") == 0)
        music_open = true;
    if(musicString.compare("music_stop") == 0)
        music_con = 1;
    if(musicString.compare("music_down") == 0)
        music_con = 2;
    if(musicString.compare("music_up") == 0)
        music_con = 3;
}

FILE *music_mplayer(vector<string>& filenames, int playmuns) {
    const char* mp;
    FILE *fp = NULL;
    mp = filenames[playmuns].c_str();
    unlink("/tmp/cmd_music");
   	mkfifo("/tmp/cmd_music", 0777);
    fp = popen(mp, "r");

    int fd = fileno(fp);
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    system("echo \"get_percent_pos\n\" > /tmp/cmd_music");

    return fp;
}

void music_stop_con(FILE *confile) {
    system("echo \"stop\n\" > /tmp/cmd_music");
    sleep(0.2);
    pclose(confile);

    music_con = 0;
    music_playing = false;
}

void music_down_con(FILE *confile, int *mun, int con) {
    system("echo \"stop\n\" > /tmp/cmd_music");
    sleep(0.2);
    pclose(confile);

    *mun = *mun+1;
    if(*mun > con)
        *mun = 0;

    music_con = 0;
    music_playing = false;
    music_open = true;
}

void music_up_con(FILE *confile, int *mun, int con) {
    system("echo \"stop\n\" > /tmp/cmd_music");
    sleep(0.2);
    pclose(confile);

    *mun = *mun-1;
    if(*mun < 0)
        *mun = con;

    music_con = 0;
    music_playing = false;
    music_open = true;
}

void music_file_con(FILE *confile, char *buf, int *mun, int con) {
    if(!feof(confile))
        fgets(buf, 256, confile);
    else {
        *mun = *mun+1;
        if(*mun > con)
            *mun = 0;
        music_open = true;
        music_playing = false;
        pclose(confile);
        cout << "file over" << endl;
    }
    music_con = 0;
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "rob_music");
    ros::NodeHandle n;

    ros::Subscriber sub = n.subscribe("rob_music_control", 10, music_play_data);
    ros::Subscriber ui_sub = n.subscribe("music/control", 10, music_play_data);
    GetFileNames(path, file_name);
    int playcon = (file_name.size())-1;
    int playmun = 0;
    cout << "file ok:" << playcon << endl;

    char musicbuf[256];

    while(ros::ok()) 
    {
        if(music_open) {
            music_open = false;

            FILE *musicfile = NULL;
            musicfile = music_mplayer(file_name, playmun);

            music_playing = true;

            while(music_playing) {
                switch(music_con) {
                    case 1: music_stop_con(musicfile); break;
                    case 2: music_down_con(musicfile, &playmun, playcon); break;
                    case 3: music_up_con(musicfile, &playmun, playcon); break;
                    default: music_file_con(musicfile, musicbuf, &playmun, playcon); break;
                }
                ros::spinOnce();
            }
        }
        ros::spinOnce();
    }
    return 0;
}

