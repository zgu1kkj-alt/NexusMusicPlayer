#include "YouTubeIntegration.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
YouTubeIntegration::YouTubeIntegration(QObject*p):QObject(p){}
void YouTubeIntegration::searchVideos(const QString&q,const QString&key){
    if(key.trimmed().isEmpty()){emit requestFailed("Configure uma chave da YouTube Data API v3.");return;}
    QUrl url("https://www.googleapis.com/youtube/v3/search");QUrlQuery params;
    params.addQueryItem("part","snippet");params.addQueryItem("type","video");params.addQueryItem("maxResults","10");params.addQueryItem("q",q);params.addQueryItem("key",key);url.setQuery(params);
    auto *reply=network.get(QNetworkRequest(url));
    connect(reply,&QNetworkReply::finished,this,[this,reply]{
        auto data=reply->readAll();if(reply->error()!=QNetworkReply::NoError)emit requestFailed(reply->errorString());
        else emit resultsReady(QJsonDocument::fromJson(data));reply->deleteLater();
    });
}
