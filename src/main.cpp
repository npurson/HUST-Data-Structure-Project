#include "mainwindow.h"
#include <QApplication>


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(1440, 800);
    w.setWindowTitle("方舟编译器");
    w.show();
    return a.exec();
}
