#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QtDebug>
TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::init()
{
    ui->et_bind_ip_client->setText("127.0.0.1");
    ui->et_bind_port_client->setText("36985");
    ui->et_server_ip_client->setText("127.0.0.1");
    ui->et_server_port_client->setText("3257");
    ui->et_send_client->setText("20000");
    ui->et_timer_client->setText("1000");
}

void TcpClient::on_btn_search_client_clicked()
{

}


void TcpClient::on_btn_connect_client_clicked()
{
    QString bindIp = ui->et_bind_ip_client->text();
    QString bindPort = ui->et_bind_port_client->text();
    QString serverIp = ui->et_server_ip_client->text();
    QString serverPort = ui->et_server_port_client->text();
    if(socket == nullptr){
        socket = new QTcpSocket();
    }
    if(isConnected){
        //已经连接过了
        isConnected = false;
        socket->close();
        socket->deleteLater();
        socket = nullptr;
        //ui->text_client->append("服务器断开连接");
        ui->btn_connect_client->setText("开始连接");
        if(timer != nullptr){
            timer->stop();
            ui->cb_timer_client->setChecked(false);
        }
        return;
    }
    QHostAddress bindHost = QHostAddress(bindIp);

    //socket->bind(bindHost,bindPort.toInt());
   // qDebug()<<"bind:"<<bindIp<<bindPort;
    socket->connectToHost(serverIp,serverPort.toInt());
    //qDebug()<<bindHost<<bindPort.toInt();

    if(socket->waitForConnected(3000)){
        ui->text_client->append("成功连接到" + serverIp + ":" + serverPort);
        isConnected = true;
        ui->btn_connect_client->setText("断开连接");
        //断开连接的槽
        connect(socket,&QTcpSocket::disconnected,this,[=](){
            ui->text_client->append("服务器断开连接");
            ui->btn_connect_client->setText("开始连接");
            isConnected = false;
            if(timer != nullptr){
                timer->stop();
                ui->cb_timer_client->setChecked(false);
            }
        });
        //接收数据的槽
        connect(socket,&QTcpSocket::readyRead,this,[=](){
            if(isServer){
                emit receiveMsg();
                return;
            }
            QByteArray buffer = socket->readAll();
            if(!isReceiveFile){
                QString ipStr = socket->peerAddress().toString();
                QString portStr = QString::number(socket->peerPort());
                QString msg;
                msg = msg.fromUtf8(buffer);
                if(msg.startsWith("file:")){
                    //文件头
                    isReceiveFile = true;
                    QString filename = msg.mid(msg.indexOf(":")+1,msg.lastIndexOf(":")-msg.indexOf(":")-1);
                    filesize = msg.mid(msg.lastIndexOf(":")+1,-1).toInt();
                    QDir dir("D:/ProjectTools/qtClient");
                    dir.mkdir("D:/ProjectTools/qtClient");
                    f = new QFile("D:/ProjectTools/qtClient/" + filename);
                    //qDebug()<<"filename"<<filename<<"filesize"<<filesize<<"lastIndex"<<msg.lastIndexOf(":");
                    f->open(QIODevice::WriteOnly);

                    return;
                }
                if(readMode == constant::READ_MODE_HEX) {
                    msg = QByteArray::fromHex(buffer);
                }
                else {
                    msg = msg.fromUtf8(buffer);
                }
                ui->text_client->append("[接收] [" + ipStr + ":" + portStr + "]:"+msg);
            }
            else {

                receivesize += f->write(buffer,buffer.size());
                if(receivesize >= filesize){
                    //写入完毕 将接收文件标识置为false 并将其余有关文件的信息置为初始值
                    ui->text_client->append("文件写入完毕");
                    isReceiveFile = false;
                    filesize = 0;
                    receivesize = 0;
                    f->close();
                }
            }
        });
    }else {
        ui->text_client->append("连接超时");
    }
}

