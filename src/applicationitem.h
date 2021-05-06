#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include <QtCore>

class ApplicationItem
{
public:
    // window class
    QString id;
    // icon name
    QString iconName;
    // visible name
    QString visibleName;
    QString desktopPath;
    QString exec;

    QList<quint64> wids;

    int currentActive = 0;
    bool isActive = false;
    bool isPinned = false;
    bool fixed = false;

    bool operator==(ApplicationItem item) {
        return item.id == this->id;
    }
};

#endif // APPLICATIONITEM_H
