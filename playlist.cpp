#include "playlist.h"
#include "ui_playlist.h"
#include "mainwindow.h"
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QTableView>
#include <QDebug>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QString>
#include <QProcess>
#include <QFileDialog>
#include <QStringList>
#include <QMediaContent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTreeWidgetItem>

PlayList::PlayList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayList)
{
    ui->setupUi(this);
    setWindowTitle("Playlist");
    //setWindowFlags(Qt::WindowStaysOnTopHint);


    tw = ui->tableWidget;
    tw->setColumnCount(1);
    tw->horizontalHeader()->setStretchLastSection(1);
    tw->setHorizontalHeaderLabels(QStringList() << tr("name")); //<<tr("author")<<tr("score"));
    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(tw,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(selectItem(int,int)));

    refresh();
}

PlayList::~PlayList()
{
    delete ui;
}

void PlayList::refresh(){
    playList = MainWindow::getInstance()->playList;
    player = MainWindow::getInstance()->player;
    tw->setRowCount(playList->mediaCount());
    qDebug() << playList->mediaCount();
    for(int i=0;i<playList->mediaCount();i++){
        QString name = playList->media(i).canonicalUrl().fileName();
        tw->setItem(i,0,new QTableWidgetItem(name));
    }
}

// 处理双击播放操作
void PlayList::selectItem(int row, int col){
    qDebug() << row << "..." << col;
    int index = row;
    playList->setCurrentIndex(index);
    player->play();
}

void PlayList::getReply(QNetworkReply *reply){

    qDebug() << "reply";
}

void PlayList::on_addBtn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this,tr("title"),"/home",tr("Music:(*.*)"));
    for(int i=0;i < files.size() ; i++){
        playList->addMedia(QUrl::fromLocalFile(files[i]));
        QFile file(QDir::currentPath().append("/list.txt"));
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;
        QTextStream out(&file);
        out << files[i] << "\n";
    }
    MainWindow::getInstance()->buildPlayList();
    refresh();
}

void PlayList::on_clearBtn_clicked()
{
    player = MainWindow::getInstance()->player;
    player->setPlaylist(NULL);
    MainWindow::getInstance()->song_name->setText("...");
    MainWindow::getInstance()->song_author->setText("...");
    MainWindow::getInstance()->song_album->setText("...");
    player->stop();
    QFile file(QDir::currentPath().append("/list.txt"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << "";

    MainWindow::getInstance()->buildPlayList();
    refresh();
}