void TcpClient::on_btn_send_client_clicked()
{   //发送数据
    if(filepath.isEmpty()){
        QString msg = ui->et_send_client->text();
        QByteArray buffer;
        //QByteArray temp;
        if(writeMode == constant::WRITE_MODE_UTF8){
            //utf8
            buffer = msg.toLatin1();
        }
        else if (writeMode == constant::WRITE_MODE_ASCII) {
            //字符
            buffer = msg.toUtf8();

        }
        else{//十六进制
            buffer = msg.toUtf8();
            buffer = buffer.toHex(' ').toUpper();
        }
        QString ipStr = socket->localAddress().toString();
        QString portStr = QString::number(socket->localPort());
        ui->text_client->append("[发送] [" + ipStr + ":" + portStr + "]:"+msg);
        socket->write(buffer);
        socket->flush();
    }
    else{
        //发送文件
        QFileInfo fileinfo(filepath);
        QString filename = fileinfo.fileName();
        qint64 filesize = fileinfo.size();
        QString tcpHead =  "file:"+filename + ":" + QString::number(filesize);

        QString filepathTemp = filepath;

        socket->write(tcpHead.toUtf8());
        QTimer* t = new QTimer;
        connect(t,&QTimer::timeout,this,[=](){
            t->stop();
            QFile f(filepathTemp);
            if(!f.open(QIODevice::ReadOnly)){
                ui->text_client->append("文件无法打开");
                return;
            }
            int len = 0;
            char buf[7*1024] = {0};
            while((len = f.read(buf,1024*7))>0){
                //qDebug()<<"len"<<len;
                socket->write(buf,len);
            }
            socket->flush();
            f.close();
        });
        t->start(20);
        ui->lab_file_dir_client->setText("");
        filepath = "";
    }
}


void TcpClient::on_cb_char_send_client_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->cb_hex_send_client->setCheckState(Qt::Unchecked);
        writeMode = constant::WRITE_MODE_ASCII;
    }
    else{
        if(ui->cb_hex_send_client->checkState() == Qt::Unchecked){
            writeMode = constant::WRITE_MODE_UTF8;
        }
    }
    QString ip = socket->localAddress().toString();
    QString port = QString::number(socket->localPort());
    QString msg = ui->et_send_client->text();
    ui->text_client->append("[发送] [" + ip + ":" + port + "]:" + msg);
}


void TcpClient::on_cb_char_receive_client_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->cb_hex_receive_client->setCheckState(Qt::Unchecked);
        readMode = constant::READ_MODE_ASCII;
    }
    else{
        if(ui->cb_hex_receive_client->checkState() == Qt::Unchecked){
            readMode = constant::READ_MODE_UTF8;
        }
    }
}


void TcpClient::on_cb_hex_send_client_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->cb_char_send_client->setCheckState(Qt::Unchecked);
        writeMode = constant::WRITE_MODE_HEX;
    }
    else{
        if(ui->cb_char_send_client->checkState() == Qt::Unchecked){
            writeMode = constant::WRITE_MODE_UTF8;
        }
    }
}


void TcpClient::on_cb_hex_receive_client_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->cb_char_receive_client->setCheckState(Qt::Unchecked);
        readMode = constant::READ_MODE_HEX;
    }
    else{
        if(ui->cb_char_receive_client->checkState() == Qt::Unchecked){
            readMode = constant::READ_MODE_UTF8;
        }
    }
}


void TcpClient::on_cb_timer_client_stateChanged(int arg1)
{
    if(arg1 == 2){
        int delay = ui->et_timer_client->text().toInt();
        if(timer == nullptr){
            timer = new QTimer;
        }
         connect(timer,&QTimer::timeout,ui->btn_send_client,&QPushButton::click);
        timer->start(delay);
    }
    else {
        timer->stop();
    }

}



void TcpClient::on_btn_choose_file_client_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"选择发送文件","/","所有文件(*.*);;");
    if(!filePath.isEmpty()){
        //qDebug()<<filePath;
        ui->lab_file_dir_client->setText(filePath);
        filepath = filePath;
    }
}


void TcpClient::on_text_client_textChanged()
{
    ui->text_client->moveCursor(QTextCursor::End);
}

