#ifndef DOCKSETTINGS_H
#define DOCKSETTINGS_H

#include <QObject>

class DockSettings : public QObject
{
    Q_OBJECT

public:
    explicit DockSettings(QObject *parent = nullptr);

    Q_INVOKABLE bool darkMode() { return m_darkMode; }
    void setDarkMode(bool enable);

signals:
    void darkModeChanged();

private:
    bool m_darkMode;
};

#endif // DOCKSETTINGS_H
