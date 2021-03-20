#ifndef TOPLEVELMENU_H
#define TOPLEVELMENU_H

#include <QQuickView>

class TopLevelMenu : public QQuickView
{
    Q_OBJECT

public:
    explicit TopLevelMenu(QQuickView *parent = nullptr);

signals:

};

#endif // TOPLEVELMENU_H
