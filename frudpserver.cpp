#include "frudpserver.h"
#include "ui_frudpserver.h"

FrUdpServer::FrUdpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrUdpServer)
{
    ui->setupUi(this);
}

FrUdpServer::~FrUdpServer()
{
    delete ui;
}

void FrUdpServer::_init()
{
    server = new QUdpSocket;

    connect(&timer,&QTimer::timeout,ui->btn_send,&QPushButton::click);
    connect(server,&QUdpSocket::readyRead,this,&FrUdpServer::readSlot);
}

void FrUdpServer::on_btn_listen_clicked()
{
    QString listenIp = ui->et_listen_ip->text();
    QString listenPort = ui->et_listen_port->text();
    if(!isListen){
        if(server->bind(QHostAddress(listenIp),listenPort.toInt())){
            ui->textEdit->append("监听成功");
            isListen = true;
            ui->btn_listen->setText("停止监听");
        }
        else {
            ui->textEdit->append("监听失败"+server->errorString());
            isListen = false;
            ui->btn_listen->setText("开始监听");
        }
    }
    else{
        ui->textEdit->append("停止监听成功");
        server->close();
        isListen = false;
        ui->btn_listen->setText("开始监听");
    }


}
void FrUdpServer::exportStateSlot(const int state, const QString filepath){
    switch (state) {
        case constant::EXPORT_STATE_SUCCESS:
            QMessageBox::information(this,"提示信息","成功导出到：" + filepath,QMessageBox::Ok);
            break;
        case constant::EXPORT_STATE_FAIL:
            QMessageBox::information(this,"提示信息","导出失败",QMessageBox::Ok);
            break;
        case constant::EXPORT_STATE_ERROR:
            QMessageBox::information(this,"提示信息","导出失败",QMessageBox::Ok);
            break;
        default:
            QMessageBox::information(this,"提示信息","导出失败",QMessageBox::Ok);
            break;
    }
}

void FrUdpServer::readSlot()
{
    qint64 len = server->pendingDatagramSize();
    QHostAddress ip;
    quint16 port;
    QByteArray temp;
    temp.resize(len);
    if(len > 0){
        server->readDatagram(temp.data(),len,&ip,&port);
    }
    else{
        return;
    }
    QString msg;
    if(isHexRecv){
        msg = QByteArray::fromHex(temp);
    }
    else {
        msg = QString::fromUtf8(temp);
    }
    ui->textEdit->append("[接收] [" + ip.toString() + ":" + QString::number(port) + "]:" + msg);
    bool hostExist = false;
    for(MyClient *mc : qAsConst(clientList)){
        if(mc->ip == ip && mc->port == port){
            hostExist = true;
            break;
        }
    }
    if(!hostExist){
        MyClient *mClient = new MyClient;
        mClient->ip = ip;
        mClient->port = port;
        clientList.append(mClient);
        qDebug()<<"mClient"<<ip<<port;
        ui->lw_client_list->addItem(ip.toString() + ":" + QString::number(port));
    }

}

void FrUdpServer::on_btn_import_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,"请选择文件","/projectTools","文本文档(*.txt);;");
    if(filepath.isEmpty()){
        return;
    }
    QFile f(filepath);
    if(!f.exists()){
        ui->textEdit->append("文件不存在");
        return;
    }
    if(!f.open(QIODevice::ReadOnly)){
        ui->textEdit->append("文件无法打开");
        return;
    }
    char buf[1024*7] = {0};
    int len = 0;
    QByteArray temp;
    while((len = f.read(buf,1024*7)) > 0){
        temp.append(buf,len);
    }
    ui->textEdit->append(QString::fromUtf8(temp));
}

void FrUdpServer::on_btn_export_clicked()
{
    QString content = ui->textEdit->toPlainText();
    if(content.isEmpty()){
        QMessageBox::information(this,"提示信息","导出内容为空",QMessageBox::Ok);
        return;
    }
    char *buf = content.toUtf8().data();
    emit exportSignal(buf,constant::EXPORT_TYPE_UDP_SERVER);
}


void FrUdpServer::on_cb_hex_send_stateChanged(int arg1)
{
    if(arg1 == 2)
        isHexSend = true;
    else
        isHexSend = false;
}


void FrUdpServer::on_cb_hex_receive_stateChanged(int arg1)
{
    if(arg1 == 2)
        isHexRecv = true;
    else
        isHexRecv = false;
}


void FrUdpServer::on_cb_timer_stateChanged(int arg1)
{
    if(arg1 == 2){
        isTimer = true;
        timer.start(ui->et_timer->text().toInt());
    }
    else{
        isTimer = false;
        timer.stop();
    }

}


void FrUdpServer::on_btn_send_clicked()
{
    QString msg = ui->et_send_text->text();
    if(clientList.isEmpty()){
        QMessageBox::information(this,"提示信息","暂无客户端连接",QMessageBox::Ok);
        return;
    }

    QByteArray buf;
    if(fileContent.isEmpty()){
        if(isHexSend){
            buf = msg.toLatin1().toHex(' ').toUpper();

        }
        else{
            buf = msg.toUtf8();
        }
    }
    else{
        msg = QString::fromUtf8(fileContent);
        if(isHexSend){
            buf = fileContent.toHex(' ').toUpper();
        }
        else{
            buf = fileContent;
        }
    }
    for(MyClient *clientItem : qAsConst(clientList)){
        ui->textEdit->append("[发送] [" + clientItem->ip.toString()
                             + ":" + QString::number(clientItem->port) + "]:" + msg);
        server->writeDatagram(buf,clientItem->ip,clientItem->port);
    }

}


void FrUdpServer::on_btn_read_file_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,"请选择文件","/projectTools","所有文件(*.*);;");
    if(filepath.isEmpty()){
        return;
    }
    fileContent.clear();
    ui->lab_read_file_path->setText(filepath);
    QFile f(filepath);
    if(!f.exists()){
        ui->textEdit->append("文件不存在");
        return;
    }
    f.open(QIODevice::ReadOnly);
    char buf[1024*7]  ={0} ;
    int len = 0;
    while((len = f.read(buf,1024*7)) > 0){
        fileContent.append(buf,len);
        qDebug()<<fileContent.isEmpty();
    }
}


void FrUdpServer::on_btn_clean_file_clicked()
{
    ui->lab_read_file_path->setText("");
    fileContent.clear();
}


void FrUdpServer::on_btn_clean_text_clicked()
{
    ui->textEdit->clear();
}

void FrUdpServer::on_textEdit_textChanged()
{
    ui->textEdit->moveCursor(QTextCursor::End);
}

