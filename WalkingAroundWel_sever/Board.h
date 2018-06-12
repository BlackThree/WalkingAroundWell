#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QPen>
#include <QMessageBox>
#include <QDir>
#include <QPushButton>

class Board : public QWidget {
    Q_OBJECT
public:
    explicit Board(QWidget *parent = 0);
    //画棋盘
    void paintEvent(QPaintEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *event); //鼠标松开
    int ClickChess(const QPoint & pt) const; //点击的棋子
    void ChessRule(const int &a, const int &b); //走棋规则
    void JudgeWin(); //输赢判断
    void FightMode();  //对战模式,返回0人人，1人机
    void MoveChess(QPoint pt);

    static int fight_mode_id;
    enum TYPE { white, black, empty }; //棋子类型
    TYPE id[5] = {white, white, empty, black, black};  //棋子位置信息
    bool black_turn_; //轮流走棋

signals:

public slots:
    void OpenMusic();   //背景音乐
    void MachineGo();  //机器下棋

private:
    QPen pen_red_;
    QPen pen_white_;
    QPen pen_black_;
    QPoint coord[5]; //棋盘上5个点坐标
    const int kd = 300; //棋盘棋点间距
    int click_id_; //被选中棋子的坐标编号
    int click_count_; //统计点击的次数，为了记录上次点击的坐标编号
    int click[100]; //保存点击的坐标编号
    QMessageBox message;
    bool mark; //不可一步走死
    QDir audio_dir_; //音频文件路径
    void Delay_MSec(unsigned int msec);
    void MachineSelectPiece();
};

#endif // BOARD_H
