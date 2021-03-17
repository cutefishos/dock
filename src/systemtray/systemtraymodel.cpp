#include "systemtraymodel.h"

#include <QApplication>
#include <QDebug>

SystemTrayModel::SystemTrayModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_hostName = "org.kde.StatusNotifierHost-" + QString::number(QCoreApplication::applicationPid());
    QDBusConnection::sessionBus().interface()->registerService(m_hostName, QDBusConnectionInterface::DontQueueService);

    m_watcher = new StatusNotifierWatcher;
    m_watcher->RegisterStatusNotifierHost(m_hostName);
    m_watcher->moveToThread(QApplication::instance()->thread());

    connect(m_watcher, &StatusNotifierWatcher::StatusNotifierItemRegistered, this, &SystemTrayModel::onItemAdded);
    connect(m_watcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered, this, &SystemTrayModel::onItemRemoved);
}

SystemTrayModel::~SystemTrayModel()
{
    QDBusConnection::sessionBus().unregisterService(m_hostName);

    delete m_watcher;
}

int SystemTrayModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_items.size();
}

QHash<int, QByteArray> SystemTrayModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[IconNameRole] = "iconName";
    roles[IconRole] = "icon";
    roles[TitleRole] = "title";
    roles[ToolTipRole] = "toolTip";
    return roles;
}

QVariant SystemTrayModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    StatusNotifierItemSource *item = m_items.at(index.row());

    switch (role) {
    case IdRole:
        return item->id();
    case IconNameRole:
        return item->iconName();
    case IconRole: {
        if (!item->icon().isNull())
            return item->icon();
        else
            return QVariant();
    }
    case TitleRole:
        return item->title();
    case ToolTipRole:
        return item->tooltip();
    }

    return QVariant();
}

int SystemTrayModel::indexOf(const QString &id)
{
    for (StatusNotifierItemSource *item : m_items) {
        if (item->id() == id)
            return m_items.indexOf(item);
    }

    return -1;
}

StatusNotifierItemSource *SystemTrayModel::findItemById(const QString &id)
{
    int index = indexOf(id);

    if (index == -1)
        return nullptr;

    return m_items.at(index);
}

void SystemTrayModel::leftButtonClick(const QString &id)
{
    StatusNotifierItemSource *item = findItemById(id);

    if (item) {
        QPoint p(QCursor::pos());
        item->activate(p.x(), p.y());
    }
}

void SystemTrayModel::rightButtonClick(const QString &id)
{
    StatusNotifierItemSource *item = findItemById(id);
    if (item) {
        QPoint p(QCursor::pos());
        item->contextMenu(p.x(), p.y());
    }
}

void SystemTrayModel::onItemAdded(const QString &service)
{
    StatusNotifierItemSource *source = new StatusNotifierItemSource(service, this);

    connect(source, &StatusNotifierItemSource::updated, this, &SystemTrayModel::updated);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items.append(source);
    endInsertRows();
}

void SystemTrayModel::onItemRemoved(const QString &service)
{
    int index = indexOf(service);

    if (index != -1) {
        beginRemoveRows(QModelIndex(), index, index);
        StatusNotifierItemSource *item = m_items.at(index);
        m_items.removeAll(item);
        endRemoveRows();
    }
}

void SystemTrayModel::updated(StatusNotifierItemSource *item)
{
    if (!item)
        return;

    int idx = indexOf(item->id());

    // update
    if (idx != -1) {
        dataChanged(index(idx, 0), index(idx, 0));
    }
}
