#ifndef STATUSNOTIFIERWATCHER_H
#define STATUSNOTIFIERWATCHER_H

#include <QObject>
#include <QDBusContext>
#include <QStringList>
#include <QSet>

class QDBusServiceWatcher;
class StatusNotifierWatcher : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_SCRIPTABLE Q_PROPERTY(bool IsStatusNotifierHostRegistered READ IsStatusNotifierHostRegistered)
    Q_SCRIPTABLE Q_PROPERTY(int ProtocolVersion READ protocolVersion)
    Q_SCRIPTABLE Q_PROPERTY(QStringList RegisteredStatusNotifierItems READ RegisteredStatusNotifierItems)

public:
    explicit StatusNotifierWatcher(QObject *parent = nullptr);
    ~StatusNotifierWatcher();

    QStringList RegisteredStatusNotifierItems() const;
    bool IsStatusNotifierHostRegistered() const;
    int protocolVersion() const { return 0; }

public slots:
    void RegisterStatusNotifierItem(const QString &service);
    void RegisterStatusNotifierHost(const QString &service);

protected Q_SLOTS:
    void serviceUnregistered(const QString &name);

Q_SIGNALS:
    void StatusNotifierItemRegistered(const QString &service);
    // TODO: decide if this makes sense, the systray itself could notice the vanishing of items, but looks complete putting it here
    void StatusNotifierItemUnregistered(const QString &service);
    void StatusNotifierHostRegistered();
    void StatusNotifierHostUnregistered();

private:
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    QStringList m_registeredServices;
    QSet<QString> m_statusNotifierHostServices;
};

#endif // STATUSNOTIFIERWATCHER_H
