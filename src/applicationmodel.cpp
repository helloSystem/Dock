#include "applicationmodel.h"

ApplicationModel::ApplicationModel(QObject *parent)
    : QAbstractListModel(parent),
      m_iface(XWindowInterface::instance()),
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
    roles[IconName] = "iconName";
    roles[VisibleName] = "visibleName";
    roles[ActiveRole] = "isActive";
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
    case IconName:
        return item->iconName;
    case VisibleName:
        return item->visibleName;
    case ActiveRole:
        return item->isActive;
    }

    // FIXME: Implement me!
    return QVariant();
}

void ApplicationModel::clicked(quint64 wid)
{
    m_iface->clicked(wid);
}

void ApplicationModel::clicked(const QString &id)
{
    ApplicationItem *item = findItemById(id);

    if (!item)
        return;

    if (item->wids.isEmpty()) {
        // open application
    } else if (item->wids.count() > 1) {
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

    item->wids.removeOne(wid);
    if (item->wids.isEmpty()) {
        int index = indexOf(item->id);

        if (index == -1)
            return;

        beginRemoveRows(QModelIndex(), index, index);
        m_appItems.removeAll(item);
        endRemoveRows();

        emit countChanged();
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
