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

#include "dockbackground.h"

#include <QApplication>
#include <QPainterPath>
#include <QPainter>

DockBackground::DockBackground()
    : m_radius(0)
{
    setSmooth(true);
    setAntialiasing(true);
}

QColor DockBackground::color() const
{
    return m_color;
}

void DockBackground::setColor(QColor color)
{
    if (m_color != color) {
        m_color = color;
        update();
        emit colorChanged();
    }
}

qreal DockBackground::radius() const
{
    return m_radius;
}

void DockBackground::setRadius(qreal radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        update();
        emit radiusChanged();
    }
}

void DockBackground::paint(QPainter *painter)
{
    if (!qApp)
        return;

    // Enable antialiasing.
    painter->setRenderHint(QPainter::Antialiasing);
    painter->save();

    QRectF rect = boundingRect();
    QPainterPath path;
    QPen pen;

    // Draw background.
    path.addRoundedRect(rect.x(), rect.y(), rect.width(), rect.height(), m_radius, m_radius);
    painter->setBrush(m_color);
    painter->setPen(Qt::NoPen);
    painter->drawPath(path);

    // Draw border line.
    path.clear();
    path.addRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5),
                        m_radius - 0.5,
                        m_radius - 0.5);
    painter->setBrush(Qt::NoBrush);
    pen.setColor(QColor(0, 0, 0, 255));
    pen.setWidthF(0.5);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    path.clear();
    path.addRoundedRect(rect.adjusted(1, 1, -1, -1),
                        m_radius - 1,
                        m_radius - 1);
    painter->setBrush(Qt::NoBrush);
    pen.setColor(QColor(255, 255, 255, 255));
    pen.setWidthF(0.5);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
    painter->restore();
}

void DockBackground::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    update();
}
