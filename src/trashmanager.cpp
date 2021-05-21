/*
 * Copyright (C) 2021 CutefishOS Team.
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

#include "trashmanager.h"
#include <QProcess>
#include <QDir>

const QString TrashDir = QDir::homePath() + "/.local/share/Trash";
const QDir::Filters ItemsShouldCount = QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot;

TrashManager::TrashManager(QObject *parent)
    : QObject(parent),
      m_filesWatcher(new QFileSystemWatcher(this)),
      m_count(0)
{
    onDirectoryChanged();
    connect(m_filesWatcher, &QFileSystemWatcher::directoryChanged, this, &TrashManager::onDirectoryChanged, Qt::QueuedConnection);
}

void TrashManager::emptyTrash()
{
    QProcess::startDetached("cutefish-filemanager", QStringList() << "-e");
}

void TrashManager::openTrash()
{
    QProcess::startDetached("gio", QStringList() << "open" << "trash:///");
}

void TrashManager::onDirectoryChanged()
{
    m_filesWatcher->addPath(TrashDir);

    if (QDir(TrashDir + "/files").exists()) {
        m_filesWatcher->addPath(TrashDir + "/files");
        m_count = QDir(TrashDir + "/files").entryList(ItemsShouldCount).count();
    } else {
        m_count = 0;
    }

    emit countChanged();
}
