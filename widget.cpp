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

    frUdpServer = new FrUdpServer;
    frUdpServer->_init();

    frUdpClient = new FrUdpClient;
    frUdpClient->_init();

    exportUtilTcpClient = new ExportUtil;
    exportUtilTcpServer = new ExportUtil;
    exportUtilUdpClient = new ExportUtil;
    exportUtilUdpServer = new ExportUtil;

    QDir dir;
    dir.mkdir(constant::EXPORT_TCPCLIENT_MESSAGE);
    dir.mkdir(constant::EXPORT_TCPSERVER_MESSAGE);
    dir.mkdir(constant::EXPORT_UDPCLIENT_MESSAGE);
    dir.mkdir(constant::EXPORT_UDPSERVER_MESSAGE);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(0);
    ui->tabWidget->addTab(server,"TCP Server");
    ui->tabWidget->addTab(client,"TCP Client");
    ui->tabWidget->addTab(frUdpServer,"UDP Server");
    ui->tabWidget->addTab(frUdpClient,"UdpClient");

    connect(client,&TcpClient::exportSignal,exportUtilTcpClient,&ExportUtil::exports);
    connect(exportUtilTcpClient,&ExportUtil::exportState,client,&TcpClient::exportStateSlot);
    connect(server,&TcpServer::exportSignal,exportUtilTcpServer,&ExportUtil::exports);
    connect(exportUtilTcpServer,&ExportUtil::exportState,server,&TcpServer::exportStateSlot);

    connect(frUdpClient,&FrUdpClient::exportSignal,exportUtilUdpClient,&ExportUtil::exports);
    connect(exportUtilUdpClient,&ExportUtil::exportState,frUdpClient,&FrUdpClient::exportStateSlot);
    connect(frUdpServer,&FrUdpServer::exportSignal,exportUtilUdpServer,&ExportUtil::exports);
    connect(exportUtilUdpServer,&ExportUtil::exportState,frUdpServer,&FrUdpServer::exportStateSlot);
}

