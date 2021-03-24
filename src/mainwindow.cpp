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

#include "mainwindow.h"
#include "processprovider.h"

#include <QGuiApplication>
#include <QScreen>

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QMetaEnum>

#include <NETWM>
#include <KWindowSystem>
#include <KWindowEffects>

MainWindow::MainWindow(QQuickView *parent)
    : QQuickView(parent)
    , m_settings(DockSettings::self())
    , m_appModel(new ApplicationModel)
    , m_fakeWindow(nullptr)
{
    setDefaultAlphaBuffer(false);
    setColor(Qt::transparent);

    setFlags(Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    KWindowSystem::setOnDesktop(winId(), NET::OnAllDesktops);
    KWindowSystem::setType(winId(), NET::Dock);

    engine()->rootContext()->setContextProperty("appModel", m_appModel);
    engine()->rootContext()->setContextProperty("process", new ProcessProvider);
    engine()->rootContext()->setContextProperty("Settings", m_settings);
    engine()->rootContext()->setContextProperty("mainWindow", this);

    setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setScreen(qApp->primaryScreen());
    setVisible(true);

    initSlideWindow();
    resizeWindow();
    onVisibilityChanged();

    connect(qApp->primaryScreen(), &QScreen::virtualGeometryChanged, this, &MainWindow::resizeWindow);
    connect(qApp->primaryScreen(), &QScreen::geometryChanged, this, &MainWindow::resizeWindow);
    connect(qApp->primaryScreen(), &QScreen::orientationChanged, this, [=] (Qt::ScreenOrientation orientation) {
        Q_UNUSED(orientation)

    });

    connect(m_appModel, &ApplicationModel::countChanged, this, &MainWindow::resizeWindow);

    connect(m_settings, &DockSettings::directionChanged, this, &MainWindow::onPositionChanged);
    connect(m_settings, &DockSettings::iconSizeChanged, this, &MainWindow::onIconSizeChanged);
    connect(m_settings, &DockSettings::visibilityChanged, this, &MainWindow::onVisibilityChanged);
}

MainWindow::~MainWindow()
{
}

QRect MainWindow::windowRect() const
{
    const QRect screenGeometry = qApp->primaryScreen()->geometry();
    const QRect availableGeometry = qApp->primaryScreen()->availableGeometry();

    bool isHorizontal = m_settings->direction() == DockSettings::Bottom;

    QSize newSize(0, 0);
    QPoint position(0, 0);
    int maxLength = isHorizontal ? screenGeometry.width() - m_settings->edgeMargins()
                                 : availableGeometry.height() - m_settings->edgeMargins();;

    int appCount = m_appModel->rowCount();
    int iconSize = m_settings->iconSize();
    int length = appCount * iconSize;

    if (length > maxLength) {
        iconSize = (maxLength - (maxLength % appCount)) / appCount;
        length = appCount * iconSize;
    }

    switch (m_settings->direction()) {
    case DockSettings::Left:
        newSize = QSize(iconSize, length);
        position.setX(screenGeometry.x() + DockSettings::self()->edgeMargins() / 2);
        // Handle the top statusbar.
        position.setY(availableGeometry.y() + (availableGeometry.height() - newSize.height()) / 2);
        break;
    case DockSettings::Bottom:
        newSize = QSize(length, iconSize);
        position.setX(screenGeometry.x() + (screenGeometry.width() - newSize.width()) / 2);
        position.setY(screenGeometry.y() + screenGeometry.height() - newSize.height() - DockSettings::self()->edgeMargins() / 2);
        break;
    case DockSettings::Right:
        newSize = QSize(iconSize, length);
        position.setX(screenGeometry.x() + screenGeometry.width() - newSize.width() - DockSettings::self()->edgeMargins() / 2);
        position.setY(availableGeometry.y() + (availableGeometry.height() - newSize.height()) / 2);
        break;
    default:
        break;
    }

    return QRect(position, newSize);
}

void MainWindow::resizeWindow()
{
    setGeometry(windowRect());
    updateViewStruts();

    emit resizingFished();
}

void MainWindow::initSlideWindow()
{
    KWindowEffects::SlideFromLocation location = KWindowEffects::NoEdge;

    if (m_settings->direction() == DockSettings::Left)
        location = KWindowEffects::LeftEdge;
    else if (m_settings->direction() == DockSettings::Right)
        location = KWindowEffects::RightEdge;
    else if (m_settings->direction() == DockSettings::Bottom)
        location = KWindowEffects::BottomEdge;

    KWindowEffects::slideWindow(winId(), location);
}

void MainWindow::updateViewStruts()
{
    XWindowInterface::instance()->setViewStruts(this, m_settings->direction(), geometry());
}

void MainWindow::createFakeWindow()
{
    if (!m_fakeWindow) {
        m_fakeWindow = new FakeWindow;

        connect(m_fakeWindow, &FakeWindow::containsMouseChanged, this, [=](bool contains) {

        });

        connect(m_fakeWindow, &FakeWindow::dragEntered, this, [&] {});

    }
}

void MainWindow::deleteFakeWindow()
{
    if (m_fakeWindow) {
        m_fakeWindow->deleteLater();
        m_fakeWindow = nullptr;
    }
}

void MainWindow::onPositionChanged()
{
    setVisible(false);
    initSlideWindow();
    setVisible(true);

    setGeometry(windowRect());
    updateViewStruts();

    emit positionChanged();
}

void MainWindow::onIconSizeChanged()
{
    setGeometry(windowRect());
    updateViewStruts();

    emit iconSizeChanged();
}

void MainWindow::onVisibilityChanged()
{
    if (m_settings->visibility() == DockSettings::AlwaysVisible)
        return;

    createFakeWindow();
}
