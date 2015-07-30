#include "mainwindow.h"
#include "playlist.h"
#include "ui_mainwindow.h"
#include <math.h>
#include <QtMultimedia>
#include <QDebug>
#include <QObject>
#include <QtNetwork>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QImage>
#include <QPixmap>
#include <QTextCodec>

void MainWindow::initGUI()
{
    song_name = ui->song_name;
    song_author = ui->song_author;
    song_album = ui->song_album;
    song_score = ui->song_score;
    preBtn = ui->preBtn;
    stopBtn = ui->stopBtn;
    nextBtn = ui->stopBtn;
    listBtn = ui->listBtn;
    modeBtn = ui->modeBtn;
    slider = ui->slider;
    img = ui->img;
    img->setPixmap(QPixmap(":/img/logo.png"));

    song_score->setText("..");

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Player - V1.0");
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

    //初始化 GUI 控件
    initGUI();

    //初始化绘图控件
    this->setMouseTracking(true);
    this->isPressed = false;

    //配置 MediaPlalyer
    volume = 100;

    buildPlayList();
    player=new QMediaPlayer;
    player->setVolume(volume);
    player->setPlaylist(playList);

    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(durationChanged(qint64)), this,SLOT(durationChanged(qint64)));
    connect(playList, SIGNAL(currentIndexChanged(int)), this,SLOT(updateSongList(int)));

    connect(playList, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSongList(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

// 绘制手势轨迹
void MainWindow::paintEvent(QPaintEvent*p){
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::darkCyan);
    pen.setWidth(5);
    painter.setPen(pen);

    for(int i = 0;i<lines.size();i++){
        myLine* pLine = lines[i];
        painter.drawLine(pLine->startPnt,pLine->endPnt);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e){
    startX = e->x();
    startY = e->y();
    setCursor(Qt::PointingHandCursor);
    startPnt = e->pos();
    endPnt = e->pos();
    this->isPressed = true;
}

void MainWindow::mouseMoveEvent(QMouseEvent *e){
    if(this->isPressed){
        endPnt = e->pos();

        myLine* line = new myLine;  			//put the new line into vector
        line->startPnt = startPnt;
        line->endPnt = endPnt;
        this->lines.push_back(line);
        update();                                    //repainter，call paintEvent
        startPnt = endPnt;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e){
    endX = e->x();
    endY = e->y();

    int deltaX = endX - startX;
    int deltaY = endY - startY;
    int threshold1 = 100;
    int threshold2 = 50;

    qDebug() << "deltaX: " << deltaX << " deltaY: " <<deltaY << " vol:" << player->volume();

    if( abs(deltaX) > threshold1 && abs(deltaY) < threshold2){
        if(deltaX > 0){
            // right
            nextBtnPressed();
        }else{
            // left
            preBtnPressed();
        }
    }else if( abs(deltaY) > threshold1 && abs(deltaX) < threshold2){
        if(deltaY > 0){
            // down
            volume -= 10;
            player->setVolume(volume);
        }else{
            // up
            volume += 10;
            player->setVolume(volume);
        }
    }

    setCursor(Qt::ArrowCursor);
    this->isPressed = false;
    this->lines.clear();
    update();
}

// 从本地文件读取播放
void MainWindow::buildPlayList(){
    QFile file(QDir::currentPath().append("/list.txt"));
    playList=new QMediaPlaylist;
    playList->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);
        str.replace("\n","");
        playList->addMedia(QUrl::fromLocalFile(str));
        qDebug() << str;
    }
    playList->setCurrentIndex(0);
    if(player != NULL){
        player->setPlaylist(playList);
    }
}

// 播放相关
void MainWindow::getReply(QNetworkReply *reply){
   QString all = reply->readAll();
   if(all == "") return;
   else song_score->setText(all);
}

void MainWindow::getScore(){
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(getReply(QNetworkReply *)));
    //QString title = player->metaData(QMediaMetaData::Title).toString();
    QString title = playList->currentMedia().canonicalUrl().fileName();
    QString author = player->metaData(QMediaMetaData::Author).toString();
    QString album = player->metaData(QMediaMetaData::AlbumTitle).toString();

    QString url = "http://127.0.0.1:8080/player/back.do?song=";
    url.append(title);
    //url.append("&");
    //url.append("author=");
    //url.append(author);
    manager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::preBtnPressed(){
    qDebug() << "pre";
    int currentIndex=playList->currentIndex();
    if(--currentIndex<0) currentIndex=0;
    playList->setCurrentIndex(currentIndex);
    player->play();
}

void MainWindow::stopBtnPressed(){
    qDebug() << "stop" << player->state();
    if(player->state() != 1){
        player->play();
        stopBtn->setText("Stop");
    }else{
        player->pause();
        stopBtn->setText("Play");
    }
}

void MainWindow::nextBtnPressed(){
    qDebug() << "next";
    int currentIndex=playList->currentIndex();
    if(++currentIndex==playList->mediaCount()) currentIndex=0;
    playList->setCurrentIndex(currentIndex);
    player->play();
}

void MainWindow::listBtnPressed(){
    qDebug() << "list";
    if(listWindow == NULL){
        listWindow = new PlayList();
    }
    listWindow->refresh();
    listWindow->show();
}

void MainWindow::modeBtnPressed(){
    // CurrentItemOnce, CurrentItemInLoop, Sequential, Loop, Random
    QString tmp;
    switch(playList->playbackMode()){
        case QMediaPlaylist::CurrentItemOnce:
            playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            tmp = "CIL";
            break;
        case QMediaPlaylist::CurrentItemInLoop:
            playList->setPlaybackMode(QMediaPlaylist::Sequential);
            tmp = "Seq";
            break;
        case QMediaPlaylist::Sequential:
            playList->setPlaybackMode(QMediaPlaylist::Loop);
            tmp = "Loop";
            break;
        case QMediaPlaylist::Loop:
            playList->setPlaybackMode(QMediaPlaylist::Random);
            tmp = "Ran";
            break;
        case QMediaPlaylist::Random:
            playList->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
            tmp = "CIO";
            break;
    }
    modeBtn->setText(tmp);
}

void MainWindow::on_slider_sliderMoved(int position)
{
    qDebug() << position;
    player->setPosition(position);
}

void MainWindow::updateSongList(int i){
    qDebug() << "i: " << i;
}

void MainWindow::durationChanged(qint64 duration){
    slider->setRange(0,duration);
    QUrl url = playList->currentMedia().canonicalUrl();
    QString file_name = url.fileName();
    QString t = player->metaData(QMediaMetaData::Title).toString();
    QString au = player->metaData(QMediaMetaData::Author).toString();
    QString al = player->metaData(QMediaMetaData::AlbumTitle).toString();

    song_name->setText(file_name);
    song_author->setText(au == "" ? "unknown" : au);
    song_album->setText(al == "" ? "unknown" : al);

    getScore();
    getMusicInfo(url.toString().mid(7));
}

void MainWindow::positionChanged(qint64 position){
    slider->setValue(position);
}

// 处理ID3
void MainWindow::getMusicInfo(QString file)
{
    proc = new QProcess();
    QString program = "mp3info";
    QStringList arguments ;
    arguments << file;
    connect(proc,SIGNAL(readyReadStandardOutput()),this,SLOT(invokeMp3Info()));
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start(program,arguments);
}

void MainWindow::invokeMp3Info(){
    QByteArray array;
    proc->readLine();
    array = proc->readLine();
    array.replace("\n","");
    QString title(array.mid(9,array.indexOf("\0",10)));
    array = proc->readLine();
    array.replace("\n","");
    QString author(array.mid(9,array.indexOf("\0",10)));
    array = proc->readLine();
    array.replace("\n","");
    QString album(array.mid(9,array.indexOf("\0",10)));
    qDebug() << title << author << album;
    song_author->setText(author);
    song_album->setText(album);
}


// handle onTop and mute operation
void MainWindow::onTopChecked(bool b){
    if(b){
        setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
    }else{
        setWindowFlags(windowFlags()&~Qt::WindowStaysOnTopHint);
    }
    show();
}
void MainWindow::muteChecked(bool b){
    if(b){
        player->setVolume(0);
    }else{
        player->setVolume(volume);
    }
}
