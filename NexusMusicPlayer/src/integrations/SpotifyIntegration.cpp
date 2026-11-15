#include "SpotifyIntegration.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
SpotifyIntegration::SpotifyIntegration(QObject*p):QObject(p){}
void SpotifyIntegration::searchTracks(const QString&q,const QString&token){
    if(token.trimmed().isEmpty()){emit requestFailed("Informe um access token OAuth válido do Spotify.");return;}
    QUrl url("https://api.spotify.com/v1/search");QUrlQuery params;params.addQueryItem("q",q);params.addQueryItem("type","track");params.addQueryItem("limit","10");url.setQuery(params);
    QNetworkRequest req(url);req.setRawHeader("Authorization",("Bearer "+token).toUtf8());
    auto *reply=network.get(req);
    connect(reply,&QNetworkReply::finished,this,[this,reply]{
        const auto data=reply->readAll();if(reply->error()!=QNetworkReply::NoError)emit requestFailed(reply->errorString());
        else emit resultsReady(QJsonDocument::fromJson(data));reply->deleteLater();
    });
}
