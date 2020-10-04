#include "applicationmodel.h"
#include "utils.h"

#include <QProcess>

ApplicationModel::ApplicationModel(QObject *parent)
    : QAbstractListModel(parent),
      m_iface(XWindowInterface::instance()),
      m_sysAppMonitor(SystemAppMonitor::self()),
      m_iconSize(88)
{
    connect(m_iface, &XWindowInterface::windowAdded, this, &ApplicationModel::onWindowAdded);
    connect(m_iface, &XWindowInterface::windowRemoved, this, &ApplicationModel::onWindowRemoved);
    connect(m_iface, &XWindowInterface::activeChanged, this, &ApplicationModel::onActiveChanged);

    m_iface->startInitWindows();
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
        process.setProgram(item->exec);
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

void ApplicationModel::onWindowAdded(quint64 wid)
{
    QMap<QString, QVariant> info = m_iface->requestInfo(wid);
    const QString id = info.value("id").toString();

    if (contains(id)) {
        for (ApplicationItem *item : m_appItems) {
            if (item->id == id) {
                item->wids.append(wid);
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

        QString desktopPath = m_iface->desktopFilePath(wid);
        if (!desktopPath.isEmpty()) {
            QMap<QString, QString> desktopInfo = Utils::instance()->readInfoFromDesktop(desktopPath);
            item->iconName = desktopInfo.value("Icon");
            item->visibleName = desktopInfo.value("Name");
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
    item->wids.removeOne(wid);

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
