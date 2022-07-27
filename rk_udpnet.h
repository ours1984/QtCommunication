#ifndef RK_UDPNET_H
#define RK_UDPNET_H

#include "rk_device.h"

class RK_UdpNetPrivate;
class RK_UdpNet : public RK_Device
{
    Q_DECLARE_PRIVATE(RK_UdpNet);
public:
    RK_UdpNet();
    bool open()override;

    bool send(const char*data,int length)override;

    bool setDstPort(int port);
    int dstPort()const;

    bool setSrcPort(int port);
    int srcPort()const;

    bool setDstAddr(QString);
    QString dstAddr()const;

protected:
    QByteArray _readData(QObject*)override;

};

#endif // RK_UDP_H
