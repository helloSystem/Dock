#include "utils.h"

#include <QFile>
#include <QUrlQuery>
#include <QSettings>

#include <KConfigGroup>
#include <KDesktopFile>
#include <kemailsettings.h>
#include <KMimeTypeTrader>
#include <KServiceTypeTrader>
#include <KSharedConfig>
#include <KStartupInfo>
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
{

}

QUrl Utils::windowUrlFromMetadata(const QString &appId, quint32 pid, const QString &xWindowsWMClassName)
{
    KSharedConfig::Ptr rulesConfig = KSharedConfig::openConfig(QStringLiteral("taskmanagerrulesrc"));;

    if (!rulesConfig) {
        return QUrl();
    }

    QUrl url;
    KService::List services;

    // The code below this function goes on a hunt for services based on the metadata
    // that has been passed in. Occasionally, it will find more than one matching
    // service. In some scenarios (e.g. multiple identically-named .desktop files)
    // there's a need to pick the most useful one. The function below promises to "sort"
    // a list of services by how closely their KService::menuId() relates to the key that
    // has been passed in. The current naive implementation simply looks for a menuId
    // that starts with the key, prepends it to the list and returns it. In practice,
    // that means a KService with a menuId matching the appId will win over one with a
    // menuId that encodes a subfolder hierarchy.
    // A concrete example: Valve's Steam client is sometimes installed two times, once
    // natively as a Linux application, once via Wine. Both have .desktop files named
    // (S|)steam.desktop. The Linux native version is located in the menu by means of
    // categorization ("Games") and just has a menuId() matching the .desktop file name,
    // but the Wine version is placed in a folder hierarchy by Wine and gets a menuId()
    // of wine-Programs-Steam-Steam.desktop. The weighing done by this function makes
    // sure the Linux native version gets mapped to the former, while other heuristics
    // map the Wine version reliably to the latter.
    // In lieu of this weighing we just used whatever KServiceTypeTrader returned first,
    // so what we do here can be no worse.
    auto sortServicesByMenuId = [](KService::List &services, const QString &key) {
        if (services.count() == 1) {
            return;
        }

        for (const auto service : services) {
            if (service->menuId().startsWith(key, Qt::CaseInsensitive)) {
                services.prepend(service);
                return;
            }
        }
    };

    if (!(appId.isEmpty() && xWindowsWMClassName.isEmpty())) {
        // Check to see if this wmClass matched a saved one ...
        KConfigGroup grp(rulesConfig, "Mapping");
        KConfigGroup set(rulesConfig, "Settings");

        // Evaluate MatchCommandLineFirst directives from config first.
        // Some apps have different launchers depending upon command line ...
        QStringList matchCommandLineFirst = set.readEntry("MatchCommandLineFirst", QStringList());

        // Evaluate any mapping rules that map to a specific .desktop file.
        QString mapped(grp.readEntry(appId + "::" + xWindowsWMClassName, QString()));

        if (mapped.endsWith(QLatin1String(".desktop"))) {
            url = QUrl(mapped);
            return url;
        }

        if (mapped.isEmpty()) {
            mapped = grp.readEntry(appId, QString());

            if (mapped.endsWith(QLatin1String(".desktop"))) {
                url = QUrl(mapped);
                return url;
            }
        }

        // Try matching both appId and xWindowsWMClassName against StartupWMClass.
        // We do this before evaluating the mapping rules further, because StartupWMClass
        // is essentially a mapping rule, and we expect it to be set deliberately and
        // sensibly to instruct us what to do. Also, mapping rules
        //
        // StartupWMClass=STRING
        //
        //   If true, it is KNOWN that the application will map at least one
        //   window with the given string as its WM class or WM name hint.
        //
        // Source: https://specifications.freedesktop.org/startup-notification-spec/startup-notification-0.1.txt
        if (services.isEmpty()) {
            services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), QStringLiteral("exist Exec and ('%1' =~ StartupWMClass)").arg(appId));
            sortServicesByMenuId(services, appId);
        }

        if (services.isEmpty() && !xWindowsWMClassName.isEmpty()) {
            services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), QStringLiteral("exist Exec and ('%1' =~ StartupWMClass)").arg(xWindowsWMClassName));
            sortServicesByMenuId(services, xWindowsWMClassName);
        }

        // Evaluate rewrite rules from config.
        if (services.isEmpty()) {
            KConfigGroup rewriteRulesGroup(rulesConfig, QStringLiteral("Rewrite Rules"));
            if (rewriteRulesGroup.hasGroup(appId)) {
                KConfigGroup rewriteGroup(&rewriteRulesGroup, appId);

                const QStringList &rules = rewriteGroup.groupList();
                for (const QString &rule : rules) {
                    KConfigGroup ruleGroup(&rewriteGroup, rule);

                    const QString propertyConfig = ruleGroup.readEntry(QStringLiteral("Property"), QString());

                    QString matchProperty;
                    if (propertyConfig == QLatin1String("ClassClass")) {
                        matchProperty = appId;
                    } else if (propertyConfig == QLatin1String("ClassName")) {
                        matchProperty = xWindowsWMClassName;
                    }

                    if (matchProperty.isEmpty()) {
                        continue;
                    }

                    const QString serviceSearchIdentifier = ruleGroup.readEntry(QStringLiteral("Identifier"), QString());
                    if (serviceSearchIdentifier.isEmpty()) {
                        continue;
                    }

                    QRegularExpression regExp(ruleGroup.readEntry(QStringLiteral("Match")));
                    const auto match = regExp.match(matchProperty);

                    if (match.hasMatch()) {
                        const QString actualMatch = match.captured(QStringLiteral("match"));
                        if (actualMatch.isEmpty()) {
                            continue;
                        }

                        QString rewrittenString = ruleGroup.readEntry(QStringLiteral("Target")).arg(actualMatch);
                        // If no "Target" is provided, instead assume the matched property (appId/xWindowsWMClassName).
                        if (rewrittenString.isEmpty()) {
                            rewrittenString = matchProperty;
                        }

                        services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), QStringLiteral("exist Exec and ('%1' =~ %2)").arg(rewrittenString, serviceSearchIdentifier));
                        sortServicesByMenuId(services, serviceSearchIdentifier);

                        if (!services.isEmpty()) {
                            break;
                        }
                    }
                }
            }
        }

        // The appId looks like a path.
        if (services.isEmpty() && appId.startsWith(QStringLiteral("/"))) {
            // Check if it's a path to a .desktop file.
            if (KDesktopFile::isDesktopFile(appId) && QFile::exists(appId)) {
                return QUrl::fromLocalFile(appId);
            }

            // Check if the appId passes as a .desktop file path if we add the extension.
            const QString appIdPlusExtension(appId + QStringLiteral(".desktop"));

            if (KDesktopFile::isDesktopFile(appIdPlusExtension) && QFile::exists(appIdPlusExtension)) {
                return QUrl::fromLocalFile(appIdPlusExtension);
            }
        }

        // Try matching mapped name against DesktopEntryName.
        if (!mapped.isEmpty() && services.isEmpty()) {
            services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), QStringLiteral("exist Exec and ('%1' =~ DesktopEntryName) and (not exist NoDisplay or not NoDisplay)").arg(mapped));
            sortServicesByMenuId(services, mapped);
        }

        // Try matching mapped name against 'Name'.
        if (!mapped.isEmpty() && services.isEmpty()) {
            services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), QStringLiteral("exist Exec and ('%1' =~ Name) and (not exist NoDisplay or not NoDisplay)").arg(mapped));
            sortServicesByMenuId(services, mapped);
        }

        // Try matching appId against DesktopEntryName.
        if (services.isEmpty()) {
            services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), QStringLiteral("exist Exec and ('%1' =~ DesktopEntryName) and (not exist NoDisplay or not NoDisplay)").arg(appId));
            sortServicesByMenuId(services, appId);
        }

        // Try matching appId against 'Name'.
        // This has a shaky chance of success as appId is untranslated, but 'Name' may be localized.
        if (services.isEmpty()) {
            services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), QStringLiteral("exist Exec and ('%1' =~ Name) and (not exist NoDisplay or not NoDisplay)").arg(appId));
            sortServicesByMenuId(services, appId);
        }

    }

    if (!services.isEmpty()) {
        const QString &menuId = services.at(0)->menuId();

        // applications: URLs are used to refer to applications by their KService::menuId
        // (i.e. .desktop file name) rather than the absolute path to a .desktop file.
        // if (!menuId.isEmpty()) {
        //     url.setUrl(QStringLiteral("applications:") + menuId);
        //     return url;
        // }

        QString path = services.at(0)->entryPath();

        if (path.isEmpty()) {
            path = services.at(0)->exec();
        }

        if (!path.isEmpty()) {
            QString query = url.query();
            url = QUrl::fromLocalFile(path);
            url.setQuery(query);
            return url;
        }
    }

    return url;
}

QString Utils::readIconNameFromDesktop(const QString &desktopFile)
{
    QSettings desktop(desktopFile, QSettings::IniFormat);
    desktop.setIniCodec("UTF-8");
    desktop.beginGroup("Desktop Entry");
    return desktop.value("Icon").toString();
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
