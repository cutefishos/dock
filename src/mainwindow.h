/*
 * Copyright (C) 2021 CutefishOS Team.
 *
 * Author:     Reion Wong <reionwong@gmail.com>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QQuickView>
#include <QTimer>

#include "activity.h"
#include "docksettings.h"
#include "applicationmodel.h"
#include "fakewindow.h"
#include "trashmanager.h"

class MainWindow : public QQuickView
{
    Q_OBJECT
    Q_PROPERTY(QRect primaryGeometry READ primaryGeometry NOTIFY primaryGeometryChanged)
    Q_PROPERTY(int direction READ direction NOTIFY directionChanged)

public:
    explicit MainWindow(QQuickView *parent = nullptr);
    ~MainWindow();

    // DBus interface
    void add(const QString &desktop);
    void remove(const QString &desktop);
    bool pinned(const QString &desktop);

    QRect primaryGeometry() const;
    int direction() const;

    void setDirection(int direction);
    void setIconSize(int iconSize);
    void setVisibility(int visibility);

    Q_INVOKABLE void updateSize();

signals:
    void resizingFished();
    void iconSizeChanged();
    void directionChanged();
    void primaryGeometryChanged();

private:
    QRect windowRect() const;
    void resizeWindow();
    void initScreens();
    void initSlideWindow();
    void updateViewStruts();
    void clearViewStruts();

    void createFakeWindow();
    void deleteFakeWindow();

private slots:
    void onPrimaryScreenChanged(QScreen *screen);
    void onPositionChanged();
    void onIconSizeChanged();
    void onVisibilityChanged();

    void onHideTimeout();

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;
    void resizeEvent(QResizeEvent *) override;

private:
    Activity *m_activity;
    DockSettings *m_settings;
    ApplicationModel *m_appModel;
    FakeWindow *m_fakeWindow;
    TrashManager *m_trashManager;

    bool m_hideBlocked;

    QTimer *m_showTimer;
    QTimer *m_hideTimer;
};

#endif // MAINWINDOW_H
