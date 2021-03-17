#include "statusnotifieritemjob.h"

StatusNotifierItemJob::StatusNotifierItemJob(StatusNotifierItemSource *source, QObject *parent)
    : QObject(parent)
    , m_source(source)
{
    // Queue connection, so that all 'deleteLater' are performed before we use updated menu.
    connect(source, SIGNAL(contextMenuReady(QMenu *)), this, SLOT(contextMenuReady(QMenu *)), Qt::QueuedConnection);
    connect(source, &StatusNotifierItemSource::activateResult, this, &StatusNotifierItemJob::activateCallback);
}

void StatusNotifierItemJob::start()
{

}

void StatusNotifierItemJob::activateCallback(bool success)
{

}

void StatusNotifierItemJob::contextMenuReady(QMenu *menu)
{

}
