#include "toplevelmenu.h"

TopLevelMenu::TopLevelMenu(QQuickView *parent)
    : QQuickView(parent)
{
    setFlags(Qt::Popup);
}
