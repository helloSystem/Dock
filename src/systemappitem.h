#ifndef SYSTEMAPPITEM_H
#define SYSTEMAPPITEM_H

#include <QObject>

class SystemAppItem : public QObject
{
    Q_OBJECT

public:
    explicit SystemAppItem(QObject *parent = nullptr);

    QString path;
    QString name;
    QString genericName;
    QString comment;
    QString iconName;
    QString startupWMClass;
    QString exec;
    QStringList args;
};

#endif // SYSTEMAPPITEM_H
