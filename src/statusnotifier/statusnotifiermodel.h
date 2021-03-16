#ifndef STATUSNOTIFIERMODEL_H
#define STATUSNOTIFIERMODEL_H

#include <QAbstractListModel>
#include <QIcon>
#include <QMenu>

#include <dbusmenu-qt5/dbusmenuimporter.h>

#include "statusnotifieritemsource.h"
#include "statusnotifierwatcher.h"
#include "sniasync.h"

class StatusNotifierModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        IconNameRole,
        IconBytesRole,
        IconRole,
        TitleRole,
        ToolTipRole
    };

    explicit StatusNotifierModel(QObject *parent = nullptr);
    ~StatusNotifierModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int indexOf(const QString &id);
    StatusNotifierItemSource *findItemById(const QString &id);

    Q_INVOKABLE void leftButtonClick(const QString &id);
    Q_INVOKABLE void rightButtonClick(const QString &id);

public slots:
    void itemAdded(QString serviceAndPath);
    void itemRemoved(const QString &serviceAndPath);
    void updated(StatusNotifierItemSource *item);

private:
    StatusNotifierWatcher *m_watcher;
    QList<StatusNotifierItemSource *> m_items;
};

#endif // STATUSNOTIFIERMODEL_H
