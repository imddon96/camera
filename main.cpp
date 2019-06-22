#include "widget.h"
#include <QApplication>
#include "v4l2_example.h"
#include <iostream>

using namespace std;
int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    Widget w;
    QFont font  = w.font();
    font.setPointSize(64);
    w.setFont(font);
    w.show();

    return a.exec();
}
