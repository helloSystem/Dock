#include "utils.h"
#include "systemappmonitor.h"
#include "systemappitem.h"

#include <QFile>
#include <QFileInfo>
#include <QUrlQuery>
#include <QSettings>

#include <KWindowSystem>

static Utils *INSTANCE = nullptr;

Utils *Utils::instance()
{
    if (!INSTANCE)
        INSTANCE = new Utils;

    return INSTANCE;
}

Utils::Utils(QObject *parent)
    : QObject(parent)
    , m_sysAppMonitor(SystemAppMonitor::self())
{

}

QString Utils::cmdFromPid(quint32 pid)
{
    QFile file(QString("/proc/%1/cmdline").arg(pid));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString cmd = QString::fromUtf8(file.readAll());
    QString bin;

    for (int i = 0; i < cmd.size(); ++i) {
        const QChar ch = cmd[i];
        if (ch == '\\')
            i++;
        else if (ch == ' ')
            break;
        else
            bin += ch;
    }

    if (bin.startsWith("/"))
        return bin.split("/").last();

    return bin;
}

QString Utils::desktopPathFromMetadata(const QString &appId, quint32 pid, const QString &xWindowWMClassName)
{
    QString result;

    if (!appId.isEmpty() && !xWindowWMClassName.isEmpty()) {
        for (SystemAppItem *item : m_sysAppMonitor->applications()) {
            QFileInfo desktopFileInfo(item->path);
            bool founded = false;

            if (desktopFileInfo.baseName() == xWindowWMClassName ||
                desktopFileInfo.completeBaseName() == xWindowWMClassName)
                founded = true;

            // StartupWMClass=STRING
            // If true, it is KNOWN that the application will map at least one
            // window with the given string as its WM class or WM name hint.
            // ref: https://specifications.freedesktop.org/startup-notification-spec/startup-notification-0.1.txt
            if (item->startupWMClass.startsWith(appId, Qt::CaseInsensitive) ||
                item->startupWMClass.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            if (!founded && item->iconName.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            // Try matching mapped name against 'Name'.
            if (!founded && item->name.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            // exec
            if (!founded && item->args.first().startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            if (!founded && item->args.first().startsWith(cmdFromPid(pid), Qt::CaseInsensitive))
                founded = true;

            if (!founded && desktopFileInfo.baseName().startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            if (founded) {
                result = item->path;
                break;
            }
        }
    }

    return result;
}

QMap<QString, QString> Utils::readInfoFromDesktop(const QString &desktopFile)
{
    QMap<QString, QString> info;
    QSettings desktop(desktopFile, QSettings::IniFormat);
    desktop.setIniCodec("UTF-8");
    desktop.beginGroup("Desktop Entry");

    info.insert("Icon", desktop.value("Icon").toString());
    info.insert("Name", desktop.value("Name").toString());

    return info;
}
