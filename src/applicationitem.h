#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include <QtCore>

class ApplicationItem
{
public:
    quint64 winId;
    QString windowClass;
    QString iconName;
    bool isActive;

    bool operator==(ApplicationItem item) {
        return item.winId == this->winId;
    }
};

#endif // APPLICATIONITEM_H
