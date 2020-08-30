#include "processprovider.h"
#include <QProcess>

ProcessProvider::ProcessProvider(QObject *parent)
    : QObject(parent)
{

}

bool ProcessProvider::start(const QString &exec)
{
    QProcess process;
    process.setProgram(exec);
    return process.startDetached();
}
