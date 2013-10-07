#include "widget.h"
#include <QApplication>
#include <QPainter>
#include <QString>
#include "math.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
