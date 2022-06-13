#ifndef FRUDPSERVER_H
#define FRUDPSERVER_H

#include <QWidget>
#include <head.h>
namespace Ui {
class FrUdpServer;
}

class FrUdpServer : public QWidget
{
    Q_OBJECT

public:
    explicit FrUdpServer(QWidget *parent = nullptr);
    ~FrUdpServer();
    void _init();
signals:
    void exportSignal(const char* data, const int type);
public slots:
    void exportStateSlot(const int state,const QString filepath);
private slots:
    void readSlot();

    void on_btn_listen_clicked();
    void on_btn_export_clicked();
    void on_cb_hex_send_stateChanged(int arg1);

    void on_cb_hex_receive_stateChanged(int arg1);

    void on_cb_timer_stateChanged(int arg1);

    void on_btn_send_clicked();

    void on_btn_read_file_clicked();

    void on_btn_clean_file_clicked();

    void on_btn_clean_text_clicked();

    void on_btn_import_clicked();

    void on_textEdit_textChanged();

public:
    bool isHexSend = false;
    bool isHexRecv = false;
    bool isTimer = false;
    struct MyClient
    {
        QHostAddress ip;
        quint16 port;
    };
private:
    Ui::FrUdpServer *ui;
    QUdpSocket *server;
    QList<MyClient *> clientList;
    QTimer timer;
    QByteArray fileContent;
    bool isListen = false;
};

#endif // FRUDPSERVER_H
