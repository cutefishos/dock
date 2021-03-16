#ifndef VOLUMEMANAGER_H
#define VOLUMEMANAGER_H

#include <QObject>

class VolumeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isValid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool isMute READ isMute NOTIFY muteChanged)
    Q_PROPERTY(int volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY volumeChanged)

public:
    static VolumeManager *self();

    explicit VolumeManager(QObject *parent = nullptr);

    bool isValid() const;
    bool isMute() const;
    int volume() const;

    QString iconName() const;

    Q_INVOKABLE void toggleMute();
    Q_INVOKABLE void setMute(bool mute);
    Q_INVOKABLE void setVolume(int value);

signals:
    void validChanged();
    void muteChanged();
    void volumeChanged();

private:
    void init();
    void initStatus();
    void connectDBusSignals();

private slots:
    void onDBusVolumeChanged(int volume);
    void onDBusMuteChanged(bool mute);

private:
    bool m_isValid;
    bool m_isMute;
    int m_volume;
};

#endif // VOLUMEMANAGER_H
