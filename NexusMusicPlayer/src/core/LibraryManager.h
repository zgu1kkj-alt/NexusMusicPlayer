#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>

class LibraryManager : public QObject {
    Q_OBJECT
public:
    explicit LibraryManager(QSqlDatabase db, QObject *parent=nullptr);
    bool addFiles(const QStringList &paths);
    bool addFolder(const QString &folder);
    bool removeTrack(int id);
    bool setFavorite(int id, bool favorite);
    bool recordPlay(int id);
    QVariantList tracks(const QString &filter = QString()) const;
    QVariantList favorites() const;
    int trackIdForPath(const QString &path) const;
signals:
    void libraryChanged();
private:
    QSqlDatabase db;
};
