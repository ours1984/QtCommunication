#ifndef RK_TCPSERVER_H
#define RK_TCPSERVER_H

#include "rk_device.h"

class RK_TCPServerPrivate;
class RK_TCPServer : public RK_Device
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RK_TCPServer);
public:
    RK_TCPServer();

    QStringList getClients()const;

    bool setCurrentClient(QString);
    QString currentClinet()const;

    bool send(const char*data,int length)override;

    bool isOpen()const override;

    bool open()override;

    void close()override;

    bool setListenPort(int port);
    int listenPort()const;

signals:
    void newConnection();

private slots:
    void clsConnect();
    void newConnect();
};

#endif // RK_TCPSERVER_H
