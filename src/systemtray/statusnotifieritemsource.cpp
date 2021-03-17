#include "statusnotifieritemsource.h"
#include "systemtraytypes.h"

#include <QDebug>
#include <dbusmenuimporter.h>
#include <netinet/in.h>

class MenuImporter : public DBusMenuImporter
{
public:
    using DBusMenuImporter::DBusMenuImporter;

protected:
    QIcon iconForName(const QString & name) override {
        return QIcon::fromTheme(name);
    }
};

StatusNotifierItemSource::StatusNotifierItemSource(const QString &notifierItemId, QObject *parent)
    : QObject(parent)
    , m_menuImporter(nullptr)
    , m_refreshing(false)
    , m_needsReRefreshing(false)
    , m_titleUpdate(true)
    , m_iconUpdate(true)
    , m_tooltipUpdate(true)
    , m_statusUpdate(true)
    , m_id(notifierItemId)
{
    setObjectName(notifierItemId);

    qDBusRegisterMetaType<KDbusImageStruct>();
    qDBusRegisterMetaType<KDbusImageVector>();
    qDBusRegisterMetaType<KDbusToolTipStruct>();

    m_name = notifierItemId;

    int slash = notifierItemId.indexOf('/');
    if (slash == -1) {
        qWarning() << "Invalid notifierItemId:" << notifierItemId;
        m_valid = false;
        m_statusNotifierItemInterface = nullptr;
        return;
    }

    QString service = notifierItemId.left(slash);
    QString path = notifierItemId.mid(slash);

    m_statusNotifierItemInterface = new org::kde::StatusNotifierItem(service, path, QDBusConnection::sessionBus(), this);

    m_refreshTimer.setSingleShot(true);
    m_refreshTimer.setInterval(10);
    connect(&m_refreshTimer, &QTimer::timeout, this, &StatusNotifierItemSource::performRefresh);

    m_valid = !service.isEmpty() && m_statusNotifierItemInterface->isValid();

    if (m_valid) {
        connect(m_statusNotifierItemInterface, &OrgKdeStatusNotifierItem::NewTitle, this, &StatusNotifierItemSource::refreshTitle);
        connect(m_statusNotifierItemInterface, &OrgKdeStatusNotifierItem::NewIcon, this, &StatusNotifierItemSource::refreshIcons);
        connect(m_statusNotifierItemInterface, &OrgKdeStatusNotifierItem::NewAttentionIcon, this, &StatusNotifierItemSource::refreshIcons);
        connect(m_statusNotifierItemInterface, &OrgKdeStatusNotifierItem::NewOverlayIcon, this, &StatusNotifierItemSource::refreshIcons);
        connect(m_statusNotifierItemInterface, &OrgKdeStatusNotifierItem::NewToolTip, this, &StatusNotifierItemSource::refreshToolTip);
        connect(m_statusNotifierItemInterface, &OrgKdeStatusNotifierItem::NewStatus, this, &StatusNotifierItemSource::syncStatus);
        refresh();
    }
}

StatusNotifierItemSource::~StatusNotifierItemSource()
{
    delete m_statusNotifierItemInterface;
}

QString StatusNotifierItemSource::id() const
{
    return m_id;
}

QString StatusNotifierItemSource::title() const
{
    return m_title;
}

QString StatusNotifierItemSource::tooltip() const
{
    return m_tooltip;
}

QString StatusNotifierItemSource::subtitle() const
{
    return m_subTitle;
}

QString StatusNotifierItemSource::iconName() const
{
    return m_iconName;
}

QIcon StatusNotifierItemSource::icon() const
{
    return m_icon;
}

void StatusNotifierItemSource::activate(int x, int y)
{
    if (m_statusNotifierItemInterface && m_statusNotifierItemInterface->isValid()) {
        QDBusMessage message = QDBusMessage::createMethodCall(m_statusNotifierItemInterface->service(),
                                                              m_statusNotifierItemInterface->path(),
                                                              m_statusNotifierItemInterface->interface(),
                                                              QStringLiteral("Activate"));

        message << x << y;
        QDBusPendingCall call = m_statusNotifierItemInterface->connection().asyncCall(message);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &StatusNotifierItemSource::activateCallback);
    }
}

void StatusNotifierItemSource::secondaryActivate(int x, int y)
{
    if (m_statusNotifierItemInterface && m_statusNotifierItemInterface->isValid()) {
        m_statusNotifierItemInterface->call(QDBus::NoBlock, QStringLiteral("SecondaryActivate"), x, y);
    }
}

