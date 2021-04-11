#include "applicationmodel.h"
#include "utils.h"

#include <QProcess>

#include "xwindowinterface.h"
#include "utils.h"

#include <QTimer>
#include <QDebug>
#include <QX11Info>
#include <QWindow>
#include <QScreen>

#include <KWindowEffects>
#include <KWindowSystem>
#include <KWindowInfo>

// X11
#include <NETWM>
#include <xcb/xcb.h>
#include <xcb/shape.h>


ApplicationModel::ApplicationModel(QObject *parent)
    : QAbstractListModel(parent),
      m_iface(XWindowInterface::instance()),
      m_sysAppMonitor(SystemAppMonitor::self()),
      m_iconSize(36*1/0.8) // probono: Was 88
{
    connect(m_iface, &XWindowInterface::windowAdded, this, &ApplicationModel::onWindowAdded);
    connect(m_iface, &XWindowInterface::windowRemoved, this, &ApplicationModel::onWindowRemoved);
    connect(m_iface, &XWindowInterface::activeChanged, this, &ApplicationModel::onActiveChanged);

    initPinedApplications();

    QTimer::singleShot(100, m_iface, &XWindowInterface::startInitWindows);
}

int ApplicationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_appItems.size();
}

QHash<int, QByteArray> ApplicationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[AppIdRole] = "appId";
    roles[IconNameRole] = "iconName";
    roles[VisibleNameRole] = "visibleName";
    roles[ActiveRole] = "isActive";
    roles[WindowCountRole] = "windowCount";
    roles[IsPinedRole] = "isPined";
    return roles;
}

QVariant ApplicationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ApplicationItem *item = m_appItems.at(index.row());

    switch (role) {
    case AppIdRole:
        return item->id;
    case IconNameRole:
        return item->iconName;
    case VisibleNameRole:
        return item->visibleName;
    case ActiveRole:
        return item->isActive;
    case WindowCountRole:
        return item->wids.count();
    case IsPinedRole:
        return item->isPined;
    default:
        return QVariant();
    }

    // FIXME: Implement me!
    return QVariant();
}

void ApplicationModel::clicked(const QString &id)
{
    ApplicationItem *item = findItemById(id);

    if (!item)
        return;

    // Application Item that has been pined,
    // We need to open it.
    if (item->wids.isEmpty()) {
        // open application
        openNewInstance(item->id);
    }
    // Multiple windows have been opened and need to switch between them,
    // The logic here needs to be improved.
    else if (item->wids.count() > 1) {
        item->currentActive++;

        if (item->currentActive == item->wids.count())
            item->currentActive = 0;

        m_iface->forceActiveWindow(item->wids.at(item->currentActive));
    } else if (m_iface->activeWindow() == item->wids.first()) {
        m_iface->minimizeWindow(item->wids.first());
    } else {
        m_iface->forceActiveWindow(item->wids.first());
    }
}

bool ApplicationModel::openNewInstance(const QString &appId)
{
    ApplicationItem *item = findItemById(appId);

    if (!item)
        return false;

    QProcess process;
    if (!item->exec.isEmpty()) {
        QStringList args = item->exec.split(" ");
        // process.setProgram(args.first());
        // args.removeFirst();
        // probono: Nah, we use the 'launch' command instead
        process.setProgram("launch");

        if (!args.isEmpty()) {
            process.setArguments(args);
        }

    } else {
        process.setProgram(appId);
    }

    return process.startDetached();
}

void ApplicationModel::closeAllByAppId(const QString &appId)
{
    ApplicationItem *item = findItemById(appId);

    if (!item)
        return;

    for (quint64 wid : item->wids) {
        m_iface->closeWindow(wid);
    }
}

void ApplicationModel::pin(const QString &appId)
{
    ApplicationItem *item = findItemById(appId);

    if (!item)
        return;

    beginResetModel();
    item->isPined = true;
    endResetModel();

    savePinAndUnPinList();
}

void ApplicationModel::unPin(const QString &appId)
{
    ApplicationItem *item = findItemById(appId);

    if (!item)
        return;

    beginResetModel();
    item->isPined = false;
    endResetModel();

    // Need to be removed after unpin
    if (item->wids.isEmpty()) {
        int index = indexOf(item->id);
        if (index != -1) {
            beginRemoveRows(QModelIndex(), index, index);
            m_appItems.removeAll(item);
            endRemoveRows();
            emit countChanged();
        }
    }

    savePinAndUnPinList();
}

ApplicationItem *ApplicationModel::findItemByWId(quint64 wid)
{
    for (ApplicationItem *item : m_appItems) {
        for (quint64 winId : item->wids) {
            if (winId == wid)
                return item;
        }
    }

    return nullptr;
}

ApplicationItem *ApplicationModel::findItemById(const QString &id)
{
    for (ApplicationItem *item : m_appItems) {
        if (item->id == id)
            return item;
    }

    return nullptr;
}

bool ApplicationModel::contains(const QString &id)
{
    for (ApplicationItem *item : qAsConst(m_appItems)) {
        if (item->id == id)
            return true;
    }

    return false;
}

