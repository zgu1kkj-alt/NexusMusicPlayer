#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>

class PlaylistManager : public QObject {
    Q_OBJECT
public:
    explicit PlaylistManager(QSqlDatabase db, QObject *parent=nullptr);
    bool create(const QString &name);
    bool remove(int id);
    bool addTrack(int playlistId, int trackId);
    bool removeTrack(int playlistId, int trackId);
    QVariantList all() const;
    QVariantList tracks(int playlistId) const;
signals:
    void playlistsChanged();
private:
    QSqlDatabase db;
};
