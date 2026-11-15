#include "MainWindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QShortcut>
#include <QUrl>
#include <QFileInfo>
#include <QDesktopServices>
#include <QSqlQuery>
#include <QSqlError>
#include <QStyle>
#include <QStandardPaths>
#include <climits>
#include <QDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <climits>

static const char *STYLE = R"(
QMainWindow,QWidget{background:#0b0d14;color:#eef0fa;font-family:'Segoe UI';font-size:10pt;}
QFrame#Sidebar{background:#10131e;border:1px solid #202437;border-radius:16px;}
QLabel#Brand{font-size:20pt;font-weight:800;color:#a78bfa;padding:8px;}
QPushButton{background:#1b2030;border:1px solid #292f45;border-radius:10px;padding:9px 13px;color:#e9eafa;}
QPushButton:hover{background:#292f45;border-color:#8064dc;}
QPushButton:checked{background:#6d4bd1;color:white;}
QLineEdit{background:#151a28;border:1px solid #292f45;border-radius:12px;padding:11px;color:white;selection-background-color:#7655d9;}
QListWidget{background:#111522;border:1px solid #242a3e;border-radius:12px;padding:7px;outline:0;}
QListWidget::item{padding:11px;border-radius:8px;margin:2px;}
QListWidget::item:selected{background:#30264e;color:#fff;}
QListWidget::item:hover{background:#20263a;}
QSlider::groove:horizontal{height:5px;background:#30364a;border-radius:2px;}
QSlider::sub-page:horizontal{background:#9675f4;border-radius:2px;}
QSlider::handle:horizontal{background:#fff;width:12px;margin:-5px 0;border-radius:6px;}
QStatusBar{background:#10131e;color:#9da5bd;}
)");

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),settings(this),player(new MusicPlayer(this)),video(new VideoPlayer(this)){
    setWindowTitle("Nexus Music Player");resize(1280,800);setMinimumSize(920,620);setAcceptDrops(true);setStyleSheet(STYLE);
    if(!database.open()){QMessageBox::critical(this,"Nexus","Não foi possível abrir o banco SQLite.");}
    library=new LibraryManager(database.database(),this);playlists=new PlaylistManager(database.database(),this);
    buildUi();
    connect(library,&LibraryManager::libraryChanged,this,&MainWindow::refreshLibrary);
    connect(playlists,&PlaylistManager::playlistsChanged,this,&MainWindow::refreshPlaylists);
    connect(player,&MusicPlayer::trackChanged,this,[this](const QString &path){
        QFileInfo fi(path);nowTitle->setText(fi.completeBaseName());nowArtist->setText(fi.suffix().toUpper()+" • Arquivo local");
        playButton->setText("Ⅱ");library->recordPlay(library->trackIdForPath(path));
        statusBar()->showMessage("Reproduzindo: "+fi.fileName(),3500);
    });
    connect(player,&MusicPlayer::playbackChanged,this,[this](bool p){playButton->setText(p?"Ⅱ":"▶");});
    connect(player,&MusicPlayer::positionChanged,this,[this](qint64 p){if(!seeking){progress->setValue(int(p));timeLabel->setText(formatTime(p)+" / "+formatTime(player->mediaPlayer()->duration()));}});
    connect(player,&MusicPlayer::durationChanged,this,[this](qint64 d){progress->setRange(0,int(qMin<qint64>(d,INT_MAX)));});
    connect(player,&MusicPlayer::errorOccurred,this,[this](const QString&e){statusBar()->showMessage("Erro de mídia: "+e,7000);});
    player->setVolume(settings.volume());
    refreshLibrary();refreshPlaylists();
    player->mediaPlayer()->setVideoOutput(video->videoWidget());
    video->setVideoVisible(false);
    new QShortcut(QKeySequence(Qt::Key_Space),this,[this]{player->toggle();});
    new QShortcut(QKeySequence(Qt::CTRL|Qt::Key_O),this,[this]{QStringList p=QFileDialog::getOpenFileNames(this,"Adicionar músicas",{},"Mídia (*.mp3 *.mp4 *.m4a *.wav *.flac *.aac *.ogg)");addPaths(p);});
    new QShortcut(QKeySequence(Qt::CTRL|Qt::Key_F),this,[this]{search->setFocus();});
}
void MainWindow::buildUi(){
    auto *central=new QWidget;setCentralWidget(central);auto *root=new QVBoxLayout(central);root->setContentsMargins(16,16,16,12);root->setSpacing(12);
    auto *top=new QHBoxLayout;
    auto *sidebar=new QFrame;sidebar->setObjectName("Sidebar");sidebar->setFixedWidth(205);auto *side=new QVBoxLayout(sidebar);
    auto *brand=new QLabel("NEXUS");brand->setObjectName("Brand");side->addWidget(brand);
    auto *home=new QPushButton("⌂   Início");auto *all=new QPushButton("♫   Minha biblioteca");auto *fav=new QPushButton("♡   Favoritos");
    auto *albums=new QPushButton("▦   Álbuns");auto *artists=new QPushButton("♙   Artistas");
    for(auto*b:{home,all,fav,albums,artists})side->addWidget(b);
    side->addSpacing(14);side->addWidget(new QLabel("SUAS PLAYLISTS"));
    playlistList=new QListWidget;playlistList->setMaximumHeight(210);side->addWidget(playlistList);
    auto *newPl=new QPushButton("+ Nova playlist");side->addWidget(newPl);
    side->addStretch();auto *settingsBtn=new QPushButton("⚙   Configurações");side->addWidget(settingsBtn);
    auto *content=new QVBoxLayout;content->setSpacing(12);
    auto *header=new QHBoxLayout;auto *heading=new QLabel("Sua música, do seu jeito");heading->setStyleSheet("font-size:22pt;font-weight:700;");
    search=new QLineEdit;search->setPlaceholderText("⌕  Buscar músicas, artistas, álbuns...");search->setMaximumWidth(380);
    auto *importBtn=new QPushButton("+ Importar arquivos");auto *folderBtn=new QPushButton("Importar pasta");
    header->addWidget(heading);header->addStretch();header->addWidget(search);header->addWidget(importBtn);header->addWidget(folderBtn);content->addLayout(header);
    auto *welcome=new QFrame;welcome->setStyleSheet("QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #34265f,stop:1 #172d4b);border-radius:18px;}");
    auto *welcomeL=new QHBoxLayout(welcome);auto *welcomeText=new QVBoxLayout;auto *wtitle=new QLabel("Bem-vindo ao Nexus");wtitle->setStyleSheet("font-size:21pt;font-weight:700;background:transparent;");
    auto *wsub=new QLabel("Sua biblioteca local. Sem anúncios, sem dependência de streaming.");wsub->setStyleSheet("color:#d2d5ef;background:transparent;");
    welcomeText->addWidget(wtitle);welcomeText->addWidget(wsub);welcomeL->addLayout(welcomeText);welcomeL->addStretch();
    auto *quick=new QPushButton("Adicionar música");quick->setStyleSheet("background:#a78bfa;color:#171225;font-weight:700;padding:12px 18px;");welcomeL->addWidget(quick);content->addWidget(welcome);
    auto *listTitle=new QHBoxLayout;auto *lt=new QLabel("Todas as músicas");lt->setStyleSheet("font-size:15pt;font-weight:700;");listTitle->addWidget(lt);listTitle->addStretch();auto *removeBtn=new QPushButton("Remover da biblioteca");auto *addToPl=new QPushButton("Adicionar à playlist");listTitle->addWidget(addToPl);listTitle->addWidget(removeBtn);content->addLayout(listTitle);
    trackList=new QListWidget;trackList->setSelectionMode(QAbstractItemView::SingleSelection);content->addWidget(trackList,1);
    auto *mainCol=new QVBoxLayout;mainCol->addLayout(content,1);mainCol->addWidget(video);top->addWidget(sidebar);top->addLayout(mainCol,1);root->addLayout(top,1);
    auto *bar=new QFrame;bar->setStyleSheet("QFrame{background:#111522;border:1px solid #252b40;border-radius:15px;}");auto *barL=new QHBoxLayout(bar);barL->setContentsMargins(16,9,16,9);
    auto *meta=new QVBoxLayout;nowTitle=new QLabel("Nenhuma música");nowTitle->setStyleSheet("font-weight:700;");nowArtist=new QLabel("Adicione arquivos à biblioteca");nowArtist->setStyleSheet("color:#929bb4;font-size:9pt;");meta->addWidget(nowTitle);meta->addWidget(nowArtist);barL->addLayout(meta,1);
    auto *prev=new QPushButton("|◀");playButton=new QPushButton("▶");playButton->setStyleSheet("background:#8b6bea;color:white;font-size:14pt;font-weight:700;border-radius:20px;min-width:42px;min-height:42px;");auto *next=new QPushButton("▶|");auto *shuffle=new QPushButton("⤨");auto *repeat=new QPushButton("↻");
    barL->addWidget(shuffle);barL->addWidget(prev);barL->addWidget(playButton);barL->addWidget(next);barL->addWidget(repeat);
    progress=new QSlider(Qt::Horizontal);progress->setRange(0,0);progress->setMinimumWidth(180);timeLabel=new QLabel("0:00 / 0:00");timeLabel->setMinimumWidth(105);barL->addWidget(timeLabel);barL->addWidget(progress,2);
    auto *volLabel=new QLabel("VOL");volume=new QSlider(Qt::Horizontal);volume->setRange(0,100);volume->setValue(settings.volume());volume->setFixedWidth(105);barL->addWidget(volLabel);barL->addWidget(volume);
    auto *videoBtn=new QPushButton("▣ Vídeo");barL->addWidget(videoBtn);root->addWidget(bar);
    connect(importBtn,&QPushButton::clicked,this,[this]{addPaths(QFileDialog::getOpenFileNames(this,"Importar mídia",{},"Mídia (*.mp3 *.mp4 *.m4a *.wav *.flac *.aac *.ogg)"));});
    connect(quick,&QPushButton::clicked,importBtn,&QPushButton::click);
    connect(folderBtn,&QPushButton::clicked,this,[this]{QString p=QFileDialog::getExistingDirectory(this,"Importar pasta");if(!p.isEmpty())library->addFolder(p);});
    connect(search,&QLineEdit::textChanged,this,[this](const QString&s){showTracks(library->tracks(s));});
    connect(trackList,&QListWidget::itemDoubleClicked,this,[this]{playSelected();});
    trackList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(trackList,&QListWidget::customContextMenuRequested,this,[this](const QPoint &pos){
        auto *item=trackList->itemAt(pos); if(!item) return;
        const int row=trackList->row(item); if(row<0||row>=currentTracks.size()) return;
        const auto data=currentTracks.at(row).toMap(); const int id=data.value("id").toInt();
        QMenu menu(this);
        QAction *favAction=menu.addAction(data.value("favorite").toInt()?"Remover dos favoritos":"Adicionar aos favoritos");
        QAction *removeAction=menu.addAction("Remover da biblioteca");
        QAction *chosen=menu.exec(trackList->viewport()->mapToGlobal(pos));
        if(chosen==favAction){library->setFavorite(id,!data.value("favorite").toInt());refreshLibrary();}
        else if(chosen==removeAction) library->removeTrack(id);
    });
    connect(playButton,&QPushButton::clicked,player,&MusicPlayer::toggle);
    connect(prev,&QPushButton::clicked,player,&MusicPlayer::previous);connect(next,&QPushButton::clicked,player,&MusicPlayer::next);
    connect(shuffle,&QPushButton::clicked,this,[this,shuffle]{static bool on=false;on=!on;player->setShuffle(on);shuffle->setChecked(on);});
    connect(repeat,&QPushButton::clicked,this,[this,repeat]{static int mode=0;mode=(mode+1)%3;player->setRepeatMode(mode);repeat->setText(mode==0?"↻":mode==1?"↻1":"↻∞");});
    connect(volume,&QSlider::valueChanged,this,[this](int v){player->setVolume(v);settings.setVolume(v);});
    connect(progress,&QSlider::sliderPressed,this,[this]{seeking=true;});connect(progress,&QSlider::sliderReleased,this,[this]{seeking=false;player->seek(progress->value());});
    connect(videoBtn,&QPushButton::clicked,this,[this]{if(!player->currentPath().endsWith(".mp4",Qt::CaseInsensitive)){QMessageBox::information(this,"Vídeo","Selecione um arquivo MP4 para exibir o vídeo.");return;}video->setVideoVisible(!video->videoVisible());});
    connect(removeBtn,&QPushButton::clicked,this,[this]{auto *it=trackList->currentItem();if(!it)return;library->removeTrack(it->data(Qt::UserRole).toInt());});
    connect(addToPl,&QPushButton::clicked,this,[this]{auto *it=trackList->currentItem();auto *pl=playlistList->currentItem();if(!it||!pl){QMessageBox::information(this,"Playlist","Selecione uma música e uma playlist.");return;}playlists->addTrack(pl->data(Qt::UserRole).toInt(),it->data(Qt::UserRole).toInt());statusBar()->showMessage("Música adicionada à playlist.",2500);});
    connect(newPl,&QPushButton::clicked,this,[this]{bool ok=false;QString n=QInputDialog::getText(this,"Nova playlist","Nome:",QLineEdit::Normal,{},&ok);if(ok)playlists->create(n);});
    connect(playlistList,&QListWidget::itemDoubleClicked,this,[this](QListWidgetItem *it){showTracks(playlists->tracks(it->data(Qt::UserRole).toInt()));});
    connect(fav,&QPushButton::clicked,this,[this]{showTracks(library->favorites());});
    connect(all,&QPushButton::clicked,this,&MainWindow::refreshLibrary);
    connect(home,&QPushButton::clicked,this,&MainWindow::refreshLibrary);
    connect(albums,&QPushButton::clicked,this,[this]{QMessageBox::information(this,"Álbuns","A organização por álbum será baseada nas tags de mídia quando a extração de metadados estiver habilitada.");});
    connect(artists,&QPushButton::clicked,this,[this]{QMessageBox::information(this,"Artistas","A organização por artista será baseada nas tags de mídia quando a extração de metadados estiver habilitada.");});
    connect(settingsBtn,&QPushButton::clicked,this,[this]{
        QDialog dlg(this); dlg.setWindowTitle("Configurações do Nexus"); dlg.setMinimumWidth(430);
        auto *layout=new QVBoxLayout(&dlg); layout->addWidget(new QLabel("Pasta padrão da biblioteca"));
        auto *folderRow=new QHBoxLayout; auto *folderEdit=new QLineEdit(settings.musicFolder()); auto *browse=new QPushButton("Escolher..."); folderRow->addWidget(folderEdit,1);folderRow->addWidget(browse);layout->addLayout(folderRow);
        auto *autoPlay=new QCheckBox("Reprodução contínua / próxima faixa");autoPlay->setChecked(settings.autoplay());layout->addWidget(autoPlay);
        auto *shortcutInfo=new QLabel("Atalhos: Espaço = Play/Pause · Ctrl+O = Importar · Ctrl+F = Pesquisar");shortcutInfo->setStyleSheet("color:#9da5bd;");layout->addWidget(shortcutInfo);
        auto *buttons=new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);layout->addWidget(buttons);
        connect(browse,&QPushButton::clicked,&dlg,[&]{QString p=QFileDialog::getExistingDirectory(&dlg,"Pasta padrão",folderEdit->text());if(!p.isEmpty())folderEdit->setText(p);});
        connect(buttons,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);connect(buttons,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);
        if(dlg.exec()==QDialog::Accepted){settings.setMusicFolder(folderEdit->text());settings.setAutoplay(autoPlay->isChecked());}
    });
}
void MainWindow::refreshLibrary(){showTracks(library->tracks(search?search->text():QString()));}
void MainWindow::refreshPlaylists(){playlistList->clear();for(const auto &v:playlists->all()){auto m=v.toMap();auto *it=new QListWidgetItem(m["name"].toString());it->setData(Qt::UserRole,m["id"]);playlistList->addItem(it);}}
void MainWindow::showTracks(const QVariantList &tracks){
    currentTracks=tracks;trackList->clear();
    for(const auto &v:tracks){auto m=v.toMap();QString title=m["title"].toString();QString artist=m["artist"].toString();QString path=m["path"].toString();auto *it=new QListWidgetItem((title.isEmpty()?QFileInfo(path).completeBaseName():title)+"    •    "+(artist.isEmpty()?"Arquivo local":artist));it->setData(Qt::UserRole,m["id"]);it->setToolTip(path);trackList->addItem(it);}
}
QStringList MainWindow::visiblePaths()const{QStringList p;for(const auto&v:currentTracks)p<<v.toMap()["path"].toString();return p;}
void MainWindow::playSelected(){int row=trackList->currentRow();if(row<0||row>=currentTracks.size())return;player->setQueue(visiblePaths(),row);}
void MainWindow::addPaths(const QStringList &p){if(p.isEmpty())return;library->addFiles(p);}
QString MainWindow::formatTime(qint64 ms)const{int s=int(ms/1000);return QString("%1:%2").arg(s/60).arg(s%60,2,10,QChar('0'));}
void MainWindow::dragEnterEvent(QDragEnterEvent *e){if(e->mimeData()->hasUrls())e->acceptProposedAction();}
void MainWindow::dropEvent(QDropEvent *e){QStringList p;for(const auto&u:e->mimeData()->urls())if(u.isLocalFile())p<<u.toLocalFile();addPaths(p);e->acceptProposedAction();}
