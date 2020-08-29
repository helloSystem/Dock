#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "applicationmodel.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    MainWindow w;

    return app.exec();
}
