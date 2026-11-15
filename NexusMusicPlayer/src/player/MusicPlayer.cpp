#include "MusicPlayer.h"
#include <QFileInfo>
#include <QRandomGenerator>
MusicPlayer::MusicPlayer(QObject *p):QObject(p),player(new QMediaPlayer(this)),output(new QAudioOutput(this)){
    player->setAudioOutput(output);
    connect(player,&QMediaPlayer::positionChanged,this,&MusicPlayer::positionChanged);
    connect(player,&QMediaPlayer::durationChanged,this,&MusicPlayer::durationChanged);
    connect(player,&QMediaPlayer::playbackStateChanged,this,[this](QMediaPlayer::PlaybackState s){emit playbackChanged(s==QMediaPlayer::PlayingState);});
    connect(player,&QMediaPlayer::mediaStatusChanged,this,[this](QMediaPlayer::MediaStatus s){
        if(s==QMediaPlayer::EndOfMedia){if(repeatMode==1)playCurrent();else next();}
    });
    connect(player,&QMediaPlayer::errorOccurred,this,[this](QMediaPlayer::Error,const QString &e){emit errorOccurred(e);});
}
void MusicPlayer::setQueue(const QStringList&p,int i){queue=p;index=(queue.isEmpty()?-1:qBound(0,i,queue.size()-1));playCurrent();}
void MusicPlayer::playPath(const QString&p){queue={p};index=0;playCurrent();}
void MusicPlayer::playCurrent(){if(index<0||index>=queue.size())return;current=queue.at(index);player->setSource(QUrl::fromLocalFile(current));player->play();emit trackChanged(current);}
void MusicPlayer::play(){player->play();} void MusicPlayer::pause(){player->pause();}
void MusicPlayer::toggle(){player->playbackState()==QMediaPlayer::PlayingState?pause():play();}
void MusicPlayer::next(){if(queue.isEmpty())return;if(shuffle&&queue.size()>1){int n=index;while(n==index)n=QRandomGenerator::global()->bounded(queue.size());index=n;}else if(index+1<queue.size())++index;else if(repeatMode==2)index=0;else{player->stop();return;}playCurrent();}
void MusicPlayer::previous(){if(player->position()>3000){player->setPosition(0);return;}if(index>0)--index;else if(repeatMode==2)index=queue.size()-1;playCurrent();}
void MusicPlayer::seek(qint64 p){player->setPosition(p);}
void MusicPlayer::setVolume(int v){output->setVolume(qBound(0,v,100)/100.0);}
void MusicPlayer::setMuted(bool m){output->setMuted(m);}
void MusicPlayer::setShuffle(bool e){shuffle=e;}
void MusicPlayer::setRepeatMode(int m){repeatMode=qBound(0,m,2);}
