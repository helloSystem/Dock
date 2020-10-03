#include "menu.h"

#include <QPainterPath>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQmlEngine>

#include <QDebug>

Menu::Menu(QWidget *parent)
    : QMenu(parent)
{
//    setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus | Qt::WindowSystemMenuHint);
//    setResizeMode(QQuickView::SizeViewToRootObject);
//    setClearBeforeRendering(true);
//    setDefaultAlphaBuffer(true);
//    setColor(Qt::transparent);
    //    setSource(QUrl(QStringLiteral("qrc:/qml/Menu.qml")));
    addAction(new QAction("hello"));
}

void Menu::popup()
{
    exec(QCursor::pos());
}
