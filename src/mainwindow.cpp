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
    , m_trashManager(new TrashManager)
    , m_hideBlocked(false)
    , m_showTimer(new QTimer(this))
    , m_hideTimer(new QTimer(this))
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
    engine()->rootContext()->setContextProperty("trash", m_trashManager);

    setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setScreen(qApp->primaryScreen());

    initSlideWindow();
    resizeWindow();
    onVisibilityChanged();

    m_showTimer->setSingleShot(true);
    m_showTimer->setInterval(300);
    connect(m_showTimer, &QTimer::timeout, this, [=] { setVisible(true); });

    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(800);
    connect(m_hideTimer, &QTimer::timeout, this, [=] { setVisible(false); });

    connect(qApp->primaryScreen(), &QScreen::virtualGeometryChanged, this, &MainWindow::resizeWindow);
    connect(qApp->primaryScreen(), &QScreen::geometryChanged, this, &MainWindow::resizeWindow);

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

    // Add trash item.
    int appCount = m_appModel->rowCount() + 1;
    int iconSize = m_settings->iconSize();
    iconSize += iconSize * 0.1;
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
    if (m_settings->visibility() == DockSettings::AlwaysShow) {
        XWindowInterface::instance()->setViewStruts(this, m_settings->direction(), geometry());
    } else {
        clearViewStruts();
    }
}

void MainWindow::clearViewStruts()
{
    XWindowInterface::instance()->clearViewStruts(this);
}

void MainWindow::createFakeWindow()
{
    if (!m_fakeWindow) {
        installEventFilter(this);

        m_fakeWindow = new FakeWindow;

        connect(m_fakeWindow, &FakeWindow::containsMouseChanged, this, [=](bool contains) {
            switch (m_settings->visibility()) {
            case DockSettings::AlwaysHide: {

                if (contains) {
                    m_hideTimer->stop();

                    // reionwong: The mouse is moved to fakewindow,
                    // if the dock is not displayed,
                    // it will start to display.
                    if (!isVisible() && !m_showTimer->isActive()) {
                        m_showTimer->start();
                    }
                } else {
                    if (!m_hideBlocked)
                        m_hideTimer->start();
                }

                break;
            }
            default:
                break;
            }
        });

        connect(m_fakeWindow, &FakeWindow::dragEntered, this, [&] {});
    }
}

void MainWindow::deleteFakeWindow()
{
    if (m_fakeWindow) {
        removeEventFilter(this);
        disconnect(m_fakeWindow);
        m_fakeWindow->deleteLater();
        m_fakeWindow = nullptr;
    }
}

void MainWindow::onPositionChanged()
{
    if (m_settings->visibility() == DockSettings::AlwaysHide) {
        setVisible(false);
        initSlideWindow();
        // Setting geometry needs to be displayed, otherwise it will be invalid.
        setVisible(true);
        setGeometry(windowRect());
        updateViewStruts();

        m_hideTimer->start();
    }

    if (m_settings->visibility() == DockSettings::AlwaysShow) {
        setVisible(false);
        initSlideWindow();
        setVisible(true);
        setGeometry(windowRect());
        updateViewStruts();
    }

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
    // Always show
    if (m_settings->visibility() == DockSettings::AlwaysShow) {
        m_hideTimer->stop();

        setGeometry(windowRect());
        setVisible(true);
        updateViewStruts();

        // Delete fakewindow
        if (m_fakeWindow) {
            deleteFakeWindow();
        }
    }

    // Always hide
    if (m_settings->visibility() == DockSettings::AlwaysHide) {
        clearViewStruts();
        setGeometry(windowRect());
        setVisible(false);

        // Create
        if (!m_fakeWindow)
            createFakeWindow();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type()) {
    case QEvent::Enter:
        m_hideTimer->stop();
        m_hideBlocked = true;
        break;
    case QEvent::Leave:
        m_hideTimer->start();
        m_hideBlocked = false;
        break;
    case QEvent::DragEnter:
    case QEvent::DragMove:
        m_hideTimer->stop();
        break;
    case QEvent::DragLeave:
    case QEvent::Drop:
        m_hideTimer->stop();
        break;
    default:
        break;
    }

    return QQuickView::eventFilter(obj, e);
}
