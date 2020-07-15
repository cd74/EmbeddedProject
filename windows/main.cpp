#include "qt1.h"
#include "start.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Qt1 w;
    start w;
    w.setWindowTitle("Start");
    w.show();

    return a.exec();
}
