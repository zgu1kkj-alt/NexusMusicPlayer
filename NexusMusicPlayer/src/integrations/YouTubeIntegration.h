#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
class YouTubeIntegration : public QObject {
    Q_OBJECT
public:
    explicit YouTubeIntegration(QObject *parent=nullptr);
    void searchVideos(const QString &query, const QString &apiKey);
signals:
    void resultsReady(const QJsonDocument &json);
    void requestFailed(const QString &message);
private:
    QNetworkAccessManager network;
};
