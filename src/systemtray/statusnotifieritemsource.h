#ifndef STATUSNOTIFIERITEMSOURCE_H
#define STATUSNOTIFIERITEMSOURCE_H

#include <QObject>
#include <QMenu>
#include <QDBusPendingCallWatcher>

#include "statusnotifieritem_interface.h"

class DBusMenuImporter;
class StatusNotifierItemSource : public QObject
{
    Q_OBJECT

public:
    explicit StatusNotifierItemSource(const QString &service, QObject *parent = nullptr);
    ~StatusNotifierItemSource();

    QString id() const;
    QString title() const;
    QString tooltip() const;
    QString subtitle() const;
    QString iconName() const;
    QIcon icon() const;

    void activate(int x, int y);
    void secondaryActivate(int x, int y);
    void scroll(int delta, const QString &direction);
    void contextMenu(int x, int y);

signals:
    void contextMenuReady(QMenu *menu);
    void activateResult(bool success);
    void updated(StatusNotifierItemSource *);

private slots:
    void contextMenuReady();
    void refreshTitle();
    void refreshIcons();
    void refreshToolTip();
    void refresh();
    void performRefresh();
    void syncStatus(QString);
    void refreshCallback(QDBusPendingCallWatcher *);
    void activateCallback(QDBusPendingCallWatcher *);

private:
    QPixmap KDbusImageStructToPixmap(const KDbusImageStruct &image) const;
    QIcon imageVectorToPixmap(const KDbusImageVector &vector) const;

private:
    bool m_valid;
    QString m_name;
    QTimer m_refreshTimer;
    DBusMenuImporter *m_menuImporter;
    org::kde::StatusNotifierItem *m_statusNotifierItemInterface;
    bool m_refreshing : 1;
    bool m_needsReRefreshing : 1;
    bool m_titleUpdate : 1;
    bool m_iconUpdate : 1;
    bool m_tooltipUpdate : 1;
    bool m_statusUpdate : 1;

    QString m_id;
    QString m_title;
    QString m_tooltip;
    QString m_subTitle;
    QString m_iconName;
    QIcon m_icon;
};

#endif // STATUSNOTIFIERITEMSOURCE_H
