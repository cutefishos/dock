    #include "statusnotifieritemsource.h"

#include <QApplication>
#include <QIcon>
#include <QDebug>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QVariantMap>
#include <QImage>
#include <QMenu>
#include <QPixmap>

#include <dbusmenu-qt5/dbusmenuimporter.h>

class MenuImporter : public DBusMenuImporter
{
public:
    using DBusMenuImporter::DBusMenuImporter;

protected:
    virtual QIcon iconForName(const QString & name) override {
        return QIcon::fromTheme(name);
    }
};

StatusNotifierItemSource::StatusNotifierItemSource(const QString &id, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_refreshing(false)
    , m_needsReRefreshing(false)
    , m_menuImporter(nullptr)
{
    int slash = id.indexOf('/');
    if (slash == -1) {
        qWarning() << "Invalid notifierItemId:" << id;
        m_valid = false;
        m_statusNotifierItemInterface = nullptr;
        return;
    }

    QString service = id.left(slash);
    QString path = id.mid(slash);

    m_statusNotifierItemInterface = new org::kde::StatusNotifierItem(service, path,
                                                                     QDBusConnection::sessionBus(), this);
    m_refreshTimer.setSingleShot(true);
    m_refreshTimer.setInterval(10);
    connect(&m_refreshTimer, &QTimer::timeout, this, &StatusNotifierItemSource::performRefresh);

    m_valid = !service.isEmpty() && m_statusNotifierItemInterface->isValid();
    if (m_valid) {
        connect(m_statusNotifierItemInterface, &StatusNotifierItemInterface::NewTitle, this, &StatusNotifierItemSource::refreshTitle);
        connect(m_statusNotifierItemInterface, &StatusNotifierItemInterface::NewIcon, this, &StatusNotifierItemSource::refreshIcons);
        connect(m_statusNotifierItemInterface, &StatusNotifierItemInterface::NewAttentionIcon, this, &StatusNotifierItemSource::refreshIcons);
        connect(m_statusNotifierItemInterface, &StatusNotifierItemInterface::NewOverlayIcon, this, &StatusNotifierItemSource::refreshIcons);
        connect(m_statusNotifierItemInterface, &StatusNotifierItemInterface::NewToolTip, this, &StatusNotifierItemSource::refreshToolTip);
        refresh();
    }
}

StatusNotifierItemSource::~StatusNotifierItemSource()
{
    delete m_statusNotifierItemInterface;
}

void StatusNotifierItemSource::activate(int x, int y)
{
    if (m_statusNotifierItemInterface && m_statusNotifierItemInterface->isValid()) {
        m_statusNotifierItemInterface->Activate(x, y);
    }
}

void StatusNotifierItemSource::secondaryActivate(int x, int y)
{
    if (m_statusNotifierItemInterface && m_statusNotifierItemInterface->isValid()) {
        m_statusNotifierItemInterface->call(QDBus::NoBlock, QStringLiteral("SecondaryActivate"), x, y);
    }
}

void StatusNotifierItemSource::contextMenu(int x, int y)
{
    if (m_menuImporter) {
        m_menuImporter->updateMenu();

        // Popup menu
        if (m_menuImporter->menu()) {
            m_menuImporter->menu()->popup(QPoint(x, y));
        }
    } else {
        qWarning() << "Could not find DBusMenu interface, falling back to calling ContextMenu()";
        if (m_statusNotifierItemInterface && m_statusNotifierItemInterface->isValid()) {
            m_statusNotifierItemInterface->call(QDBus::NoBlock, QStringLiteral("ContextMenu"), x, y);
        }
    }
}

void StatusNotifierItemSource::refresh()
{
    if (!m_refreshTimer.isActive()) {
        m_refreshTimer.start();
    }
}

void StatusNotifierItemSource::refreshTitle()
{
    refresh();
}

void StatusNotifierItemSource::refreshToolTip()
{
    refresh();
}

void StatusNotifierItemSource::refreshIcons()
{
    refresh();
}

void StatusNotifierItemSource::performRefresh()
{
    if (m_refreshing) {
        m_needsReRefreshing = true;
        return;
    }

    m_refreshing = true;
    QDBusMessage message = QDBusMessage::createMethodCall(m_statusNotifierItemInterface->service(),
                                                          m_statusNotifierItemInterface->path(),
                                                          QStringLiteral("org.freedesktop.DBus.Properties"),
                                                          QStringLiteral("GetAll"));

    message << m_statusNotifierItemInterface->interface();
    QDBusPendingCall call = m_statusNotifierItemInterface->connection().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &StatusNotifierItemSource::refreshCallback);
}

void StatusNotifierItemSource::refreshCallback(QDBusPendingCallWatcher *call)
{
    m_refreshing = false;
    if (m_needsReRefreshing) {
        m_needsReRefreshing = false;
        performRefresh();
        call->deleteLater();
        return;
    }

    QDBusPendingReply<QVariantMap> reply = *call;
    if (reply.isError()) {
        m_valid = false;
    } else {
        QVariantMap properties = reply.argumentAt<0>();
        QString path = properties[QStringLiteral("IconThemePath")].toString();

        m_title = properties[QStringLiteral("Title")].toString();
        m_iconName = properties[QStringLiteral("IconName")].toString();

        // ToolTip
        ToolTip toolTip;
        properties[QStringLiteral("ToolTip")].value<QDBusArgument>() >> toolTip;
        m_tooltip = toolTip.title;

        // Menu
        QString menuObjectPath = properties[QStringLiteral("Menu")].value<QDBusObjectPath>().path();
        if (!menuObjectPath.isEmpty()) {
            if (menuObjectPath.startsWith(QLatin1String("/NO_DBUSMENU"))) {
                qWarning() << "DBusMenu disabled for this application";
            } else {
                m_menuImporter = new MenuImporter(m_statusNotifierItemInterface->service(), menuObjectPath, this);
            }
        }

        // Icon
        IconPixmapList iconPixmaps;
        properties[QStringLiteral("IconPixmap")].value<QDBusArgument>() >> iconPixmaps;

        QImage image = IconPixmapListToImage(iconPixmaps);
        if (!image.isNull()) {
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            image.save(&buffer, "PNG");
            m_iconBytes = byteArray.toBase64();
        }

        emit updated(this);
    }

    call->deleteLater();
}

QImage StatusNotifierItemSource::IconPixmapListToImage(const IconPixmapList &list) const
{
    QIcon icon;

    for (IconPixmap iconPixmap: list) {
        if (!iconPixmap.bytes.isNull()) {
            QImage image((uchar*) iconPixmap.bytes.data(), iconPixmap.width,
                         iconPixmap.height, QImage::Format_ARGB32);

            const uchar *end = image.constBits() + image.sizeInBytes();
            uchar *dest = reinterpret_cast<uchar*>(iconPixmap.bytes.data());
            for (const uchar *src = image.constBits(); src < end; src += 4, dest += 4)
                qToUnaligned(qToBigEndian<quint32>(qFromUnaligned<quint32>(src)), dest);

            icon.addPixmap(QPixmap::fromImage(image));
        }
    }

    return icon.pixmap(QSize(24, 24)).toImage();
}
