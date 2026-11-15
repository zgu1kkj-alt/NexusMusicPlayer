#include "SettingsManager.h"
SettingsManager::SettingsManager(QObject *p):QObject(p),settings("Nexus","NexusMusicPlayer"){}
int SettingsManager::volume()const{return settings.value("volume",65).toInt();}
void SettingsManager::setVolume(int v){settings.setValue("volume",qBound(0,v,100));}
QString SettingsManager::musicFolder()const{return settings.value("musicFolder",QString()).toString();}
void SettingsManager::setMusicFolder(const QString&p){settings.setValue("musicFolder",p);}
bool SettingsManager::autoplay()const{return settings.value("autoplay",true).toBool();}
void SettingsManager::setAutoplay(bool v){settings.setValue("autoplay",v);}
