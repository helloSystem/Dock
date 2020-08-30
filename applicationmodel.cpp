#include "applicationmodel.h"

ApplicationModel::ApplicationModel(QObject *parent)
    : QAbstractListModel(parent),
      m_iface(XWindowInterface::instance()),
      m_iconSize(72)
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
    roles[WindowClass] = "windowClass";
    roles[IconName] = "iconName";
    roles[ActiveRole] = "isActive";
    return roles;
}

QVariant ApplicationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ApplicationItem item = m_appItems.at(index.row());

    switch (role) {
    case AppIdRole:
        return item.winId;
    case WindowClass:
        return item.windowClass;
    case IconName:
        return item.iconName;
    case ActiveRole:
        return item.isActive;
    }

    // FIXME: Implement me!
    return QVariant();
}

void ApplicationModel::toggleMinimize(quint64 wid)
{
    m_iface->toggleMinimize(wid);
}

bool ApplicationModel::contains(quint64 wid)
{
    for (const ApplicationItem &item : qAsConst(m_appItems)) {
        if (item.winId == wid)
            return true;
    }

    return false;
}

int ApplicationModel::indexOf(quint64 wid)
{
    for (const ApplicationItem &item : m_appItems) {
        if (item.winId == wid)
            return m_appItems.indexOf(item);
    }

    return -1;
}

void ApplicationModel::onWindowAdded(quint64 wid)
{
    if (contains(wid))
        return;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    ApplicationItem item = XWindowInterface::instance()->requestInfo(wid);
    m_appItems << item;
    endInsertRows();

    emit countChanged();
}

void ApplicationModel::onWindowRemoved(quint64 wid)
{
    int index = indexOf(wid);

    if (index == -1)
        return;

    ApplicationItem item = m_appItems.at(index);

    beginRemoveRows(QModelIndex(), index, index);
    m_appItems.removeAll(item);
    endRemoveRows();

    emit countChanged();
}

void ApplicationModel::onActiveChanged(quint64 wid)
{
    beginResetModel();

    for (ApplicationItem &item : m_appItems) {
        if (item.winId == wid) {
            item.isActive = true;
        } else {
            item.isActive = false;
        }
    }

    endResetModel();
}
