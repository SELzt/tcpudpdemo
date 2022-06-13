#include "exportutil.h"

ExportUtil::ExportUtil(QObject *parent) : QObject(parent)
{

}
void ExportUtil::exports(const char *data, const int type){
    QString filepath;
    switch (type) {
    case constant::EXPORT_TYPE_TCP_CLIENT:
        filepath = constant::EXPORT_TCPCLIENT_MESSAGE;
        break;
    case constant::EXPORT_TYPE_TCP_SERVER:
        filepath = constant::EXPORT_TCPSERVER_MESSAGE;
        break;
    case constant::EXPORT_TYPE_UDP_CLIENT:
        filepath = constant::EXPORT_UDPCLIENT_MESSAGE;
        break;
    case constant::EXPORT_TYPE_UDP_SERVER:
        filepath = constant::EXPORT_UDPSERVER_MESSAGE;
        break;
    default:

        return;
    }
    filepath = filepath + "/" + QString::number(QDateTime::currentDateTimeUtc().toTime_t()) + ".txt";
    QFile f(filepath);
    if(!f.open(QIODevice::WriteOnly)){
        emit exportState(constant::EXPORT_STATE_ERROR,"");
        return;
    }
    f.write(data);
    f.close();
    emit exportState(constant::EXPORT_STATE_SUCCESS,filepath);
}
