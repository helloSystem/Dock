#include "trashmanager.h"
#include <QProcess>
#include <QDir>

const QString TrashDir = QDir::homePath() + "/.local/share/Trash";
const QDir::Filters ItemsShouldCount = QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot;

TrashManager::TrashManager(QObject *parent)
    : QObject(parent),
      m_filesWatcher(new QFileSystemWatcher(this)),
      m_count(0)
{
    onDirectoryChanged();
    connect(m_filesWatcher, &QFileSystemWatcher::directoryChanged, this, &TrashManager::onDirectoryChanged, Qt::QueuedConnection);
}

void TrashManager::emptyTrash()
{

}

void TrashManager::openTrash()
{
    QProcess::startDetached("gio", QStringList() << "open" << "trash:///");
}

void TrashManager::onDirectoryChanged()
{
    m_filesWatcher->addPath(TrashDir);

    if (QDir(TrashDir + "/files").exists()) {
        m_filesWatcher->addPath(TrashDir + "/files");
        m_count = QDir(TrashDir + "/files").entryList(ItemsShouldCount).count();
    } else {
        m_count = 0;
    }

    emit countChanged();
}
