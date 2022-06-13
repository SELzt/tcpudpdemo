#ifndef EXPORTUTIL_H
#define EXPORTUTIL_H

#include <QObject>
#include <head.h>
class ExportUtil : public QObject
{
    Q_OBJECT
public:
    explicit ExportUtil(QObject *parent = nullptr);

signals:
    void exportState(const int state,const QString filepath);
public slots:
    void exports(const char* data, const int type);
};

#endif // EXPORTUTIL_H
