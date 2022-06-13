#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QtNetwork>
#include "constant.h"
#include <QFileDialog>
#include <QFile>
#include <QTextDocument>
#include <QMessageBox>
#include <QDateTime>
namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:


    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void init();
signals:
    void receiveMsg();
    void exportSignal(const char* data, const int type);
public slots:
    void exportStateSlot(const int state,const QString filepath);
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

    void on_btn_read_file_client_clicked();

    void on_btn_clean_read_file_clicked();

    void on_btn_clean_text_clicked();

    void on_btn_import_client_clicked();

    void on_btn_export_client_clicked();

public:
    bool isServer = false;
    bool isFirstFind = true;
    QTcpSocket* socket = nullptr;
private:
    Ui::TcpClient *ui;
    QTimer* timer = nullptr;
    bool isConnected = false;
    bool isReceiveFile = false;
    int readMode = 0;
    int writeMode = 0;

    quint64 filesize;
    quint64 receivesize;
    QFile *f;
    QString filepath;
    QByteArray readContent;
};

#endif // TCPCLIENT_H
