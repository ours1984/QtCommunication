#include "rk_tcpclient.h"
#include "rk_device_p.h"
#include <QTcpSocket>
#include <QHostAddress>

class RK_TCPClientPrivate:public RK_DevicePrivate
{
public:
    RK_TCPClientPrivate():RK_DevicePrivate(RK_Device::e_tcpClient){

    }
    int dst_port = 8080;
    QHostAddress dst_addr{"127.0.0.1"};
};

RK_TCPClient::RK_TCPClient()
    :RK_Device(new RK_TCPClientPrivate())
{
    d_ptr->device = new QTcpSocket();
}

bool RK_TCPClient::open()
{
    Q_D(RK_TCPClient);
    RK_DD(QTcpSocket);
    dd->connectToHost(d->dst_addr,d->dst_port);
    if(dd->waitForConnected())
    {
        return RK_Device::open();
    }
    return false;
}

void RK_TCPClient::close()
{
    RK_DD(QTcpSocket);
    dd->disconnectFromHost();
    RK_Device::close();
}

bool RK_TCPClient::setDstPort(int port)
{
    Q_D(RK_TCPClient);
    d->dst_port = port;
    return true;
}

int RK_TCPClient::dstPort() const
{
    Q_CD(RK_TCPClient);
    return d->dst_port;
}

bool RK_TCPClient::setDstAddr(QString addr)
{
    Q_D(RK_TCPClient);
    return d->dst_addr.setAddress(addr);
}

QString RK_TCPClient::dstAddr() const
{
    Q_CD(RK_TCPClient);
    return d->dst_addr.toString();
}
