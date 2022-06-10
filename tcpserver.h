#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include "tcpclient.h"
namespace Ui {
class TcpServer;
}

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void init();
signals:
    void listenState(const int mode, const QString &msg);
private slots:
    void newConnectedSlot();

    void on_btn_listen_server_clicked();
    

    void on_btn_send_server_clicked();

    void on_cb_char_send_server_stateChanged(int arg1);

    void on_cb_char_receive_server_stateChanged(int arg1);

    void on_cb_hex_send_server_stateChanged(int arg1);

    void on_cb_hex_receive_server_stateChanged(int arg1);

    void on_cb_timer_server_stateChanged(int arg1);

    void on_btn_choose_file_server_clicked();

    void on_text_server_textChanged();

private:
    Ui::TcpServer *ui;
    QTimer *timer = nullptr;
    QTcpServer *server;
    QList<TcpClient *> clientList;
    int writeMode = 0; //0 utf-8 ,1 ascii ,2 hex
    int readMode = 0; //0 utf-8 ,1 ascii ,2 hex
    bool isReceiveFile = false;
    QString filepath;
    quint64 filesize;
    quint64 receivesize;
    QFile *f;

};

#endif // TCPSERVER_H
