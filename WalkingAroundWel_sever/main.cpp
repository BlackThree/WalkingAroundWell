#include <QApplication>
#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>

#include "NetGame.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);


    NetGame board;
    board.show();
   // Board board;
    //board.show();
    board.FightMode();
    app.exec();
    return 0;
}
