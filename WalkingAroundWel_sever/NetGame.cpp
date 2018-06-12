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
    _socket = nullptr;
    _server = new QTcpServer(this);
    _server->listen(QHostAddress::Any, 9999);
    connect(_server, SIGNAL(newConnection()),
            this, SLOT(slotNewConnection()));
}

void NetGame::slotRecv()
{
    QByteArray ba = _socket->readAll();

    if (3 == ba[0]) {
        QPoint pt(ba[1], ba[2]);
        Board::MoveChess(pt);
    }
}

void NetGame::slotNewConnection() //服务器建立连接
{
    if (_socket) return; //只接受一个连接
    _socket = _server->nextPendingConnection();
    connect(_socket, SIGNAL(readyRead()),
            this, SLOT(slotRecv()));

    //发送
    char buf[7];
    buf[0] = 1;  //校验位. 再5位是棋子位置状态. 最后一位该谁走
    for (int i = 0; i < 5; ++i) buf[i+1] = id[i];
    buf[6] = black_turn_;
    _socket->write(buf, 7);
}

void NetGame::MoveChess(QPoint pt) {
    char buf[3] = {3, pt.x(), pt.y()};
    _socket->write(buf, 3);
}
/*
void NetGame::mouseReleaseEvent(QMouseEvent *event) {
    if (!black_turn_) return;
    //MoveChess(event->pos());
    Board::mouseReleaseEvent(event);
    char buf[3] = {3, event->pos().x(), event->pos().y()};
    _socket->write(buf, 3);
}*/
