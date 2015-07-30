#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QLinkedList>

class QTableWidget;
class QMediaPlaylist;
class QMediaPlayer;
class QNetworkReply;
class QTreeWidgetItem;
class QProcess;

namespace Ui {
class PlayList;
}

class PlayList : public QWidget
{
    Q_OBJECT

public:
    explicit PlayList(QWidget *parent = 0);
    ~PlayList();

    void initGUI();
    void refresh();

private slots:
    void on_addBtn_clicked();
    void on_clearBtn_clicked();
    void selectedFiles(QStringList*);
    void getReply(QNetworkReply *);
    void selectItem(int,int);

private:
    Ui::PlayList *ui;
    QTableWidget *tw;

    QMediaPlaylist *playList;
    QMediaPlayer *player;

};

#endif // PLAYLIST_H
