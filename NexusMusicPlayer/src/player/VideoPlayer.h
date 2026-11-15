#pragma once
#include <QWidget>
#include <QVideoWidget>
#include <QPushButton>
#include "../core/SettingsManager.h"
class VideoPlayer : public QWidget {
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent=nullptr);
    void setVideoVisible(bool visible);
    bool videoVisible() const;
    QVideoWidget *videoWidget() const { return video; }
signals:
    void visibilityChanged(bool visible);
private:
    QVideoWidget *video;
    QPushButton *hideButton;
};
