#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QIcon>
#include <QUrl>

class SystemAppMonitor;
class Utils : public QObject
{
    Q_OBJECT

public:
    struct AppData
    {
        QString id; // Application id (*.desktop sans extension).
        QString name; // Application name.
        QString genericName; // Generic application name.
        QIcon icon;
        QUrl url;
        bool skipTaskbar = false;
    };

    enum UrlComparisonMode {
         Strict = 0,
         IgnoreQueryItems
    };

    static Utils *instance();

    explicit Utils(QObject *parent = nullptr);

    QString cmdFromPid(quint32 pid);
    QString desktopPathFromMetadata(const QString &appId, quint32 pid = 0,
                                    const QString &xWindowWMClassName = QString());
    QMap<QString, QString> readInfoFromDesktop(const QString &desktopFile);
    QMap<QString, QString> readInfoFromPid(quint32 pid);

private:
    SystemAppMonitor *m_sysAppMonitor;
    QMap<QString, QString>  examinePotentialBundlePath(QString path);
};

#endif // UTILS_H
