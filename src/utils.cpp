/*
 * Copyright (C) 2021 CutefishOS Team.
 *
 * Author:     rekols <revenmartin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils.h"
#include "systemappmonitor.h"
#include "systemappitem.h"

#include <QFile>
#include <QFileInfo>
#include <QUrlQuery>
#include <QSettings>
#include <QDebug>

#include <KWindowSystem>

static Utils *INSTANCE = nullptr;

Utils *Utils::instance()
{
    if (!INSTANCE)
        INSTANCE = new Utils;

    return INSTANCE;
}

Utils::Utils(QObject *parent)
    : QObject(parent)
    , m_sysAppMonitor(SystemAppMonitor::self())
{

}

QString Utils::cmdFromPid(quint32 pid)
{
    QFile file(QString("/proc/%1/cmdline").arg(pid));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString cmd = QString::fromUtf8(file.readAll());
    QString bin;

    for (int i = 0; i < cmd.size(); ++i) {
        const QChar ch = cmd[i];
        if (ch == '\\')
            i++;
        else if (ch == ' ')
            break;
        else
            bin += ch;
    }

    if (bin.split("/").last() == "electron")
        return QString();

    if (bin.startsWith("/"))
        return bin.split("/").last();

    return bin;
}

QString Utils::desktopPathFromMetadata(const QString &appId, quint32 pid, const QString &xWindowWMClassName)
{
    QString result;

    if (!appId.isEmpty() && !xWindowWMClassName.isEmpty()) {
        for (SystemAppItem *item : m_sysAppMonitor->applications()) {
            // Start search.
            const QFileInfo desktopFileInfo(item->path);
            const QString cmdline = cmdFromPid(pid);

            bool founded = false;

            if (desktopFileInfo.baseName() == xWindowWMClassName ||
                desktopFileInfo.completeBaseName() == xWindowWMClassName)
                founded = true;

            // StartupWMClass=STRING
            // If true, it is KNOWN that the application will map at least one
            // window with the given string as its WM class or WM name hint.
            // ref: https://specifications.freedesktop.org/startup-notification-spec/startup-notification-0.1.txt
            if (item->startupWMClass.startsWith(appId, Qt::CaseInsensitive) ||
                item->startupWMClass.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            if (!founded && item->iconName.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            // Icon name and cmdline.
            if (!founded && item->iconName == cmdline)
                founded = true;

            // Exec name and cmdline.
            if (!founded && item->exec == cmdline)
                founded = true;

            // Try matching mapped name against 'Name'.
            if (!founded && item->name.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            // exec
            if (!founded && item->exec.startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            if (!founded && desktopFileInfo.baseName().startsWith(xWindowWMClassName, Qt::CaseInsensitive))
                founded = true;

            if (founded) {
                result = item->path;
                break;
            }
        }
    }

    return result;
}

QMap<QString, QString> Utils::readInfoFromDesktop(const QString &desktopFile)
{
    QMap<QString, QString> info;
    for (SystemAppItem *item : m_sysAppMonitor->applications()) {
        if (item->path == desktopFile) {
            info.insert("Icon", item->iconName);
            info.insert("Name", item->name);
            info.insert("Exec", item->exec);
        }
    }

    return info;
}
