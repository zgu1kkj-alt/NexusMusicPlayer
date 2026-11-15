#include "LibraryManager.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QDebug>

LibraryManager::LibraryManager(QSqlDatabase database, QObject *parent) : QObject(parent), db(database) {}
bool LibraryManager::addFiles(const QStringList &paths) {
    bool changed = false;
    for (const QString &path : paths) {
        QFileInfo fi(path);
        const QString ext = fi.suffix().toLower();
        if (!fi.exists() || !fi.isFile() || (ext != "mp3" && ext != "mp4" && ext != "m4a" && ext != "wav" && ext != "flac" && ext != "aac" && ext != "ogg" && ext != "wma")) continue;
        QString title = fi.completeBaseName();
        QString artist, album;
        const int dash = title.indexOf(" - ");
        if (dash > 0) { artist = title.left(dash).trimmed(); title = title.mid(dash + 3).trimmed(); }
        QSqlQuery q(db);
        q.prepare("INSERT OR IGNORE INTO tracks(path,title,artist,album) VALUES(?,?,?,?)");
        q.addBindValue(fi.absoluteFilePath()); q.addBindValue(title); q.addBindValue(artist); q.addBindValue(album);
        if (q.exec() && q.numRowsAffected() > 0) changed = true;
    }
    if (changed) emit libraryChanged();
    return true;
}
bool LibraryManager::addFolder(const QString &folder) {
    QStringList paths;
    QDirIterator it(folder, {"*.mp3","*.mp4","*.m4a","*.wav","*.flac","*.aac","*.ogg","*.wma"}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) paths << it.next();
    return addFiles(paths);
}
bool LibraryManager::removeTrack(int id) {
    QSqlQuery q(db); q.prepare("DELETE FROM tracks WHERE id=?"); q.addBindValue(id);
    const bool ok=q.exec(); if(ok) emit libraryChanged(); return ok;
}
bool LibraryManager::setFavorite(int id, bool favorite) {
    QSqlQuery q(db); q.prepare("UPDATE tracks SET favorite=? WHERE id=?"); q.addBindValue(favorite?1:0); q.addBindValue(id); return q.exec();
}
bool LibraryManager::recordPlay(int id) {
    QSqlQuery q(db); q.prepare("UPDATE tracks SET plays=plays+1 WHERE id=?"); q.addBindValue(id);
    if (!q.exec()) return false;
    QSqlQuery h(db); h.prepare("INSERT INTO history(track_id) VALUES(?)"); h.addBindValue(id); return h.exec();
}
QVariantList LibraryManager::tracks(const QString &filter) const {
    QVariantList out; QSqlQuery q(db);
    if (filter.isEmpty()) q.exec("SELECT id,path,title,artist,album,favorite,plays FROM tracks ORDER BY title COLLATE NOCASE");
    else {
        q.prepare("SELECT id,path,title,artist,album,favorite,plays FROM tracks WHERE title LIKE ? OR artist LIKE ? OR album LIKE ? OR path LIKE ? ORDER BY title COLLATE NOCASE");
        const QString f="%"+filter+"%"; q.addBindValue(f); q.addBindValue(f); q.addBindValue(f); q.addBindValue(f); q.exec();
    }
    while(q.next()) {
        QVariantMap m; m["id"]=q.value(0); m["path"]=q.value(1); m["title"]=q.value(2);
        m["artist"]=q.value(3); m["album"]=q.value(4); m["favorite"]=q.value(5); m["plays"]=q.value(6); out << m;
    }
    return out;
}
QVariantList LibraryManager::favorites() const {
    QVariantList out; QSqlQuery q(db); q.exec("SELECT id,path,title,artist,album,favorite,plays FROM tracks WHERE favorite=1 ORDER BY title");
    while(q.next()){ QVariantMap m; m["id"]=q.value(0);m["path"]=q.value(1);m["title"]=q.value(2);m["artist"]=q.value(3);m["album"]=q.value(4);m["favorite"]=q.value(5);m["plays"]=q.value(6);out<<m; }
    return out;
}
int LibraryManager::trackIdForPath(const QString &path) const {
    QSqlQuery q(db); q.prepare("SELECT id FROM tracks WHERE path=?"); q.addBindValue(path); return q.exec()&&q.next()?q.value(0).toInt():-1;
}
