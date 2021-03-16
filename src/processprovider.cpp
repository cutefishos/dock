#include "processprovider.h"
#include <QProcess>

ProcessProvider::ProcessProvider(QObject *parent)
    : QObject(parent)
{

}

bool ProcessProvider::startDetached(const QString &exec, QStringList args)
{
    QProcess process;
    process.setProgram(exec);
    process.setArguments(args);
    return process.startDetached();
}
