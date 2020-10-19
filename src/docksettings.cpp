#include "docksettings.h"

#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QDBusInterface>

#include <QDebug>

static const QString Service = "org.cyber.settings";
static const QString ObjectPath = "/Theme";
static const QString Interface = "org.cyber.Theme";

static DockSettings *SELF = nullptr;

DockSettings *DockSettings::self()
{
    if (SELF == nullptr)
        SELF = new DockSettings;

    return SELF;
}

DockSettings::DockSettings(QObject *parent)
    : QObject(parent)
    , m_darkMode(false)
    , m_direction(Bottom)
    , m_settings(new QSettings(QSettings::UserScope, "cyberos", "dock"))
    , m_fileWatcher(new QFileSystemWatcher(this))
{
    QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(Service, QDBusConnection::sessionBus(),
                                                                  QDBusServiceWatcher::WatchForRegistration);
    connect(serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, [=] {
        initDBusSignals();
        initData();
    });

    initDBusSignals();
    initData();
}

void DockSettings::setDarkMode(bool enable)
{
    if (m_darkMode != enable) {
        m_darkMode = enable;
        emit darkModeChanged();
    }
}

void DockSettings::setDirection(Direction direction)
{
    if (m_direction != direction) {
        m_direction = direction;
        emit directionChanged();
    }
}

void DockSettings::onDBusDarkModeChanged(bool darkMode)
{
    m_darkMode = darkMode;

    emit darkModeChanged();
}

void DockSettings::initDBusSignals()
{
    QDBusInterface iface(Service, ObjectPath, Interface, QDBusConnection::sessionBus(), this);

    if (iface.isValid()) {
        QDBusConnection::sessionBus().connect(Service, ObjectPath, Interface, "darkModeChanged",
                                              this, SLOT(onDBusDarkModeChanged(bool)));
    }
}

void DockSettings::initData()
{
    QDBusInterface iface(Service, ObjectPath, Interface, QDBusConnection::sessionBus(), this);

    if (iface.isValid()) {
        m_darkMode = iface.property("isDarkMode").toBool();

        emit darkModeChanged();
    }
}
