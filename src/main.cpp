#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "applicationmodel.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    MainWindow w;

    return app.exec();
}
