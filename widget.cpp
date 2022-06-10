#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}
void Widget :: init(){
    client = new TcpClient;
    client->init();
    server = new TcpServer;
    server->init();

    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(0);
    ui->tabWidget->addTab(server,"TCP Server");
    ui->tabWidget->addTab(client,"TCP Client");

}

