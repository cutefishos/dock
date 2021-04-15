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

#ifndef DOCKBACKGROUND_H
#define DOCKBACKGROUND_H

#include <QQuickPaintedItem>

class DockBackground : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)

public:
    DockBackground();

    QColor color() const;
    void setColor(QColor color);

    qreal radius() const;
    void setRadius(qreal radius);

    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:
    void colorChanged();
    void radiusChanged();

private:
    qreal m_radius;

    QColor m_color;
};

#endif // DOCKBACKGROUND_H
