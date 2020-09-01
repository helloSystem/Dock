#include "popuptips.h"
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>

#include <QDebug>

PopupTips::PopupTips(QQuickView *parent)
    : QQuickView(parent)
{
    setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus | Qt::ToolTip);
    setSource(QUrl(QStringLiteral("qrc:/qml/PopupTips.qml")));
    setResizeMode(QQuickView::SizeViewToRootObject);
    setClearBeforeRendering(true);
}

void PopupTips::popup(bool containsMouse, qreal mouseX, qreal mouseY)
{
    qDebug() << "popup !!!" << containsMouse << mouseX << mouseY;
}

void PopupTips::popup(bool containsMouse, const QPointF point, const QString &text)
{
    if (!containsMouse || text.isEmpty()) {
        setVisible(false);
        return;
    }

    const int padding = 10;

    QMetaObject::invokeMethod(rootObject(), "setText", Q_ARG(QVariant, text));
    QMetaObject::invokeMethod(rootObject(), "setVisible", Q_ARG(QVariant, true));

    setVisible(true);
    setX(point.x());
    setY(point.y() - height() - padding);
}
