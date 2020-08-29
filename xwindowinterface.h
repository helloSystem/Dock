#ifndef XWINDOWINTERFACE_H
#define XWINDOWINTERFACE_H

#include "applicationitem.h"
#include <QObject>

// KLIB
#include <KWindowInfo>
#include <KWindowEffects>

class XWindowInterface : public QObject
{
    Q_OBJECT

public:
    static XWindowInterface *instance();
    explicit XWindowInterface(QObject *parent = nullptr);

    void enableBlurBehind(QWindow &view);

    ApplicationItem requestInfo(quint64 wid);
    bool isAcceptableWindow(quint64 wid);

    void toggleMinimize(quint64 wid);

    void startInitWindows();

signals:
    void windowAdded(quint64 wid);
    void windowRemoved(quint64 wid);

private:
    void onWindowadded(quint64 wid);
};

#endif // XWINDOWINTERFACE_H
