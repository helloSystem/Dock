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

    void enableBlurBehind(QWindow *view, bool enable = true, const QRegion &region = QRegion());

    WId activeWindow();
    void minimizeWindow(WId win);
    void closeWindow(WId id);
    void forceActiveWindow(WId win);

    QMap<QString, QVariant> requestInfo(quint64 wid);
    QString requestWindowClass(quint64 wid);
    bool isAcceptableWindow(quint64 wid);

    void setViewStruts(QWindow *view, const QRect &rect);

    void startInitWindows();

    QString desktopFilePath(quint64 wid);

    void setIconGeometry(quint64 wid, const QRect &rect);

signals:
    void windowAdded(quint64 wid);
    void windowRemoved(quint64 wid);
    void activeChanged(quint64 wid);

private:
    void onWindowadded(quint64 wid);
};

#endif // XWINDOWINTERFACE_H
