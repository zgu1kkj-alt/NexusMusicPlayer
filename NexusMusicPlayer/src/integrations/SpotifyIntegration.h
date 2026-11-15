#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
class SpotifyIntegration : public QObject {
    Q_OBJECT
public:
    explicit SpotifyIntegration(QObject *parent=nullptr);
    void searchTracks(const QString &query, const QString &oauthAccessToken);
signals:
    void resultsReady(const QJsonDocument &json);
    void requestFailed(const QString &message);
private:
    QNetworkAccessManager network;
};
