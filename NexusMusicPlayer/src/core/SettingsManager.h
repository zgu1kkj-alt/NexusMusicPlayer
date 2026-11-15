#pragma once
#include <QObject>
#include <QSettings>
class SettingsManager : public QObject {
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent=nullptr);
    int volume() const;
    void setVolume(int value);
    QString musicFolder() const;
    void setMusicFolder(const QString &path);
    bool autoplay() const;
    void setAutoplay(bool value);
private:
    QSettings settings;
};
