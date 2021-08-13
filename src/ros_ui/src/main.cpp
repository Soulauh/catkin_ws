#include <QtGui>
#include <QApplication>
#include "../include/ros_ui/main_window.hpp"

int main(int argc, char **argv) {
    
    QApplication app(argc, argv);
    ui_view::MainWindow w(argc,argv);
    w.show();
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    int result = app.exec();

	return result;
}
