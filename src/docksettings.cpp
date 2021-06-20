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

#include "docksettings.h"

#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QDBusInterface>

#include <QFile>
#include <QDebug>

static DockSettings *SELF = nullptr;

DockSettings *DockSettings::self()
{
    if (SELF == nullptr)
        SELF = new DockSettings;

    return SELF;
}

DockSettings::DockSettings(QObject *parent)
    : QObject(parent)
    , m_iconSize(0)
    , m_edgeMargins(10)
    , m_roundedWindowEnabled(true)
    , m_direction(Left)
    , m_visibility(AlwaysShow)
    , m_settings(new QSettings(QSettings::UserScope, "cutefishos", "dock"))
    , m_fileWatcher(new QFileSystemWatcher(this))
{
    if (!m_settings->contains("IconSize"))
        m_settings->setValue("IconSize", 64);
    if (!m_settings->contains("Direction"))
        m_settings->setValue("Direction", Bottom);
    if (!m_settings->contains("Visibility"))
        m_settings->setValue("Visibility", AlwaysShow);
    if (!m_settings->contains("RoundedWindow"))
        m_settings->setValue("RoundedWindow", true);

    m_settings->sync();

    m_iconSize = m_settings->value("IconSize").toInt();
    m_direction = static_cast<Direction>(m_settings->value("Direction").toInt());
    m_visibility = static_cast<Visibility>(m_settings->value("Visibility").toInt());
    m_roundedWindowEnabled = m_settings->value("RoundedWindow").toBool();

    m_fileWatcher->addPath(m_settings->fileName());
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &DockSettings::onConfigFileChanged);
}

int DockSettings::iconSize() const
{
    return m_iconSize;
}

void DockSettings::setIconSize(int iconSize)
{
    m_iconSize = iconSize;
    emit iconSizeChanged();
}

DockSettings::Direction DockSettings::direction() const
{
    return m_direction;
}

void DockSettings::setDirection(const Direction &direction)
{
    m_direction = direction;
    emit directionChanged();
}

DockSettings::Visibility DockSettings::visibility() const
{
    return m_visibility;
}

void DockSettings::setVisibility(const DockSettings::Visibility &visibility)
{
    if (m_visibility != visibility) {
        m_visibility = visibility;
        emit visibilityChanged();
    }
}

int DockSettings::edgeMargins() const
{
    return m_edgeMargins;
}

void DockSettings::setEdgeMargins(int edgeMargins)
{
    m_edgeMargins = edgeMargins;
}

bool DockSettings::roundedWindowEnabled() const
{
    return m_roundedWindowEnabled;
}

void DockSettings::setRoundedWindowEnabled(bool enabled)
{
    if (m_roundedWindowEnabled != enabled) {
        m_roundedWindowEnabled = enabled;
        emit roundedWindowEnabledChanged();
    }
}

void DockSettings::onConfigFileChanged()
{
    if (!QFile(m_settings->fileName()).exists())
        return;

    m_settings->sync();

    int iconSize = m_settings->value("IconSize").toInt();
    Direction direction = static_cast<Direction>(m_settings->value("Direction").toInt());
    Visibility visibility = static_cast<Visibility>(m_settings->value("Visibility").toInt());
    bool roundedWindow = m_settings->value("RoundedWindow").toBool();

    if (m_iconSize != iconSize)
        setIconSize(iconSize);

    if (m_direction != direction)
        setDirection(direction);

    if (m_visibility != visibility)
        setVisibility(visibility);

    if (m_roundedWindowEnabled != roundedWindow)
        setRoundedWindowEnabled(roundedWindow);

    m_fileWatcher->addPath(m_settings->fileName());
}
