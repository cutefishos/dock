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

#include "docksettings.h"
#include "applicationmodel.h"
#include "fakewindow.h"
#include "trashmanager.h"

class MainWindow : public QQuickView
{
    Q_OBJECT

public:
    explicit MainWindow(QQuickView *parent = nullptr);
    ~MainWindow();

signals:
    void resizingFished();
    void iconSizeChanged();
    void positionChanged();

private:
    QRect windowRect() const;
    void resizeWindow();
    void initSlideWindow();
    void updateViewStruts();
    void clearViewStruts();

    void createFakeWindow();
    void deleteFakeWindow();

private slots:
    void onPositionChanged();
    void onIconSizeChanged();
    void onVisibilityChanged();

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    DockSettings *m_settings;
    ApplicationModel *m_appModel;
    FakeWindow *m_fakeWindow;
    TrashManager *m_trashManager;

    bool m_hideBlocked;

    QTimer *m_showTimer;
    QTimer *m_hideTimer;
};

#endif // MAINWINDOW_H
