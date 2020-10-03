#ifndef DOCKSETTINGS_H
#define DOCKSETTINGS_H

#include <QObject>

class DockSettings : public QObject
{
    Q_OBJECT

public:
    enum Direction {
        Left = 0,
        Right,
        Bottom
    };

    explicit DockSettings(QObject *parent = nullptr);

    Q_INVOKABLE bool darkMode() { return m_darkMode; }
    void setDarkMode(bool enable);

    void setDirection(Direction direction);

Q_SIGNALS:
    void darkModeChanged();
    void directionChanged();

private:
    bool m_darkMode;
    Direction m_direction;
};

#endif // DOCKSETTINGS_H
