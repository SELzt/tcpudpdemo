#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "tcpserver.h"
#include "tcpclient.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void init();

private:
    Ui::Widget *ui;
    TcpServer *server;
    TcpClient *client;
};
#endif // WIDGET_H
