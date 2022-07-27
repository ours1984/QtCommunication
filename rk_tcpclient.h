#ifndef RK_TCPCLIENT_H
#define RK_TCPCLIENT_H

#include "rk_device.h"

class RK_TCPClientPrivate;
class RK_TCPClient : public RK_Device
{
    Q_DECLARE_PRIVATE(RK_TCPClient);
public:
    RK_TCPClient();

    bool open()override;
    void close()override;

    bool setDstPort(int port);
    int dstPort()const;

    bool setDstAddr(QString);
    QString dstAddr()const;
};

#endif // RK_TCPCLIENT_H
