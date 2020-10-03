#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include <QtCore>

class ApplicationItem
{
public:
    QString id;
    QString iconName;
    QString visibleName;
    QString desktopPath;
    bool isActive = false;
    bool isPined = false;

    int currentActive = 0;

    QList<quint64> wids;

    bool operator==(ApplicationItem item) {
        return item.id == this->id;
    }
};

#endif // APPLICATIONITEM_H
