#include "controlcenterdialog.h"
#include "xwindowinterface.h"
#include <KWindowSystem>

ControlCenterDialog::ControlCenterDialog(QQuickView *parent)
    : QQuickView(parent)
{
    setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    connect(this, &QQuickView::activeChanged, this, [=] {
        if (!isActive())
            hide();
    });
}

void ControlCenterDialog::showEvent(QShowEvent *event)
{
    KWindowSystem::setState(winId(), NET::SkipTaskbar | NET::SkipPager | NET::SkipSwitcher);
    QQuickView::showEvent(event);
}

void ControlCenterDialog::hideEvent(QHideEvent *event)
{
    QQuickView::hideEvent(event);
}
