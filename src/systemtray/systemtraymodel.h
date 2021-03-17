#ifndef SYSTEMTRAYMODEL_H
#define SYSTEMTRAYMODEL_H

#include <QAbstractListModel>

#include "statusnotifierwatcher.h"
#include "statusnotifieritemsource.h"

class SystemTrayModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        IconNameRole,
        IconRole,
        TitleRole,
        ToolTipRole
    };

    explicit SystemTrayModel(QObject *parent = nullptr);
    ~SystemTrayModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int indexOf(const QString &id);
    StatusNotifierItemSource *findItemById(const QString &id);

    Q_INVOKABLE void leftButtonClick(const QString &id);
    Q_INVOKABLE void rightButtonClick(const QString &id);

private slots:
    void onItemAdded(const QString &service);
    void onItemRemoved(const QString &service);
    void updated(StatusNotifierItemSource *item);

private:
    StatusNotifierWatcher *m_watcher;
    QList<StatusNotifierItemSource *> m_items;
    QString m_hostName;
};

#endif // SYSTEMTRAYMODEL_H
