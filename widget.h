#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "tcpserver.h"
#include "tcpclient.h"
#include "frudpclient.h"
#include "frudpserver.h"
#include "constant.h"
#include "exportutil.h"
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
    FrUdpServer *frUdpServer;
    FrUdpClient *frUdpClient;
    ExportUtil *exportUtilTcpClient;
    ExportUtil *exportUtilTcpServer;
    ExportUtil *exportUtilUdpClient;
    ExportUtil *exportUtilUdpServer;
};
#endif // WIDGET_H
