#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPoint>
#include <QLinkedList>

using namespace std;

class QMediaPlayer;
class QMediaPlaylist;
class QSystemTrayIcon;

class QNetworkAccessManager;
class QNetworkReply;

class QLabel;
class QPushButton;
class QSlider;

class PlayList;
class QProcess;

typedef struct myLine{
    QPoint startPnt;
    QPoint endPnt;

}myLine;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
//    static MainWindow *instance;

public:

    static MainWindow * getInstance(){
        static MainWindow instance;   //局部静态变量
        return &instance;
    }

    QMediaPlayer *player;
    QMediaPlaylist *playList;
    PlayList *listWindow;

    void initGUI();
    void getScore();
    void buildPlayList();
    void getMusicInfo(QString);

    // GUI
    QLabel *song_name;
    QLabel *song_author;
    QLabel *song_album;
    QLabel *song_score;

    QPushButton *preBtn;
    QPushButton *stopBtn;
    QPushButton *nextBtn;
    QPushButton *listBtn;
    QPushButton *modeBtn;

    QSlider *slider;
    QLabel *img;


private:
    int startX;
    int startY;
    int endX;
    int endY;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    int volume;

    Ui::MainWindow *ui;

    // 绘图
    void paintEvent(QPaintEvent* p);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    QPoint startPnt;   //起点
    QPoint endPnt;     //终点
    bool isPressed;    //鼠标是否按下

    vector<myLine*> lines; //存放所有的线段
    QProcess *proc;

private slots:
    void getReply(QNetworkReply *);
    void preBtnPressed();
    void stopBtnPressed();
    void nextBtnPressed();
    void listBtnPressed();
    void modeBtnPressed();
    void on_slider_sliderMoved(int position);

    void updateSongList(int);
    void durationChanged(qint64);
    void positionChanged(qint64);

    void onTopChecked(bool);
    void muteChecked(bool);

    void invokeMp3Info();
};

#endif // MAINWINDOW_H
