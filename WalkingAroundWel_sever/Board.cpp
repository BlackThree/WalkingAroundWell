#include "Board.h"
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QCheckBox>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QTime>
#include <QCoreApplication>
#include <QTimer>
#include <time.h>

int Board::fight_mode_id = -1;

Board::Board(QWidget *parent) : QWidget(parent) {
    this->resize(2*kd, 2*kd);
    click_count_ = 0;
    for (int i = 0; i < 10; i++) {
        click[i] = -1;
    }
    black_turn_ = true; //黑棋先走
    mark = false;
    //设置画笔颜色
    QColor colorRed(Qt::red);
    pen_red_.setColor(colorRed);
    pen_red_.setWidth(3);
    QColor colorWhite(Qt::white);
    pen_white_.setColor(colorWhite);
    pen_white_.setWidth(3);
    QColor colorBlack(Qt::black);
    pen_black_.setColor(colorBlack);
    pen_black_.setWidth(3);

    //将坐标与棋盘上位置对应
    coord[0] = QPoint(kd/3, kd/3);
    coord[1] = QPoint(kd/3 + kd, kd/3);
    coord[2] = QPoint(kd/3 + kd/2, kd/3 + kd/2);
    coord[3] = QPoint(kd/3, kd/3 + kd);
    coord[4] = QPoint(kd/3 + kd, kd/3 + kd);

    //背景音乐
    QCheckBox *check_music = new QCheckBox("背景音乐", this);
    check_music->setGeometry(kd/3 + kd + kd/3, kd/3 + kd/2, 100, 30); //位置和大小
    connect(check_music, SIGNAL(clicked()),
            this, SLOT(OpenMusic()));

    //机器走子.间隔1秒执行一次
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(MachineGo()));
}

void Board::FightMode() {
    QWidget *fight_mode_widget = new QWidget;
    fight_mode_widget->setWindowTitle("对战模式选择");

    QRadioButton *man_man = new QRadioButton("人人对战", fight_mode_widget);
    man_man->setGeometry(kd/6, kd/6, 100, 30);
    QRadioButton *man_machine = new QRadioButton("人机对战", fight_mode_widget);
    man_machine->setGeometry(kd/6, kd/3, 100, 30);
    QButtonGroup *group = new QButtonGroup(fight_mode_widget);
    group->addButton(man_man);
    group->addButton(man_machine);
    group->setId(man_man, 0);
    group->setId(man_machine, 1);

    QPushButton *confirm_button = new QPushButton("确定", fight_mode_widget);
    confirm_button->setGeometry(kd/6, kd/2, 80, 30);
    confirm_button->setObjectName("确定");

    connect(confirm_button, &QPushButton::clicked,
            [=](){if (group->checkedId() != -1) {
                     fight_mode_widget->close();
                     fight_mode_id = group->checkedId();
                  }});
    fight_mode_widget->show();
}

//画棋盘
void Board::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawText(3*kd/6, kd/6, "黑棋先走,服务器执黑棋,客户端执白棋");
    //消锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen_black_); //默认画笔格式
    //画横线
    painter.drawLine(QPoint(kd/3, kd/3), QPoint(kd/3 + kd, kd/3));
    painter.drawLine(QPoint(kd/3, kd/3 + kd), QPoint(kd/3 + kd, kd/3 + kd));
    //画竖线
    //painter.setPen(Qt::red); //改变线为红色
    painter.setPen(pen_red_);
    painter.drawLine(QPoint(kd/3, kd/3), QPoint(kd/3, kd/3 + kd));
    painter.drawEllipse(QPoint(kd/3 + 17, kd/3 + kd/2), 10, 10); //画井
    //painter.setPen(Qt::black); //重置线为黑色
    painter.setPen(pen_black_);
    painter.drawLine(QPoint(kd/3 + kd, kd/3), QPoint(kd/3 + kd, kd/3 + kd));
    //画交叉线
    painter.drawLine(QPoint(kd/3, kd/3), QPoint(kd/3 + kd, kd/3 + kd));
    painter.drawLine(QPoint(kd/3 + kd, kd/3), QPoint(kd/3, kd/3 + kd));

    //画棋子
    for (int i = 0; i < 5; i++) {
        if (id[i] == white) {
            if (click_id_ == i) {  //点中该棋子
                painter.setBrush(Qt::yellow);
            }
            else {
                painter.setBrush(Qt::white);
            }
            painter.setPen(pen_white_);
            painter.drawEllipse(coord[i], 20, 20);
        }
        else if (id[i] == black) {
            if (click_id_ == i) {  //点中该棋子
                painter.setBrush(Qt::yellow);
            }
            else {
                painter.setBrush(Qt::black);
            }
            painter.setPen(pen_black_);
            painter.drawEllipse(coord[i], 20, 20);
        }
        update();
    }
}

