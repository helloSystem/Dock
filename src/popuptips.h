#ifndef POPUPTIPSMANAGER_H
#define POPUPTIPSMANAGER_H

#include <QQuickView>

class PopupTips : public QQuickView
{
    Q_OBJECT

public:
    explicit PopupTips(QQuickView *parent = nullptr);

    Q_INVOKABLE void popup(bool containsMouse, qreal mouseX, qreal mouseY);
    Q_INVOKABLE void popup(bool containsMouse, const QPointF point, const QString &text);
};

#endif // POPUPTIPSMANAGER_H
