#ifndef PROCESSPROVIDER_H
#define PROCESSPROVIDER_H

#include <QObject>

class ProcessProvider : public QObject
{
    Q_OBJECT

public:
    explicit ProcessProvider(QObject *parent = nullptr);

    Q_INVOKABLE bool start(const QString &exec);
};

#endif // PROCESSPROVIDER_H
