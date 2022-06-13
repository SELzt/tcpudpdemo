#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QTextCodec* codec = QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForLocale(codec);
    w.init();
    w.show();
    return a.exec();
}
