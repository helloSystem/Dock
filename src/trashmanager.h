#ifndef TRASHMANAGER_H
#define TRASHMANAGER_H

#include <QObject>
#include <QFileSystemWatcher>

class TrashManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit TrashManager(QObject *parent = nullptr);

    Q_INVOKABLE void openTrash();
    Q_INVOKABLE void emptyTrash();

    int count() { return m_count; }

Q_SIGNALS:
    void countChanged();

private slots:
    void onDirectoryChanged();

private:
    QFileSystemWatcher *m_filesWatcher;
    int m_count;
};

#endif // TRASHMANAGER_H
