#pragma once
namespace constant {
    const int READ_MODE_UTF8 = 0;
    const int READ_MODE_ASCII = 1;
    const int READ_MODE_HEX = 2;

    const int WRITE_MODE_UTF8 = 0;
    const int WRITE_MODE_ASCII = 1;
    const int WRITE_MODE_HEX = 2;

    const int EXPORT_TYPE_TCP_CLIENT = 0;
    const int EXPORT_TYPE_TCP_SERVER = 1;
    const int EXPORT_TYPE_UDP_CLIENT = 2;
    const int EXPORT_TYPE_UDP_SERVER = 3;

    const int EXPORT_STATE_SUCCESS = 0;
    const int EXPORT_STATE_FAIL = 1;
    const int EXPORT_STATE_ERROR = 2;

    const QString EXPORT_TCPSERVER_MESSAGE = "D:/ProjectTools/qtServer/TcpExport";
    const QString EXPORT_TCPCLIENT_MESSAGE = "D:/ProjectTools/qtClient/TcpExport";
    const QString EXPORT_UDPSERVER_MESSAGE = "D:/ProjectTools/qtServer/UdpExport";
    const QString EXPORT_UDPCLIENT_MESSAGE = "D:/ProjectTools/qtClient/UdpExport";
}
