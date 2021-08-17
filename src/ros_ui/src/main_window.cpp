#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include "../include/ros_ui/main_window.hpp"

namespace ui_view {

using namespace Qt;

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
	: QMainWindow(parent)
	, qnode(argc,argv)
{
	ui.setupUi(this); 
    setWindowIcon(QIcon(":/images/test.jpg"));
	ui.tab_manager->setCurrentIndex(0); 

    if (qnode.init()) {
        qDebug() << "init qnode sucessful !!!";
    } else {
        qDebug() << "init qnode failed !!!";
        exit(1);
    }
        
    QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));
    QObject::connect(&qnode, SIGNAL(updataLocal()), this, SLOT(updateLocalVideo()));
    QObject::connect(&qnode, SIGNAL(updataSocket()), this, SLOT(updateSocketVideo()));
    QObject::connect(&qnode, SIGNAL(updataUserStr()), this, SLOT(displayUserStr()));
    QObject::connect(&qnode, SIGNAL(updataRoboyStr()), this, SLOT(displayRobotStr()));
    QObject::connect(&qnode, SIGNAL(updataVideoArg()), this, SLOT(updateVideoArg()));
    QObject::connect(ui.btn_ps, SIGNAL(clicked()), this, SLOT(contrloMusic_btn()));
    QObject::connect(ui.btn_up, SIGNAL(clicked()), this, SLOT(contrloMusic_btn()));
    QObject::connect(ui.btn_down, SIGNAL(clicked()), this, SLOT(contrloMusic_btn()));
    QObject::connect(ui.btn_vc_video, SIGNAL(clicked()), this, SLOT(contrloVideo_btn()));
    QObject::connect(ui.btn_up_video, SIGNAL(clicked()), this, SLOT(contrloVideo_btn()));
    QObject::connect(ui.btn_down_video, SIGNAL(clicked()), this, SLOT(contrloVideo_btn()));

}

MainWindow::~MainWindow() {}

void MainWindow::updateLocalVideo() {
    displayLocalVideo(qnode.get_imageLocalVideo());
}

void MainWindow::updateSocketVideo() {
    displaySocketVideo(qnode.get_imageSocketVideo());
}

void MainWindow::updateVideoArg() {
    ui.label_frames->setText(QString::number(qnode.arg.frames));
    ui.label_fps->setText(QString::number(qnode.arg.fps));
    ui.label_size->setText(QString::number(qnode.arg.size[0]) + " x " + QString::number(qnode.arg.size[1]));
}

void MainWindow::displayUserStr() {
    ui.label_user_txt->setText(qnode.get_user_dataString());   
}

void MainWindow::displayRobotStr() {
    ui.label_robot_txt->setText(qnode.get_robot_dataString());   
}

void MainWindow::contrloMusic_btn() {
    QPushButton * btn = qobject_cast<QPushButton*> (sender());
    char k = btn->text().toStdString()[0];
    switch (k) {
        case 'p': 
            qnode.music_control(k);
            btn->setText("stop");
            break;
        case 's': 
            qnode.music_control(k);
            btn->setText("play");
            break;
        case 'u': 
            qnode.music_control(k);
            break;
        case 'd': 
            qnode.music_control(k);
            break;    
    }
}

void MainWindow::contrloVideo_btn() {
    QPushButton * btn = qobject_cast<QPushButton*> (sender());
    char k = btn->text().toStdString()[0];
    switch (k) {
        case 'v': 
            qnode.video_control(k);
            btn->setText("camera");
            break;
        case 'c': 
            qnode.video_control(k);
            btn->setText("video");
            break;
        case 'u': 
            qnode.video_control(k);
            break;
        case 'd': 
            qnode.video_control(k);
            break;    
    }
}

void MainWindow::displayLocalVideo(const QImage &image) {
    qimage_mutex_.lock();
    qimage_ = image.copy();
    ui.label_localVideo->setPixmap(QPixmap::fromImage(qimage_));
    ui.label_localVideo->resize(ui.label_localVideo->pixmap()->size());
    qimage_mutex_.unlock();      
}

void MainWindow::displaySocketVideo(const QImage &image) {
    qimage_mutex_.lock();
    qimage_ = image.copy();
    ui.label_socketVideo->setPixmap(QPixmap::fromImage(qimage_));
    ui.label_socketVideo->resize(ui.label_socketVideo->pixmap()->size());
    qimage_mutex_.unlock();      
}



}  

