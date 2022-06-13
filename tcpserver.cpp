#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QtDebug>
TcpServer::TcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);
}

TcpServer::~TcpServer()
{
    delete ui;
}
void TcpServer::init(){
    ui->text_server->setReadOnly(true);
    ui->et_listen_ip_server->setText("127.0.0.1");
    ui->et_listen_port_server->setText("3257");
    ui->et_send_server->setText("10000");
    ui->et_timer_server->setText("1000");
    server = new QTcpServer;
    connect(server,SIGNAL(newConnection()),this,SLOT(newConnectedSlot()));

}

void TcpServer::on_btn_listen_server_clicked(){
    
    QString ip = ui->et_listen_ip_server->text();
    QString port = ui->et_listen_port_server->text();
    if (ip.size() == 0 || port.size() == 0) {
        ui->text_server->append("IP或端口为空");
        return;
    }
    QHostAddress host = QHostAddress(ip);
    if (!server->isListening()) {
        //开始监听
        if (!server->listen(host, port.toInt())) {
            //监听失败
            ui->text_server->append(server->errorString());
        }
        else {
            ui->text_server->append(("监听" + port + "端口成功"));
            ui->btn_listen_server->setText("结束监听");
        }
    }
    else {
        //结束监听
        ui->text_server->append("结束监听");
        ui->btn_listen_server->setText("开始监听");
        server->close();
        if(timer != nullptr){
            timer->stop();
            ui->cb_timer_server->setChecked(false);
        }
        for(TcpClient* client : qAsConst(clientList)){
            client->socket->close();
        }
    }
}

void TcpServer::newConnectedSlot() {
    //新连接
    //qDebug()<<"新链接";
    TcpClient* client = new TcpClient;
    client->socket = server->nextPendingConnection();
    client->socket->setReadBufferSize(1024*1024*7);
    client->isServer = true;
    QString ip = client->socket->peerAddress().toString();
    QString port = QString::number(client->socket->peerPort());
    ui->text_server->append("连接成功:" + ip + ":" + port);
    clientList.append(client);
    //读取信息槽
    connect(client->socket, &QTcpSocket::readyRead, this, [=](){
        QByteArray buffer = client->socket->readAll();
        if(!isReceiveFile){
            QString ip = client->socket->peerAddress().toString();
            QString port = QString::number(client->socket->peerPort());
            QString msg;
            msg = msg.fromUtf8(buffer);
            if(msg.startsWith("file:")){
                //文件头
                isReceiveFile = true;
                QString filename = msg.mid(msg.indexOf(":")+1,msg.lastIndexOf(":")-msg.indexOf(":")-1);
                filesize = msg.mid(msg.lastIndexOf(":")+1,-1).toInt();
                QDir dir("D:/ProjectTools/qtServer");
                dir.mkdir("D:/ProjectTools/qtServer");
                f = new QFile("D:/ProjectTools/qtServer/" + filename);
                f->open(QIODevice::WriteOnly);
                return;
            }

            QString temp = "[接收] [" + ip + ":" + port + "]:";
            if(readMode == constant::READ_MODE_UTF8){
                //utf8
                msg = QString::fromLocal8Bit(buffer);
            }
            else if(readMode == constant::READ_MODE_ASCII){
                //ascii
                msg = QString::fromUtf8(buffer);
            }
            else{
                //hex
                msg = QByteArray::fromHex(buffer);
            }
            ui->text_server->append(temp + msg);
        }
        else {
            receivesize += f->write(buffer,buffer.size());
            if(receivesize >= filesize){
                //写入完毕 将接收文件标识置为false 并将其余有关文件的信息置为初始值
                ui->text_server->append("文件写入完毕");

                isReceiveFile = false;
                filesize = 0;
                receivesize = 0;
                f->close();
            }
        }
        });
    //断开连接槽
    connect(client->socket,&QTcpSocket::disconnected,this,[=](){
        ui->text_server->append("客户端断开连接");
        QString ip = client->socket->peerAddress().toString();
        QString port = QString::number(client->socket->peerPort());
        QString temp = ip + ":" + port;

        int index = -1;
        for(int i = 0;i<ui->lw_client_list_server->count(); i++){

            if(temp == ui->lw_client_list_server->item(i)->text()){
                index = i;
                break;
            }
        }

        delete ui->lw_client_list_server->takeItem(index);
        clientList.removeOne(client);
        if(timer != nullptr){
            timer->stop();
            ui->cb_timer_server->setChecked(false);
        }
    });
    //更新客户端列表
    ui->lw_client_list_server->addItem(ip + ":" + port);
    
}

