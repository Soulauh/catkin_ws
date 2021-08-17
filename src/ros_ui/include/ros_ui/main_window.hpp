#ifndef UI_VIEW_MAIN_WINDOW_H
#define UI_VIEW_MAIN_WINDOW_H

#include <QtGui/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"
#include <QImage>
#include <QMutex>
#include <QPushButton>

namespace ui_view {

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();
	void displayLocalVideo(const QImage& image);
	void displaySocketVideo(const QImage& image);

public Q_SLOTS:
	void displayUserStr();
	void displayRobotStr();
    void updateLocalVideo();
	void updateSocketVideo();
	void updateVideoArg();
	void contrloMusic_btn();
	void contrloVideo_btn();
    	
private:
	Ui::MainWindowDesign ui;
	QNode qnode;
	QImage qimage_;
	mutable QMutex qimage_mutex_;
};

} 

#endif 
