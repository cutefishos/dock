#include "sniasync.h"

SniAsync::SniAsync(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent/* = 0*/)
    : QObject(parent)
    , m_sni{service, path, connection}
{
    //forward StatusNotifierItem signals
    connect(&m_sni, &org::kde::StatusNotifierItem::NewAttentionIcon, this, &SniAsync::NewAttentionIcon);
    connect(&m_sni, &org::kde::StatusNotifierItem::NewIcon, this, &SniAsync::NewIcon);
    connect(&m_sni, &org::kde::StatusNotifierItem::NewOverlayIcon, this, &SniAsync::NewOverlayIcon);
    connect(&m_sni, &org::kde::StatusNotifierItem::NewStatus, this, &SniAsync::NewStatus);
    connect(&m_sni, &org::kde::StatusNotifierItem::NewTitle, this, &SniAsync::NewTitle);
    connect(&m_sni, &org::kde::StatusNotifierItem::NewToolTip, this, &SniAsync::NewToolTip);
}

QDBusPendingReply<QDBusVariant> SniAsync::asyncPropGet(QString const & property)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(m_sni.service(), m_sni.path(), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get"));
    msg << m_sni.interface() << property;
    return m_sni.connection().asyncCall(msg);
}
