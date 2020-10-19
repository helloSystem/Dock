#include "waylandinterface.h"

// Qt
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QtX11Extras/QX11Info>
#include <QRasterWindow>

#include <KWindowSystem>
#include <KWindowInfo>
#include <KWayland/Client/surface.h>

// X11
#include <NETWM>

WaylandInterface::WaylandInterface(QObject *parent)
    : QObject(parent)
{

}
