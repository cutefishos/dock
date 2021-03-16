#include "statusnotifiermodel.h"
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDBusConnectionInterface>
#include <QApplication>

#include <QPixmap>
#include <QIcon>
#include <QImage>
#include <QCursor>

StatusNotifierModel::StatusNotifierModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_watcher(nullptr)
{
    QFutureWatcher<StatusNotifierWatcher *> * futureWatcher = new QFutureWatcher<StatusNotifierWatcher *>;
    connect(futureWatcher, &QFutureWatcher<StatusNotifierWatcher *>::finished, this, [this, futureWatcher] {
        m_watcher = futureWatcher->future().result();

        connect(m_watcher, &StatusNotifierWatcher::StatusNotifierItemRegistered,
                this, &StatusNotifierModel::itemAdded);
        connect(m_watcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered,
                this, &StatusNotifierModel::itemRemoved);

        qDebug() << m_watcher->RegisteredStatusNotifierItems();

        futureWatcher->deleteLater();
    });

    QFuture<StatusNotifierWatcher *> future = QtConcurrent::run([=] {
        QString dbusName = QStringLiteral("org.kde.StatusNotifierHost-%1-%2").arg(QApplication::applicationPid()).arg(1);
        if (QDBusConnectionInterface::ServiceNotRegistered == QDBusConnection::sessionBus().interface()->registerService(dbusName, QDBusConnectionInterface::DontQueueService))
            qDebug() << "unable to register service for " << dbusName;

        StatusNotifierWatcher * watcher = new StatusNotifierWatcher;
        watcher->RegisterStatusNotifierHost(dbusName);
        watcher->moveToThread(QApplication::instance()->thread());
        return watcher;
    });

    futureWatcher->setFuture(future);
}

StatusNotifierModel::~StatusNotifierModel()
{
    delete m_watcher;
}

int StatusNotifierModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_items.size();
}

QHash<int, QByteArray> StatusNotifierModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[IconNameRole] = "iconName";
    roles[IconBytesRole] = "iconBytes";
    roles[TitleRole] = "title";
    roles[ToolTipRole] = "toolTip";
    return roles;
}

QVariant StatusNotifierModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    StatusNotifierItemSource *item = m_items.at(index.row());

    switch (role) {
    case IdRole:
        return item->id();
    case IconNameRole:
        return item->iconName();
    case IconBytesRole:
        return item->iconBytes();
    case TitleRole:
        return item->title();
    case ToolTipRole:
        return item->tooltip();
    }

    return QVariant();
}

int StatusNotifierModel::indexOf(const QString &id)
{
    for (StatusNotifierItemSource *item : m_items) {
        if (item->id() == id)
            return m_items.indexOf(item);
    }

    return -1;
}

StatusNotifierItemSource *StatusNotifierModel::findItemById(const QString &id)
{
    int index = indexOf(id);

    if (index == -1)
        return nullptr;

    return m_items.at(index);
}

void StatusNotifierModel::leftButtonClick(const QString &id)
{
    StatusNotifierItemSource *item = findItemById(id);
    if (item) {
        QPoint p(QCursor::pos());
        item->activate(p.x(), p.y());
    }
}

void StatusNotifierModel::rightButtonClick(const QString &id)
{
    StatusNotifierItemSource *item = findItemById(id);
    if (item) {
        QPoint p(QCursor::pos());
        item->contextMenu(p.x(), p.y());
    }
}

void StatusNotifierModel::itemAdded(QString serviceAndPath)
{
    StatusNotifierItemSource *source = new StatusNotifierItemSource(serviceAndPath, this);

    connect(source, &StatusNotifierItemSource::updated, this, &StatusNotifierModel::updated);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items.append(source);
    endInsertRows();
}

void StatusNotifierModel::itemRemoved(const QString &serviceAndPath)
{
    int index = indexOf(serviceAndPath);

    if (index != -1) {
        beginRemoveRows(QModelIndex(), index, index);
        StatusNotifierItemSource *item = m_items.at(index);
        m_items.removeAll(item);
        endRemoveRows();
    }
}

void StatusNotifierModel::updated(StatusNotifierItemSource *item)
{
    if (!item)
        return;

    int idx = indexOf(item->id());

    // update
    if (idx != -1) {
        dataChanged(index(idx, 0), index(idx, 0));
    }
}
