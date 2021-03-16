#include "statusnotifierwatcher.h"
#include <QDebug>
#include <QDBusConnectionInterface>

StatusNotifierWatcher::StatusNotifierWatcher(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<IconPixmap>("IconPixmap");
    qDBusRegisterMetaType<IconPixmap>();
    qRegisterMetaType<IconPixmapList>("IconPixmapList");
    qDBusRegisterMetaType<IconPixmapList>();
    qRegisterMetaType<ToolTip>("ToolTip");
    qDBusRegisterMetaType<ToolTip>();

    QDBusConnection dbus = QDBusConnection::sessionBus();
    switch (dbus.interface()->registerService(QStringLiteral("org.kde.StatusNotifierWatcher"), QDBusConnectionInterface::QueueService).value())
    {
        case QDBusConnectionInterface::ServiceNotRegistered:
            qWarning() << "StatusNotifier: unable to register service for org.kde.StatusNotifierWatcher";
            break;
        case QDBusConnectionInterface::ServiceQueued:
            qWarning() << "StatusNotifier: registration of service org.kde.StatusNotifierWatcher queued, we can become primary after existing one deregisters";
            break;
        case QDBusConnectionInterface::ServiceRegistered:
            break;
    }

    dbus.registerObject(QStringLiteral("/StatusNotifierWatcher"), this, QDBusConnection::ExportScriptableContents);

//    if (!dbus.registerObject(QStringLiteral("/StatusNotifierWatcher"), this, QDBusConnection::ExportScriptableContents))
//        qDebug() << QDBusConnection::sessionBus().lastError().message();

    mWatcher = new QDBusServiceWatcher(this);
    mWatcher->setConnection(dbus);
    mWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);

    connect(mWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &StatusNotifierWatcher::serviceUnregistered);
}

StatusNotifierWatcher::~StatusNotifierWatcher()
{
    QDBusConnection::sessionBus().unregisterService(QStringLiteral("org.kde.StatusNotifierWatcher"));
}

void StatusNotifierWatcher::RegisterStatusNotifierItem(const QString &serviceOrPath)
{
    QString service = serviceOrPath;
    QString path = QStringLiteral("/StatusNotifierItem");

    // workaround for sni-qt
    if (service.startsWith(QLatin1Char('/'))) {
        path = service;
        service = message().service();
    }

    QString notifierItemId = service + path;

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(service).value()
        && !mServices.contains(notifierItemId)) {
        mServices << notifierItemId;
        mWatcher->addWatchedService(service);
        emit StatusNotifierItemRegistered(notifierItemId);
    }
}

void StatusNotifierWatcher::RegisterStatusNotifierHost(const QString &service)
{
    if (!mHosts.contains(service))
    {
        mHosts.append(service);
        mWatcher->addWatchedService(service);
    }
}

void StatusNotifierWatcher::serviceUnregistered(const QString &service)
{
    // qDebug() << "Service" << service << "unregistered";

    mWatcher->removeWatchedService(service);

    if (mHosts.contains(service))
    {
        mHosts.removeAll(service);
        return;
    }

    QString match = service + QLatin1Char('/');
    QStringList::Iterator it = mServices.begin();
    while (it != mServices.end())
    {
        if (it->startsWith(match))
        {
            QString name = *it;
            it = mServices.erase(it);
            emit StatusNotifierItemUnregistered(name);
        }
        else
            ++it;
    }
}
