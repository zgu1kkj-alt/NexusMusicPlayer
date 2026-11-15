#include "VideoPlayer.h"
#include <QVBoxLayout>
VideoPlayer::VideoPlayer(QWidget*p):QWidget(p),video(new QVideoWidget(this)),hideButton(new QPushButton("Ocultar vídeo",this)){
    auto *l=new QVBoxLayout(this);l->setContentsMargins(0,0,0,0);l->addWidget(video);l->addWidget(hideButton);
    setStyleSheet("QWidget{background:#080a12;color:white;} QPushButton{background:#25283a;border:0;border-radius:8px;padding:8px;color:white;}");
    connect(hideButton,&QPushButton::clicked,this,[this]{setVideoVisible(false);});
    hide();
}
void VideoPlayer::setVideoVisible(bool v){video->setVisible(v);hideButton->setVisible(v);setVisible(v);emit visibilityChanged(v);}
bool VideoPlayer::videoVisible()const{return isVisible();}
