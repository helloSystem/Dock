#include "docksettings.h"

DockSettings::DockSettings(QObject *parent)
    : QObject(parent),
      m_darkMode(true)
{

}

void DockSettings::setDarkMode(bool enable)
{
    if (m_darkMode != enable) {
        m_darkMode = enable;
        emit darkModeChanged();
    }
}
