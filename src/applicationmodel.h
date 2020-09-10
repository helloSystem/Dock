#ifndef APPLICATIONMODEL_H
#define APPLICATIONMODEL_H

#include <QAbstractListModel>
#include "applicationitem.h"
#include "xwindowinterface.h"

class ApplicationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        AppIdRole = Qt::UserRole + 1,
        IconName,
        IconSize,
        VisibleName,
        ActiveRole
    };

    explicit ApplicationModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void clicked(quint64 wid);
    Q_INVOKABLE void clicked(const QString &id);

    int iconSize() { return m_iconSize; }

signals:
    void countChanged();

private:
    ApplicationItem *findItemByWId(quint64 wid);
    ApplicationItem *findItemById(const QString &id);
    bool contains(const QString &id);
    int indexOf(const QString &id);
    void onWindowAdded(quint64 wid);
    void onWindowRemoved(quint64 wid);
    void onActiveChanged(quint64 wid);

private:
    XWindowInterface *m_iface;
    QList<ApplicationItem *> m_appItems;
    int m_iconSize;
};

#endif // APPLICATIONMODEL_H
