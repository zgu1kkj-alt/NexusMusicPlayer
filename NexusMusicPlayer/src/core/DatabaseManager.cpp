#include "DatabaseManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QSqlError>
#include <QDebug>

bool DatabaseManager::open() {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir().mkpath(dir)) return false;
    if (QSqlDatabase::contains("nexus_connection"))
        db = QSqlDatabase::database("nexus_connection");
    else {
        db = QSqlDatabase::addDatabase("QSQLITE", "nexus_connection");
        db.setDatabaseName(dir + "/nexus_library.sqlite");
    }
    if (!db.open()) {
        qWarning() << "SQLite:" << db.lastError().text();
        return false;
    }
    const QStringList schema = {
        "CREATE TABLE IF NOT EXISTS tracks (id INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT UNIQUE NOT NULL, title TEXT, artist TEXT, album TEXT, favorite INTEGER DEFAULT 0, plays INTEGER DEFAULT 0, added_at TEXT DEFAULT CURRENT_TIMESTAMP)",
        "CREATE TABLE IF NOT EXISTS playlists (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE NOT NULL, cover TEXT DEFAULT '')",
        "CREATE TABLE IF NOT EXISTS playlist_tracks (playlist_id INTEGER NOT NULL, track_id INTEGER NOT NULL, position INTEGER DEFAULT 0, PRIMARY KEY(playlist_id, track_id), FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, FOREIGN KEY(track_id) REFERENCES tracks(id) ON DELETE CASCADE)",
        "CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY AUTOINCREMENT, track_id INTEGER NOT NULL, played_at TEXT DEFAULT CURRENT_TIMESTAMP)"
    };
    for (const auto &s : schema) if (!execute(s)) return false;
    QSqlQuery fk(db); fk.exec("PRAGMA foreign_keys = ON");
    return true;
}
bool DatabaseManager::execute(const QString &sql) {
    QSqlQuery q(db);
    if (!q.exec(sql)) { qWarning() << q.lastError().text() << sql; return false; }
    return true;
}
