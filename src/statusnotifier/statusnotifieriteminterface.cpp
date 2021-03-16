#include "statusnotifieriteminterface.h"

/*
 * Implementation of interface class StatusNotifierItemInterface
 */

StatusNotifierItemInterface::StatusNotifierItemInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

StatusNotifierItemInterface::~StatusNotifierItemInterface()
{
}
