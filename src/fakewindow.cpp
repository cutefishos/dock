#include "fakewindow.h"
#include "docksettings.h"

// Qt
#include <QDebug>
#include <QApplication>
#include <QSurfaceFormat>
#include <QQuickView>
#include <QScreen>
#include <QTimer>

#include <KWindowSystem>

// X11
#include <NETWM>

FakeWindow::FakeWindow(QQuickView *parent)
    : QQuickView(parent)
    , m_delayedContainsMouse(false)
    , m_containsMouse(false)
{
    setColor(Qt::red);
    setDefaultAlphaBuffer(true);
    setFlags(Qt::FramelessWindowHint |
             Qt::WindowStaysOnTopHint |
             Qt::NoDropShadowWindowHint |
             Qt::WindowDoesNotAcceptFocus);
    setScreen(qApp->primaryScreen());
    updateGeometry();
    show();

    m_delayedMouseTimer.setSingleShot(true);
    m_delayedMouseTimer.setInterval(50);
    connect(&m_delayedMouseTimer, &QTimer::timeout, this, [this]() {
        if (m_delayedContainsMouse) {
            setContainsMouse(true);
        } else {
            setContainsMouse(false);
        }
    });

    connect(DockSettings::self(), &DockSettings::directionChanged, this, &FakeWindow::updateGeometry);
}

bool FakeWindow::containsMouse() const
{
    return m_containsMouse;
}

bool FakeWindow::event(QEvent *e)
{
    if (e->type() == QEvent::DragEnter || e->type() == QEvent::DragMove) {
        if (!m_containsMouse) {
            m_delayedContainsMouse = false;
            m_delayedMouseTimer.stop();
            setContainsMouse(true);
            emit dragEntered();
        }
    } else if (e->type() == QEvent::Enter) {
        m_delayedContainsMouse = true;
        if (!m_delayedMouseTimer.isActive()) {
            m_delayedMouseTimer.start();
        }
    } else if (e->type() == QEvent::Leave || e->type() == QEvent::DragLeave) {
        m_delayedContainsMouse = false;
        if (!m_delayedMouseTimer.isActive()) {
            m_delayedMouseTimer.start();
        }
    }

    return QQuickView::event(e);
}

void FakeWindow::setContainsMouse(bool contains)
{
    if (m_containsMouse != contains) {
        m_containsMouse = contains;
        emit containsMouseChanged(contains);
    }
}

void FakeWindow::updateGeometry()
{
    int length = 10;
    const QRect screenRect = qApp->primaryScreen()->geometry();
    QRect newRect;

    if (DockSettings::self()->direction() == DockSettings::Left) {
        newRect = QRect(screenRect.x() - (length * 2), (screenRect.height() + length) / 2,
                        length, screenRect.height());
    } else if (DockSettings::self()->direction() == DockSettings::Bottom) {
        newRect = QRect(screenRect.x(),
                         screenRect.y() + screenRect.height() - length,
                         screenRect.width(), length);
    }

    setGeometry(newRect);
}
