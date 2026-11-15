#pragma once
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QStringList>

class MusicPlayer : public QObject {
    Q_OBJECT
public:
    explicit MusicPlayer(QObject *parent=nullptr);
    void setQueue(const QStringList &paths, int startIndex=0);
    void playPath(const QString &path);
    void play(); void pause(); void toggle(); void next(); void previous();
    void seek(qint64 position); void setVolume(int percent); void setMuted(bool muted);
    void setShuffle(bool enabled); void setRepeatMode(int mode);
    QMediaPlayer *mediaPlayer() const { return player; }
    int currentIndex() const { return index; }
    QString currentPath() const { return current; }
    int volume() const { return qRound(output->volume()*100); }
signals:
    void trackChanged(const QString &path);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void playbackChanged(bool playing);
    void errorOccurred(const QString &message);
private:
    void playCurrent();
    QMediaPlayer *player;
    QAudioOutput *output;
    QStringList queue;
    QString current;
    int index=-1;
    bool shuffle=false;
    int repeatMode=0; // 0 off, 1 track, 2 queue
};
