#ifndef WAYLANDINTERFACE_H
#define WAYLANDINTERFACE_H

#include <QObject>

#include <KWayland/Client/registry.h>
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/surface.h>
#include <KWindowInfo>
#include <KWindowEffects>

class WaylandInterface : public QObject
{
    Q_OBJECT

public:
    explicit WaylandInterface(QObject *parent = nullptr);

signals:

};

#endif // WAYLANDINTERFACE_H
