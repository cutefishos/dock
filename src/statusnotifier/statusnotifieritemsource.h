#ifndef STATUSNOTIFIERITEMSOURCE_H
#define STATUSNOTIFIERITEMSOURCE_H

#include <QObject>
#include <QIcon>

#include "statusnotifieriteminterface.h"

class MenuImporter;
class StatusNotifierItemSource : public QObject
{
    Q_OBJECT

public:
    explicit StatusNotifierItemSource(const QString &id, QObject *parent = nullptr);
    ~StatusNotifierItemSource();

    QString id() const { return m_id; }
    QString title() const { return m_title; }
    QString tooltip() const { return m_tooltip; }
    QString iconName() const { return m_iconName; }
    QString iconBytes() const { return m_iconBytes; }

    void activate(int x, int y);
    void secondaryActivate(int x, int y);
    void contextMenu(int x, int y);

    MenuImporter *menuImporter() { return m_menuImporter; }

signals:
    void updated(StatusNotifierItemSource *);

private slots:
    void refresh();
    void refreshTitle();
    void refreshToolTip();
    void refreshIcons();
    void performRefresh();
    void refreshCallback(QDBusPendingCallWatcher *);
    QImage IconPixmapListToImage(const IconPixmapList &list) const;

private:
    QString m_id;
    QString m_title;
    QString m_tooltip;
    QString m_iconName;
    QString m_iconBytes;

    bool m_valid;
    bool m_refreshing;
    bool m_needsReRefreshing;
    StatusNotifierItemInterface *m_statusNotifierItemInterface;
    MenuImporter *m_menuImporter;
    QTimer m_refreshTimer;
};

#endif // STATUSNOTIFIERITEMSOURCE_H
