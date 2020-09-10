#ifndef POPUPTIPSMANAGER_H
#define POPUPTIPSMANAGER_H

#include <QQuickView>

class PopupTips : public QQuickView
{
    Q_OBJECT

public:
    explicit PopupTips(QQuickView *parent = nullptr);

    Q_INVOKABLE void popup(const QPointF point, const QString &text);
    Q_INVOKABLE void hide();
    Q_INVOKABLE void updateBlurRegion();
};

#endif // POPUPTIPSMANAGER_H
