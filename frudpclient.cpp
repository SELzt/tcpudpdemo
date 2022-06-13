#include "frudpclient.h"
#include "ui_frudpclient.h"

FrUdpClient::FrUdpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrUdpClient)
{
    ui->setupUi(this);
}

FrUdpClient::~FrUdpClient()
{
    delete ui;
}

void FrUdpClient::_init()
{
    client = new QUdpSocket;
    connect(&timer,&QTimer::timeout,ui->btn_send,&QPushButton::click);
    connect(client,&QUdpSocket::readyRead,this,&FrUdpClient::readSlot);
}

void FrUdpClient::on_btn_export_clicked()
{
    QString content = ui->textEdit->toPlainText();
    if(content.isEmpty()){
        QMessageBox::information(this,"提示信息","导出内容为空",QMessageBox::Ok);
        return;
    }
    char *buf = content.toUtf8().data();
    emit exportSignal(buf,constant::EXPORT_TYPE_UDP_CLIENT);
}

void FrUdpClient::on_btn_import_clicked()
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

void FrUdpClient::exportStateSlot(const int state, const QString filepath)
{
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

void FrUdpClient::readSlot()
{
    qDebug()<<"udpClientRead";
    qint64 len = client->pendingDatagramSize();
    QHostAddress ip;
    quint16 port;
    QByteArray temp;
    temp.resize(len);
    if(len > 0){
        client->readDatagram(temp.data(),len,&ip,&port);
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
}

void FrUdpClient::on_cb_hex_send_stateChanged(int arg1)
{
    if(arg1 == 2)
        isHexSend = true;
    else
        isHexSend = false;
}


void FrUdpClient::on_cb_hex_receive_stateChanged(int arg1)
{
    if(arg1 == 2)
        isHexRecv = true;
    else
        isHexRecv = false;
}


void FrUdpClient::on_cb_timer_stateChanged(int arg1)
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


void FrUdpClient::on_btn_send_clicked()
{

    if(ip.isEmpty() || port.isEmpty()){
        ip = ui->et_bind_ip->text();
        port = ui->et_bind_port->text();

        if(ip.isEmpty() || port.isEmpty()){
            QMessageBox::information(this,"提示信息","绑定ip或绑定端口为空",QMessageBox::Ok);
            return;
        }
        //udp首次绑定
        if(!client->bind(QHostAddress(ip),port.toInt())){
            ui->textEdit->append("绑定" + ip + ":" + port + "失败");
            return;
        }



    }
    else{
        //判断ip port是否更换，更换了则关闭socket绑定另一个端口进行收发
        QString tempIp = ui->et_bind_ip->text();
        QString tempPort = ui->et_bind_port->text();
        if(tempIp.isEmpty() || tempPort.isEmpty()){
             QMessageBox::information(this,"提示信息","更换绑定的ip或绑定端口为空",QMessageBox::Ok);
             return;
        }
        else if(tempIp != ip || tempPort != port){
            ip = tempIp;
            port = tempPort;
            client->close();
            if(!client->bind(QHostAddress(ip),port.toInt())){
                ui->textEdit->append("绑定失败:" + tempIp + ":" + tempPort);
            }
        }

    }
    QString destIp = ui->et_server_ip->text();
    QString destPort = ui->et_server_port->text();
    if(destIp.isEmpty() || destPort.isEmpty()){
        QMessageBox::information(this,"提示信息","服务器ip或端口为空",QMessageBox::Ok);
    }
    QString msg = ui->et_send_text->text();
    QByteArray buf;
    if(fileContent.isEmpty()){
        //发送普通数据
        if(isHexSend){
            buf = msg.toLatin1().toHex(' ').toUpper();
        }
        else {
            buf = msg.toUtf8();
        }
    }
    else {
        //发送文件数据
        msg = QString::fromUtf8(fileContent);
        if(isHexSend){
            buf = fileContent.toHex(' ').toUpper();
        }
        else {
            buf = fileContent;
        }
    }
    client->writeDatagram(buf,QHostAddress(destIp),destPort.toInt());
    ui->textEdit->append("[发送] [" + destIp + ":" + destPort + "]:" + msg);
}


void FrUdpClient::on_btn_read_file_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,"请选择文件","/projectTools","所有文件(*.*)");
    if(filepath.isEmpty()){
        return;
    }
    ui->lab_read_file_path->setText(filepath);
    fileContent.clear();
    QFile f(filepath);
    if(!f.exists()){
        ui->textEdit->append("文件不存在");
        return;
    }
    if(!f.open(QIODevice::ReadOnly)){
        ui->textEdit->append("文件打不开");
        return;
    }
    char buf[1024*7] = {0};
    int len = 0;
    while((len = f.read(buf,1024*7)) > 0){
        fileContent.append(buf,len);
    }
}


void FrUdpClient::on_btn_clean_file_clicked()
{
    ui->lab_read_file_path->setText("");
    fileContent.clear();
}


void FrUdpClient::on_btn_clean_text_clicked()
{
    ui->textEdit->clear();
}

void FrUdpClient::on_btn_search_clicked()
{
    QString key = ui->et_search_text->text();
    if(key.isEmpty()){
        QMessageBox::information(this,"提示信息","搜索关键词为空",QMessageBox::Ok);
        return;
    }
    if(ui->textEdit->find(key)){
        QPalette palette = ui->textEdit->palette();
        palette.setBrush(QPalette::Highlight,Qt::green);
        ui->textEdit->setPalette(palette);
    }
    else{
        ui->textEdit->moveCursor(QTextCursor::Start);
        if(ui->textEdit->find(key)){
            QPalette palette = ui->textEdit->palette();
            palette.setBrush(QPalette::Highlight,Qt::green);
            ui->textEdit->setPalette(palette);
        }
        else{
            QMessageBox::information(this,"提示信息","搜索无结果",QMessageBox::Ok);
        }
    }

}


void FrUdpClient::on_textEdit_textChanged()
{
    ui->textEdit->moveCursor(QTextCursor::End);
}

