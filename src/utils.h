#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QIcon>
#include <QModelIndex>
#include <QUrl>

#include <KService>
#include <KSharedConfig>

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

    QUrl windowUrlFromMetadata(const QString &appId, quint32 pid = 0,
                               const QString &xWindowsWMClassName = QString());

    QString readIconNameFromDesktop(const QString &desktopFile);
    QMap<QString, QString> readInfoFromDesktop(const QString &desktopFile);

signals:

};

#endif // UTILS_H