void StatusNotifierItemSource::scroll(int delta, const QString &direction)
{
    if (m_statusNotifierItemInterface && m_statusNotifierItemInterface->isValid()) {
        m_statusNotifierItemInterface->call(QDBus::NoBlock, QStringLiteral("Scroll"), delta, direction);
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

void StatusNotifierItemSource::contextMenuReady()
{
    emit contextMenuReady(m_menuImporter->menu());
}

void StatusNotifierItemSource::refreshTitle()
{
    m_titleUpdate = true;
    refresh();
}

void StatusNotifierItemSource::refreshIcons()
{
    m_iconUpdate = true;
    refresh();
}

void StatusNotifierItemSource::refreshToolTip()
{
    m_tooltipUpdate = true;
    refresh();
}

void StatusNotifierItemSource::refresh()
{
    if (!m_refreshTimer.isActive()) {
        m_refreshTimer.start();
    }
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

void StatusNotifierItemSource::syncStatus(QString)
{

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
        KDbusToolTipStruct toolTip;
        properties[QStringLiteral("ToolTip")].value<QDBusArgument>() >> toolTip;
        m_tooltip = toolTip.title;

        // Icon
        KDbusImageVector image;
        properties[QStringLiteral("IconPixmap")].value<QDBusArgument>() >> image;
        if (!image.isEmpty()) {
            m_icon = imageVectorToPixmap(image);
        }

//        QString newTitle;
//        QString newIconName;
//        QString newToolTip;

//        QString overlayIconName = properties[QStringLiteral("OverlayIconName")].toString();
//        QString iconName = properties[QStringLiteral("IconName")].toString();

//        bool changed = false;

//        newTitle = properties[QStringLiteral("Title")].toString();

//        if (!overlayIconName.isEmpty())
//            newIconName = iconName;
//        if (!iconName.isEmpty())
//            newIconName = iconName;

//        KDbusToolTipStruct toolTip;
//        properties[QStringLiteral("ToolTip")].value<QDBusArgument>() >> toolTip;
//        // newToolTip = !toolTip.title.isEmpty() ? toolTip.title : toolTip.subTitle;

//        if (newTitle != m_title) {
//            m_title = newTitle;
//            changed = true;
//        }

//        if (newIconName != m_iconName) {
//            m_iconName = iconName;
//            changed = true;
//        }

//        if (newToolTip != m_tooltip) {
//            m_tooltip = newToolTip;
//            changed = true;
//        }

//        // Icon
//        KDbusImageVector image;
//        properties[QStringLiteral("AttentionIconPixmap")].value<QDBusArgument>() >> image;
//        if (!image.isEmpty()) {
//            m_icon = imageVectorToPixmap(image);
//            changed = true;
//        }

//        properties[QStringLiteral("IconPixmap")].value<QDBusArgument>() >> image;
//        if (!image.isEmpty()) {
//            m_icon = imageVectorToPixmap(image);
//            changed = true;
//        }

        // Menu
        if (!m_menuImporter) {
            QString menuObjectPath = properties[QStringLiteral("Menu")].value<QDBusObjectPath>().path();
            if (!menuObjectPath.isEmpty()) {
                if (menuObjectPath.startsWith(QLatin1String("/NO_DBUSMENU"))) {
                    // This is a hack to make it possible to disable DBusMenu in an
                    // application. The string "/NO_DBUSMENU" must be the same as in
                    // KStatusNotifierItem::setContextMenu().
                    qWarning() << "DBusMenu disabled for this application";
                } else {
                    m_menuImporter = new MenuImporter(m_statusNotifierItemInterface->service(),
                                                      menuObjectPath, this);
                }
            }
        }

        // qDebug() << newTitle << newIconName << newToolTip << image.isEmpty();

        emit updated(this);
    }

    call->deleteLater();
}

void StatusNotifierItemSource::activateCallback(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<void> reply = *call;
    emit activateResult(!reply.isError());
    call->deleteLater();
}

QPixmap StatusNotifierItemSource::KDbusImageStructToPixmap(const KDbusImageStruct &image) const
{
    // swap from network byte order if we are little endian
    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
        uint *uintBuf = (uint *)image.data.data();
        for (uint i = 0; i < image.data.size() / sizeof(uint); ++i) {
            *uintBuf = ntohl(*uintBuf);
            ++uintBuf;
        }
    }
    if (image.width == 0 || image.height == 0) {
        return QPixmap();
    }

    // avoid a deep copy of the image data
    // we need to keep a reference to the image.data alive for the lifespan of the image, even if the image is copied
    // we create a new QByteArray with a shallow copy of the original data on the heap, then delete this in the QImage cleanup
    auto dataRef = new QByteArray(image.data);

    QImage iconImage(
        reinterpret_cast<const uchar *>(dataRef->data()),
        image.width,
        image.height,
        QImage::Format_ARGB32,
        [](void *ptr) {
            delete static_cast<QByteArray *>(ptr);
        },
        dataRef);
    return QPixmap::fromImage(iconImage);
}

QIcon StatusNotifierItemSource::imageVectorToPixmap(const KDbusImageVector &vector) const
{
    QIcon icon;

    for (int i = 0; i < vector.size(); ++i) {
        icon.addPixmap(KDbusImageStructToPixmap(vector[i]));
    }

    return icon;
}
