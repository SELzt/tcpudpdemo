#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QtNetwork>
#include "constant.h"
#include <QFileDialog>
#include <QFile>
namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    QTcpSocket* socket = nullptr;

    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void init();
signals:
    void receiveMsg();
private slots:
    void on_btn_search_client_clicked();

    void on_btn_connect_client_clicked();

    void on_btn_send_client_clicked();
    void on_cb_char_send_client_stateChanged(int arg1);

    void on_cb_char_receive_client_stateChanged(int arg1);

    void on_cb_hex_send_client_stateChanged(int arg1);

    void on_cb_hex_receive_client_stateChanged(int arg1);

    void on_cb_timer_client_stateChanged(int arg1);

    void on_btn_choose_file_client_clicked();

    void on_text_client_textChanged();

public:
    bool isServer = false;
private:
    Ui::TcpClient *ui;
    QTimer* timer = nullptr;
    bool isConnected = false;
    int readMode = 0;
    int writeMode = 0;
    bool isReceiveFile = false;
    quint64 filesize;
    quint64 receivesize;
    QFile *f;
    QString filepath;

};

#endif // TCPCLIENT_H