void Board::mouseReleaseEvent(QMouseEvent *event) {  //鼠标松开
    MoveChess(event->pos());
}

void Board::MoveChess(QPoint pt) {
    click_id_ = ClickChess(pt);

    if (black_turn_) { //黑棋走
        //走棋
        click_count_++;
        click[click_count_] = click_id_;
        if (click[click_count_ - 1] != -1 && click[click_count_] != -1) {  //需要点到坐标位置上
            if (id[click[click_count_ - 1]] == black && id[click[click_count_]] == empty) {  //该处有空位才可以走棋,2是空位
                ChessRule(click[click_count_ - 1], click[click_count_]);

                //输赢判断
                JudgeWin();

                //重置参数
                black_turn_ = false;
                click_count_ = 0;
                for (int i = 0; i < 10; i++) {
                    click[i] = -1;
                }
            }
        }
    }
    else {
        if(0 == fight_mode_id) {
            //走棋
            click_count_++;
            click[click_count_] = click_id_;
            if (click[click_count_ - 1] != -1 && click[click_count_] != -1) { //需要点到坐标位置上
                if (id[click[click_count_ - 1]] == white && id[click[click_count_]] == empty) { //该处有空位才可以走棋,2是空位
                    ChessRule(click[click_count_ - 1], click[click_count_]);

                    //输赢判断
                    JudgeWin();

                    //重置参数
                    black_turn_ = true;
                    click_count_ = 0;
                    for (int i = 0; i < 10; i++) {
                        click[i] = -1;
                    }
                }
            }
        }
    }
}

int Board::ClickChess(const QPoint &pt) const {  //点击的棋子
    if (pt.x() < kd/3 + 20 &&
        pt.x() > kd/3 - 20 &&
        pt.y() < kd/3 + 20 &&
        pt.y() > kd/3 - 20) {
        return 0;
    }
    else if (pt.x() < kd/3 + kd + 20 &&
             pt.x() > kd/3 + kd - 20 &&
             pt.y() < kd/3 + 20 &&
             pt.y() > kd/3 - 20) {
        return 1;
    }
    else if (pt.x() < kd/3 + kd/2 + 20 &&
             pt.x() > kd/3 + kd/2 - 20 &&
             pt.y() < kd/3 + kd/2 + 20 &&
             pt.y() > kd/3 + kd/2 - 20) {
        return 2; //空位
    }
    else if (pt.x() < kd/3 + 20 &&
             pt.x() > kd/3 - 20 &&
             pt.y() < kd/3 + kd + 20 &&
             pt.y() > kd/3 + kd - 20) {
        return 3;
    }
    else if (pt.x() < kd/3 + kd + 20 &&
             pt.x() > kd/3 + kd - 20 &&
             pt.y() < kd/3 + kd + 20 &&
             pt.y() > kd/3 + kd - 20) {
        return 4;
    }
    else {  //其他地方
        return -1;
    }
}
void Board::ChessRule(const int &a, const int &b) {  //走棋规则
    if (a == 0) {  //0处棋
        if (b == 1 || b == 2) {
            id[b] = id[a];
            id[a] = empty;
        }
    }
    else if (a == 1) {  //1处棋
        if (b != 3) {
            id[b] = id[a];
            id[a] = empty;
        }
    }
    else if (a == 2) {
        id[b] = id[a];
        id[a] = empty;
    }
    else if(a == 3) {
        if(b == 2 || b == 4) {
            id[b] = id[a];
            id[a] = empty;
        }
    }
    else {
        if(b != 0) {
            id[b] = id[a];
            id[a] = empty;
        }
    }
}

