#include "NetGame.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMouseEvent>

//1）执红方还是黑方，这个信息由服务器发出，客户端接收
//报文：第一个字节固定是1，第二个字节是1或0，1表示接收方走红棋，0表示走黑棋
//2）点击信息（点击坐标、棋子编号）
//报文：第一个字节固定是2，第二个字节是row,第三个字节是col，第四个字节是点击的棋子id

NetGame::NetGame()
{
    ip_ = "null";
    IPSelect();
/*    _socket = new QTcpSocket(this);
    _socket->connectToHost(QHostAddress("127.0.0.1"), 9999);
    connect(_socket, SIGNAL(readyRead()),
            this, SLOT(slotRecv())); */
}

void NetGame::slotRecv()
{
    QByteArray ba = _socket->readAll();
    if (1 == ba[0]) {
        for (int i = 1; i < 6; ++i) {
            switch(ba[i]) {
            case 0:
                id[i-1] = white;
                break;
            case 1:
                id[i-1] = black;
                break;
            case 2:
                id[i-1] = empty;
                break;
            default: break;
            }
        }
        black_turn_ = ba[6];
    }

    if (3 == ba[0]) {
        QPoint pt(ba[1], ba[2]);
        Board::MoveChess(pt);
    }
}

void NetGame::MoveChess(QPoint pt) {
    Board::MoveChess(pt);
    char buf[3] = {3, pt.x(), pt.y()};
    _socket->write(buf, 3);
}
/*
void NetGame::mouseReleaseEvent(QMouseEvent *event) {
    if (!black_turn_) return;
    Board::mouseReleaseEvent(event);
    //MoveChess(event->pos());
    char buf[3] = {3, event->pos().x(), event->pos().y()};
    _socket->write(buf, 3);
}
*/
void NetGame::IPSelect(){
    //输入IP地址
    ip_widget = new QWidget;
    ip_widget->setWindowTitle("IP地址");
    int d = 30;
    ip_widget->setBaseSize(QSize(10*d,10*d));
    QLabel *ip_label = new QLabel("请输入服务器IP地址：",ip_widget);
    ip_label->setGeometry(d,d,5*d,2*d);
    ip_edit = new QLineEdit(ip_widget);
    ip_edit->setGeometry(d,3*d,5*d,d);
    QPushButton *confirm_button = new QPushButton("确定", ip_widget);
    confirm_button->setGeometry(2*d, 5*d, 80, 30);
    ip_widget->show();
    connect(confirm_button, SIGNAL(clicked(bool)),this,SLOT(SlotIPSelect()));
}

void NetGame::SlotIPSelect() {
    ip_ = ip_edit->text();
    ip_widget->close();
    //qDebug() << ip_;
    if(ip_ != "null") {
        _socket = new QTcpSocket(this);
        _socket->connectToHost(QHostAddress(ip_), 9999);
        connect(_socket, SIGNAL(readyRead()),
                this, SLOT(slotRecv()));
    }
}




