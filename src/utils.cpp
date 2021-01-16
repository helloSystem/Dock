#include "utils.h"
#include "systemappmonitor.h"
#include "systemappitem.h"

#include <QFile>
#include <QFileInfo>
#include <QUrlQuery>
#include <QSettings>
#include <QDebug>
#include <QProcess>

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

// probono: Get launchableExecutable, applicationName, and icon for a given process ID
// from .app bundles and .AppDir directories
QMap<QString, QString> Utils::examinePotentialBundlePath(QString path)
{

    qDebug() << "probono: path to be examined:" << path;

    QMap<QString, QString> info; // Data structure to be returned
    QString launchableExecutable = "";
    QString applicationName = "";
    QString icon = "";

    QFileInfo fileInfo = QFileInfo(path);
    QString nameWithoutSuffix = QFileInfo(fileInfo.completeBaseName()).fileName();

    // (Simplified) GNUstep .app bundle
    if (path.toLower().endsWith(".app")) {
        QFile executableFile(path.toUtf8() + "/" + nameWithoutSuffix);
        if (executableFile.exists() && QFileInfo(executableFile).isExecutable()) {
            qDebug() << "probono: We have an .app bundle";
            launchableExecutable = QString(path + "/" + nameWithoutSuffix);
            applicationName = nameWithoutSuffix;
        }

        QFile tiffFile1(path.toUtf8() + "/Resources/" + nameWithoutSuffix.toUtf8() + ".tiff");
        if (tiffFile1.exists()) {
            icon = QFileInfo(tiffFile1).canonicalFilePath();
        }
        QFile tiffFile2(path.toUtf8() + "/.dir.tiff");
        if (tiffFile2.exists()) {
            icon = QFileInfo(tiffFile2).canonicalFilePath();
        }
        QFile pngFile1(path.toUtf8() + "/Resources/" + nameWithoutSuffix.toUtf8() + ".png");
        QFile svgFile1(path.toUtf8() + "/Resources/" + nameWithoutSuffix.toUtf8() + ".svg");
        if (svgFile1.exists()) {
            qDebug() << "probono: FIXME: There is a svg but we are not using it yet";
        }
        if (pngFile1.exists()) {
            icon = QFileInfo(pngFile1).canonicalFilePath();
        }
    }

    // ROX AppDir
    QFile appRunFile(path.toUtf8() + "/AppRun");
    if ((appRunFile.exists()) && QFileInfo(appRunFile).isExecutable()) {
        launchableExecutable = QString(path + "/AppRun");
        applicationName = nameWithoutSuffix;
        qDebug() << "probono: We have an AppDir";

        QFile dirIconFile(path.toUtf8() + "/.DirIcon");
        if (dirIconFile.exists()) {
            icon = QFileInfo(dirIconFile).canonicalFilePath();
        }
    }
    if(icon != "")
        info.insert("Icon", icon);
    if(applicationName != "")
        info.insert("Name", applicationName);
    if(launchableExecutable != "")
        info.insert("Exec", launchableExecutable);
    return info;
}

QMap<QString, QString> Utils::readInfoFromPid(quint32 pid)
{
    QMap<QString, QString> info; // Data structure to be returned
    QString launchableExecutable = "";
    QString applicationName = "";
    QString icon = "";

    QStringList args; // Arguments of the proecss determined by pid

#if __FreeBSD__

    // On FreeBSD, /proc is optional and considered deprecated

    QProcess *p = new QProcess();
    p->setProgram("procstat");
    QStringList arguments;
    arguments << "--libxo=xml,pretty" << "arguments" << QString::number(pid);
    p->setArguments(arguments);
    p->start();
    p->waitForFinished();
    QList<QByteArray> lines = p->readAllStandardOutput().split('\n');
    // TODO: Should probably take only the first three args into consideration
    foreach (const QByteArray &line, lines) {
        QString arg = QString::fromUtf8(line).trimmed();
        if(arg.startsWith("<arguments>") and arg.endsWith("</arguments>")) {
            arg = arg.replace("<arguments>", "").replace("</arguments>", "");
            args.append(arg);
        }
    }

#else

    // Read cmdline from /proc; make sure to get all zero-delimited parts of it
    // because our .app bundle or .AppDir might well be the second argument (e.g., 'python /Some/App.app/App args')
    QFile file(QString("/proc/%1/cmdline").arg(pid));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "Could not open /proc, hence could not get information about the process that belongs to the window";
    QList<QByteArray> list = file.readAll().split('\0');
    list.takeLast(); // Remove extraneous last item

    // Convert to QStringList
    foreach (const QByteArray &item, list) {
        args.append(QString::fromUtf8(item));
    }

#endif

    qDebug() << "probono: args" << args;
    foreach (const QString &part, args) { // FIXME: Consider only the first two parts as possible paths

        // Determine whether we have an AppDir or .app bundle at hand and if yes, get launchableExecutable, applicationName, and icon
        // This code is similar to bundle.cpp in Filer; we might consider creating a static library for it

        QString path = QFileInfo(part).canonicalPath(); // Resolve symlinks and get absolute path

        if(QFileInfo(part).exists() == false) {
            continue;
        }

        info = examinePotentialBundlePath(path);

        if((info.value("Icon") != "") && (info.value("Name") != "") && (info.value("Exec") != "")) {
            return info;
        }

        // Also check the parent directory because it is not uncommon that the main executable is in a subdirectory
        // TODO: "Search up" multiple levels until an .app bundle or .AppDir is found; how?

        QString parent_path = QFileInfo(part + "/../").canonicalPath(); // Resolve symlinks and get absolute path
        qDebug() << "probono: Trying parent_path:" << parent_path;
        info = examinePotentialBundlePath(parent_path);

        if((info.value("Icon") != "") && (info.value("Name") != "") && (info.value("Exec") != "")) {
            return info;
        }
    }
    return info;
}

QString Utils::cmdFromPid(quint32 pid)
{
    QFile file(QString("/proc/%1/cmdline").arg(pid));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QList<QByteArray> list = file.readAll().split('\0');
    list.takeLast(); // Remove extraneous last item
    // Convert to QStringList
    QStringList parts;
    foreach (const QByteArray &item, list) {
        parts.append(QString::fromUtf8(item));
    }

    // QString cmd = QString::fromUtf8(file.readAll()); // probono: This gives only the first part so we need to do something else

    return parts[0];
    QString bin;
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
            if (!founded && item->exec.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            if (!founded && desktopFileInfo.baseName().startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            // Match cmdlind
            if (!founded) {
                QString cmd = cmdFromPid(pid);
                if (!cmd.isEmpty()) {
                    if (item->exec.startsWith(cmdFromPid(pid), Qt::CaseInsensitive) ||
                        item->exec.endsWith(cmdFromPid(pid), Qt::CaseInsensitive)) {
                        founded = true;
                    }
                }
            }

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
    for (SystemAppItem *item : m_sysAppMonitor->applications()) {
        if (item->path == desktopFile) {
            info.insert("Icon", item->iconName);
            info.insert("Name", item->name);
            info.insert("Exec", item->exec);
        }
    }

    return info;
}
