#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QStackedWidget>
#include <QVideoWidget>
#include "core/DatabaseManager.h"
#include "core/LibraryManager.h"
#include "core/PlaylistManager.h"
#include "core/SettingsManager.h"
#include "player/MusicPlayer.h"
#include "player/VideoPlayer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
private:
    void buildUi();
    void refreshLibrary();
    void refreshPlaylists();
    void showTracks(const QVariantList &tracks);
    void playSelected();
    void addPaths(const QStringList &paths);
    QStringList visiblePaths() const;
    QString formatTime(qint64 ms) const;
    DatabaseManager database;
    LibraryManager *library=nullptr;
    PlaylistManager *playlists=nullptr;
    SettingsManager settings;
    MusicPlayer *player=nullptr;
    VideoPlayer *video=nullptr;
    QListWidget *trackList=nullptr;
    QListWidget *playlistList=nullptr;
    QLineEdit *search=nullptr;
    QLabel *nowTitle=nullptr,*nowArtist=nullptr,*timeLabel=nullptr;
    QPushButton *playButton=nullptr,*favoriteButton=nullptr;
    QSlider *progress=nullptr,*volume=nullptr;
    QVariantList currentTracks;
    bool seeking=false;
};
