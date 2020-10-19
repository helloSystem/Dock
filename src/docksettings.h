#ifndef DOCKSETTINGS_H
#define DOCKSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QFileSystemWatcher>

class DockSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool darkMode READ darkMode NOTIFY darkModeChanged)

public:
    enum Direction {
        Left = 0,
        Right,
        Bottom
    };

    static DockSettings *self();
    explicit DockSettings(QObject *parent = nullptr);

    bool darkMode() { return m_darkMode; }
    void setDarkMode(bool enable);

    void setDirection(Direction direction);

private slots:
    void onDBusDarkModeChanged(bool darkMode);

private:
    void initDBusSignals();
    void initData();

Q_SIGNALS:
    void darkModeChanged();
    void directionChanged();

private:
    bool m_darkMode;
    Direction m_direction;
    QSettings *m_settings;
    QFileSystemWatcher *m_fileWatcher;
};

#endif // DOCKSETTINGS_H
