#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QUrlQuery>
#include <QFile>
#include <QDebug>
#include <QString>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
