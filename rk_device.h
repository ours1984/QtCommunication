#ifndef RK_DEVICE_H
#define RK_DEVICE_H

#include<QObject>
#include<functional>

class RK_DevicePrivate;
class RK_Device:public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RK_Device)
    Q_DISABLE_COPY(RK_Device)

public:

    enum DeviceType{
        e_serial,
        e_tcpClient,
        e_tcpServer,
        e_updNet,
     };

    virtual ~RK_Device();

    virtual bool isOpen()const;

    virtual bool open();

    virtual void close();

    virtual bool send(const char*data,int length);
    bool send(QByteArray str){return send(str.constData(),str.length());}
    bool send(QString str){return send(str.toUtf8());}

    QByteArray readWaitNextData(int msecs = 30000);

    QByteArray readWaitNextDataSem(int msecs,bool *sem);

    QByteArray readAllCurrentData();

    auto type()const ->DeviceType;

signals:
    void readReady(QObject* sender);
    void connectChanged();

protected:
    virtual QByteArray _readData(QObject* sender);

protected slots:

    void _receiveInfo();

protected:
    RK_Device(RK_DevicePrivate*);
    RK_DevicePrivate* d_ptr;

};



#endif // RK_DEVICE_H
