#ifndef FAKEWINDOW_H
#define FAKEWINDOW_H

#include <QQuickView>
#include <QTimer>

class FakeWindow : public QQuickView
{
    Q_OBJECT

public:
    explicit FakeWindow(QQuickView *parent = nullptr);

    bool containsMouse() const;

signals:
    void containsMouseChanged(bool contains);
    void dragEntered();

protected:
    bool event(QEvent *e) override;

private:
    void setContainsMouse(bool contains);
    void updateGeometry();

private:
    QTimer m_delayedMouseTimer;

    bool m_delayedContainsMouse;
    bool m_containsMouse;
};

#endif // FAKEWINDOW_H