void TcpServer::exportStateSlot(const int state, const QString filepath){
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


void TcpServer::on_btn_send_server_clicked()
{
    if(filepath.isEmpty()){
        QString msg;
        if(!readContent.isEmpty()){
            msg = QString::fromUtf8(readContent);
        }
        else {
            msg = ui->et_send_server->text();
        }
        if(msg.isEmpty()){
            return;
        }
        QByteArray buffer;
        if(writeMode == constant::WRITE_MODE_UTF8){
            //utf8
            buffer = msg.toUtf8();
        }
        else if (writeMode == constant::WRITE_MODE_ASCII) {
            //字符
            buffer = msg.toLatin1();
        }
        else{//十六进制
            buffer = msg.toUtf8();
            buffer = buffer.toHex(' ').toUpper();

        }
        QString ip = server->serverAddress().toString();
        QString port = QString::number(server->serverPort());
        ui->text_server->append("[发送] [" + ip + ":" + port + "]:" + msg);
        for(TcpClient* c : qAsConst(clientList)){
            c->socket->write(buffer);
            c->socket->flush();
        }
    }
    else {
        //发送文件
        QFileInfo fileinfo(filepath);
        QString filename = fileinfo.fileName();
        qint64 filesize = fileinfo.size();
        QString tcpHead =  "file:"+filename + ":" + QString::number(filesize);

        QString filepathTemp = filepath;
        foreach(TcpClient *client , clientList){
            client->socket->write(tcpHead.toUtf8());
            QTimer* t = new QTimer;
            connect(t,&QTimer::timeout,this,[=](){
                t->stop();
                QFile f(filepathTemp);
                if(!f.open(QIODevice::ReadOnly)){
                    ui->text_server->append("文件无法打开");
                    return;
                }
                int len = 0;
                char buf[7*1024] = {0};
                while((len = f.read(buf,1024*7))>0){
                    //qDebug()<<"len"<<len;
                    client->socket->write(buf,len);
                }
                f.close();
            });
            t->start(20);
        }
        ui->lab_file_dir_server->setText("");
        filepath = "";
    }
}


void TcpServer::on_cb_char_send_server_stateChanged(int arg1)
{
    //选中是2，未选中是0

    if(arg1 == 2){
        ui->cb_hex_send_server->setCheckState(Qt::Unchecked);
        writeMode = constant::WRITE_MODE_ASCII;
    }
    else{
        if(ui->cb_hex_send_server->checkState() == Qt::Unchecked){
            writeMode = constant::WRITE_MODE_UTF8;
        }
    }

}


void TcpServer::on_cb_char_receive_server_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->cb_hex_receive_server->setCheckState(Qt::Unchecked);
        readMode = constant::READ_MODE_ASCII;
    }
    else{
        if(ui->cb_hex_receive_server->checkState() == Qt::Unchecked){
            readMode = constant::READ_MODE_UTF8;
        }
    }
}


void TcpServer::on_cb_hex_send_server_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->cb_char_send_server->setCheckState(Qt::Unchecked);
        writeMode = constant::WRITE_MODE_HEX;
    }
    else{
        if(ui->cb_char_send_server->checkState() == Qt::Unchecked){
            writeMode = constant::WRITE_MODE_UTF8;
        }
    }

}


