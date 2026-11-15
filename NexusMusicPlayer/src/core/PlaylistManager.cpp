#include "PlaylistManager.h"
#include <QSqlQuery>
PlaylistManager::PlaylistManager(QSqlDatabase database,QObject *parent):QObject(parent),db(database){}
bool PlaylistManager::create(const QString &name){ if(name.trimmed().isEmpty())return false; QSqlQuery q(db);q.prepare("INSERT INTO playlists(name) VALUES(?)");q.addBindValue(name.trimmed());bool ok=q.exec();if(ok)emit playlistsChanged();return ok;}
bool PlaylistManager::remove(int id){QSqlQuery q(db);q.prepare("DELETE FROM playlists WHERE id=?");q.addBindValue(id);bool ok=q.exec();if(ok)emit playlistsChanged();return ok;}
bool PlaylistManager::addTrack(int p,int t){QSqlQuery q(db);q.prepare("INSERT OR IGNORE INTO playlist_tracks(playlist_id,track_id,position) VALUES(?,?,(SELECT COALESCE(MAX(position),-1)+1 FROM playlist_tracks WHERE playlist_id=?))");q.addBindValue(p);q.addBindValue(t);q.addBindValue(p);return q.exec();}
bool PlaylistManager::removeTrack(int p,int t){QSqlQuery q(db);q.prepare("DELETE FROM playlist_tracks WHERE playlist_id=? AND track_id=?");q.addBindValue(p);q.addBindValue(t);return q.exec();}
QVariantList PlaylistManager::all()const{QVariantList a;QSqlQuery q(db);q.exec("SELECT id,name FROM playlists ORDER BY name");while(q.next()){QVariantMap m;m["id"]=q.value(0);m["name"]=q.value(1);a<<m;}return a;}
QVariantList PlaylistManager::tracks(int p)const{QVariantList a;QSqlQuery q(db);q.prepare("SELECT t.id,t.path,t.title,t.artist,t.album,t.favorite,t.plays FROM tracks t JOIN playlist_tracks pt ON pt.track_id=t.id WHERE pt.playlist_id=? ORDER BY pt.position");q.addBindValue(p);q.exec();while(q.next()){QVariantMap m;m["id"]=q.value(0);m["path"]=q.value(1);m["title"]=q.value(2);m["artist"]=q.value(3);m["album"]=q.value(4);m["favorite"]=q.value(5);m["plays"]=q.value(6);a<<m;}return a;}
