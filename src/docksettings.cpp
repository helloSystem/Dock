#include "docksettings.h"

DockSettings::DockSettings(QObject *parent)
    : QObject(parent)
    , m_darkMode(true)
    , m_direction(Bottom)
{

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
