#ifndef NETGAME_H
#define NETGAME_H

#include "Board.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <QString>
#include <QLineEdit>

class NetGame : public Board
{
    Q_OBJECT
public:
    NetGame();
    //virtual void mouseReleaseEvent(QMouseEvent *event); //鼠标松开
    virtual void MoveChess(QPoint pt);
private:

    QTcpSocket * _socket; //客户端
    void IPSelect();
    QString ip_;
    QWidget *ip_widget;
    QLineEdit *ip_edit;

public slots:
    void slotRecv(); //接收数据
    void SlotIPSelect();
};


#endif // NETGAME_H