int ApplicationModel::indexOf(const QString &id)
{
    for (ApplicationItem *item : m_appItems) {
        if (item->id == id)
            return m_appItems.indexOf(item);
    }

    return -1;
}

void ApplicationModel::initPinedApplications()
{
    QSettings settings(QSettings::UserScope, "cyberos", "dock_pinned");
    QStringList groups = settings.childGroups();

    for (int i = 0; i < groups.size(); ++i) {
        for (const QString &id : groups) {
            settings.beginGroup(id);
            int index = settings.value("Index").toInt();

            if (index == i) {
                beginInsertRows(QModelIndex(), rowCount(), rowCount());
                ApplicationItem *item = new ApplicationItem;
                item->iconName = settings.value("IconName").toString();
                item->visibleName = settings.value("visibleName").toString();
                item->exec = settings.value("Exec").toString();
                item->desktopPath = settings.value("DesktopPath").toString();
                item->id = id;
                item->isPined = true;
                m_appItems.append(item);
                endInsertRows();
                emit countChanged();
                settings.endGroup();
                break;
            } else {
                settings.endGroup();
            }
        }
    }
}

void ApplicationModel::savePinAndUnPinList()
{
    QSettings settings(QSettings::UserScope, "cyberos", "dock_pinned");
    settings.clear();

    int index = 0;

    for (ApplicationItem *item : m_appItems) {
        if (item->isPined) {
            settings.beginGroup(item->id);
            settings.setValue("IconName", item->iconName);
            settings.setValue("visibleName", item->visibleName);
            settings.setValue("Exec", item->exec);
            settings.setValue("Index", index);
            settings.setValue("DesktopPath", item->desktopPath);
            settings.endGroup();
            ++index;
        }
    }

    settings.sync();
}

void ApplicationModel::onWindowAdded(quint64 wid)
{
    qDebug() << "probono: onWindowAdded";
    QMap<QString, QVariant> info = m_iface->requestInfo(wid);
    const QString id = info.value("id").toString();

    if (contains(id)) {
        for (ApplicationItem *item : m_appItems) {
            if (item->id == id) {
                // Need to update application active status.
                beginResetModel();
                item->wids.append(wid);
                item->isActive = info.value("active").toBool();
                endResetModel();
            }
        }
    } else {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        ApplicationItem *item = new ApplicationItem;
        item->id = id;
        item->iconName = info.value("iconName").toString();
        item->visibleName = info.value("visibleName").toString();
        item->isActive = info.value("active").toBool();
        item->wids.append(wid);

        if(item->exec == nullptr) {
            qDebug() << "probono: Checking for .app bundle or .AppDir";
            KWindowInfo info(wid, NET::WMPid);
            if (info.valid()){
                qDebug() << "probono: PID:" << info.pid();
                QMap<QString, QString> processInfo = Utils::instance()->readInfoFromPid(info.pid());

                if(processInfo.value("Icon") != ""){
                    qDebug() << "probono: Icon:" << processInfo.value("Icon");
                    item->iconName = "file://" + processInfo.value("Icon");
                } else {
                    // qDebug() << "probono: Icon empty, not using it";
                }
                if(processInfo.value("Name") != ""){
                    qDebug() << "probono: Name:" << processInfo.value("Name");
                    item->visibleName = processInfo.value("Name");
                } else {
                    // qDebug() << "probono: Name empty, not using it";
                }
                if(processInfo.value("Exec") != ""){
                    qDebug() << "probono: Exec:" << processInfo.value("Exec");
                    item->exec = processInfo.value("Exec");
                } else {
                    // qDebug() << "probono: Exec empty, not using it";
                }
            }
        }

        if(item->exec == nullptr) {
            QString desktopPath = m_iface->desktopFilePath(wid);
            qDebug() << "probono: desktopPath:" << desktopPath;

            if (!desktopPath.isEmpty()) {
                qDebug() << "probono: Use information from desktop file";
                QMap<QString, QString> desktopInfo = Utils::instance()->readInfoFromDesktop(desktopPath);
                item->iconName = desktopInfo.value("Icon");
                item->visibleName = desktopInfo.value("Name");
                item->exec = desktopInfo.value("Exec");
                item->desktopPath = desktopPath;
            }
        }

        m_appItems << item;
        endInsertRows();
        emit countChanged();
    }
}

void ApplicationModel::onWindowRemoved(quint64 wid)
{
    ApplicationItem *item = findItemByWId(wid);

    if (!item)
        return;

    // Remove from wid list.
    beginResetModel();
    item->wids.removeOne(wid);
    endResetModel();

    if (item->wids.isEmpty()) {
        // If it is not fixed to the dock, need to remove it.
        if (!item->isPined) {
            int index = indexOf(item->id);

            if (index == -1)
                return;

            beginRemoveRows(QModelIndex(), index, index);
            m_appItems.removeAll(item);
            endRemoveRows();
            emit countChanged();
        }
    }
}

void ApplicationModel::onActiveChanged(quint64 wid)
{
    beginResetModel();

    for (ApplicationItem *item : m_appItems) {
        item->isActive = item->wids.contains(wid);
    }

    endResetModel();
}
