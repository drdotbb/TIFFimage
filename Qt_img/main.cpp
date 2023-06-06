#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <fstream>
#include <string>
#include <QImage>
#include <QLabel>
using namespace std;





int main( int argc, char **argv )
{
    QApplication a( argc, argv );


    MainWindow w;
    w.show();

    return a.exec();




}
