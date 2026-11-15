#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariantList>

class DatabaseManager {
public:
    bool open();
    QSqlDatabase database() const { return db; }
    bool execute(const QString &sql);
private:
    QSqlDatabase db;
};