void Board::JudgeWin() {  //输赢判断
    if ((id[0] == white &&
         id[1] == white &&
         id[2] == black &&
         id[4] == black) ||
        (id[3] == white &&
         id[4] == white &&
         id[2] == black &&
         id[1] == black)) {
        if (mark) {
            message.information(nullptr, "游戏结束", "   黑棋胜！  ");
        }
        else {
            message.information(nullptr, "游戏中止", "   不可一步走死，白棋胜！  ");
        }
    }
    else if ((id[2] == white &&
              id[4] == white &&
              id[0] == black &&
              id[1] == black) ||
             (id[1] == white &&
              id[2] == white &&
              id[3] == black &&
              id[4] == black)) {
        if (mark) {
            message.information(nullptr, "游戏结束", "   白棋胜！  ");
        }
        else {
            message.information(nullptr, "游戏中止", "   不可一步走死，黑棋胜！  ");
        }
    }
    mark = true;
}

//背景音乐
void Board::OpenMusic() {
    QString path = audio_dir_.currentPath(); //获取程序路径
    path.replace('\\', '/'); /* 用 / 替换 \ */
    path += "/孙子涵_唐人.mp3"; //添加音频文件
    QMediaPlaylist * playlist = new QMediaPlaylist;
    playlist->addMedia(QUrl::fromLocalFile(path));
    playlist->setCurrentIndex(1); //激活播放列表
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop); //单曲循环

    static QMediaPlayer * player = new QMediaPlayer; //使用static后才能停止音乐播放
    player->setPlaylist(playlist);
    player->setVolume(30);
    //if(checked) //播放音乐
   // {
        player->play();
   // }
   // else //停止播放音乐
   // {
   //     player->stop();
   // }
}

void Board::MachineGo() {
    //TODO:白棋选错了没法走
    if((!black_turn_) && 1 == fight_mode_id) {
        do {  //判断是否需要重新选棋
            MachineSelectPiece();
            Delay_MSec(1000);
        }while ((0 == click[click_count_ - 1] && 4 == click[click_count_]) ||
                (4 == click[click_count_ - 1] && 0 == click[click_count_]) ||
                (3 == click[click_count_ - 1] && 0 == click[click_count_]) ||
                (0 == click[click_count_ - 1] && 3 == click[click_count_]) ||
                (3 == click[click_count_ - 1] && 1 == click[click_count_]) ||
                (1 == click[click_count_ - 1] && 3 == click[click_count_]));

        //if (id[click[click_count_ - 1]] == white && id[click[click_count_]] == empty) { //该处有空位才可以走棋,2是空位
            ChessRule(click[click_count_ - 1], click[click_count_]);
            //输赢判断
            JudgeWin();
            //重置参数
            black_turn_ = true;
            click_count_ = 0;
            for (int i = 0; i < 10; i++) {
                click[i] = -1;
            }
       // }
    }
}

void Board::Delay_MSec(unsigned int msec) {
    QTime _Timer = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < _Timer ) QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Board::MachineSelectPiece(){
    click_count_ = 0;
    for (int i = 0; i < 10; i++) {
        click[i] = -1;
    }
    //选中白棋,重复选择，直至选中一颗白棋
    do {
        //QTime t(QTime::currentTime());
        //qsrand(t.msec() + t.second() * 1000);
        qsrand(time(nullptr));
        click_id_ = qrand() % 4;
    } while (white != id[click_id_]);
    ++click_count_;
    click[click_count_] = click_id_;
    //寻找空位，并走棋
    for(int i = 0; i < 5; i++) {
        if (empty == id[i]) {
            ++click_count_;
            click[click_count_] = i;
            break;
        }
    }
}
