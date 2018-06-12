#ifndef NETGAME_H
#define NETGAME_H

#include "Board.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QLineEdit>

class NetGame : public Board
{
    Q_OBJECT
public:
    NetGame();
   // virtual void mouseReleaseEvent(QMouseEvent *event); //鼠标松开
    virtual void MoveChess(QPoint pt);
private:
    QTcpServer * _server; //服务器
    QTcpSocket *_socket;

public slots:
    void slotNewConnection(); //服务器建立连接
    void slotRecv(); //接收数据
};


#endif // NETGAME_H
