#ifndef STATUSNOTIFIERITEMJOB_H
#define STATUSNOTIFIERITEMJOB_H

#include <QThread>
#include <QMenu>

#include "statusnotifieritemsource.h"

class StatusNotifierItemJob : public QObject
{
    Q_OBJECT

public:
    explicit StatusNotifierItemJob(StatusNotifierItemSource *source, QObject *parent = nullptr);

protected:
    void start();

private Q_SLOTS:
    void activateCallback(bool success);
    void contextMenuReady(QMenu *menu);

private:
    StatusNotifierItemSource *m_source;
};

#endif // STATUSNOTIFIERITEMJOB_H