void TcpServer::on_cb_hex_receive_server_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->cb_char_receive_server->setCheckState(Qt::Unchecked);
        readMode = constant::READ_MODE_HEX;
    }
    else{
        if(ui->cb_char_receive_server->checkState() == Qt::Unchecked){
            readMode = constant::READ_MODE_UTF8;
        }
    }
}


void TcpServer::on_cb_timer_server_stateChanged(int arg1)
{
    if(arg1 == 2){
        if(timer == nullptr){
            timer = new QTimer;
            connect(timer,&QTimer::timeout,ui->btn_send_server,&QPushButton::click);
        }
        int delay = ui->et_timer_server->text().toInt();

        timer->start(delay);
    }
    else{
        timer->stop();
    }

}


void TcpServer::on_btn_choose_file_server_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"选择发送文件","/","所有文件(*.*);;");
    if(!filePath.isEmpty()){
        //qDebug()<<filePath;
        ui->lab_file_dir_server->setText(filePath);
        filepath = filePath;
    }


}





void TcpServer::on_btn_read_file_server_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"选择读取文件","/","所有文件(*.*);;");
    if(!path.isEmpty()){
        ui->lab_read_file_name->setText(path);
        readContent.clear();
        QFile f(path);
        if(f.exists()){
            if(f.open(QIODevice::ReadOnly)){
                QByteArray temp;
                qint64 len = 0;
                char buf[7*1024]  = {0};
                while((len = f.read(buf,7*1024))>0){
                    temp.append(buf,len);
                }
                readContent = temp;
            }
            else
                ui->text_server->append("文件无法打开");
        }
        else {
            ui->text_server->append("文件不存在");
        }
    }
}


void TcpServer::on_btn_clean_read_file_clicked()
{
    readContent.clear();
    ui->lab_read_file_name->setText("");
}

void TcpServer::on_text_server_textChanged()
{
    ui->text_server->moveCursor(QTextCursor::End);
}

void TcpServer::on_btn_search_server_clicked()
{
    QString key = ui->et_search_server->text();
    if(key.isEmpty()){
        QMessageBox::information(this,"提示信息","关键字为空",QMessageBox::Ok);
        return;
    }
    if(ui->text_server->find(key)){
        QPalette palette = ui->text_server->palette();
        palette.setBrush(QPalette::Highlight,Qt::green);
        ui->text_server->setPalette(palette);
    }
    else {
        ui->text_server->moveCursor(QTextCursor::Start);
        if(!ui->text_server->find(key)){
            isFirstFind = true;
            ui->text_server->moveCursor(QTextCursor::End);
            QMessageBox::information(this,"提示信息","查询该关键字无结果",QMessageBox::Ok);

        }
        else {
            QPalette palette = ui->text_server->palette();
            palette.setBrush(QPalette::Highlight,Qt::green);
            ui->text_server->setPalette(palette);
        }
    }
}


void TcpServer::on_btn_clean_text_clicked()
{
    ui->text_server->clear();
}


void TcpServer::on_btn_export_server_clicked()
{
    QString content = ui->text_server->toPlainText();
    if(content.isEmpty()){
        QMessageBox::information(this,"提示信息","导出内容为空",QMessageBox::Ok);
        return;
    }
    char *buf = content.toUtf8().data();
    emit exportSignal(buf,constant::EXPORT_TYPE_TCP_SERVER);
}


void TcpServer::on_btn_import_server_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,"请选择文件","/ProjectTools","文本文档(*.txt);;");
    if(!filepath.isEmpty()){
        QFile f(filepath);
        if(!f.open(QIODevice::ReadOnly)){
            QMessageBox::information(this,"提示信息","文件无法打开",QMessageBox::Ok);
            return;
        }
        char buf[7*1024] = {0};
        qint64 len = 0;
        QByteArray temp;
        while((len = f.read(buf,1024*7)) > 0){
            temp.append(buf,len);
        }
        ui->text_server->append(QString::fromUtf8(temp,temp.size()));
        f.close();
    }
}

