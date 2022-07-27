#include "rk_udpnet.h"
#include "rk_device_p.h"
#include <QUdpSocket>

class RK_UdpNetPrivate:public RK_DevicePrivate
{
public:
    RK_UdpNetPrivate():RK_DevicePrivate(RK_Device::e_updNet){

    }
    int dst_port = 1025;
    int src_port = 1024;
    QHostAddress dst_addr{"127.0.0.1"};
};

RK_UdpNet::RK_UdpNet():RK_Device(new RK_UdpNetPrivate())
{
    d_ptr->device = new QUdpSocket();
}

bool RK_UdpNet::open()
{
    RK_DD(QUdpSocket);
    Q_D(RK_UdpNet);
    if(dd->bind(d->src_port)){
        return RK_Device::open();
    }
    return false;
}

bool RK_UdpNet::send(const char *data, int length)
{
    Q_D(RK_UdpNet);
    RK_DD(QUdpSocket);

    auto ll = dd->writeDatagram(data,length,d->dst_addr,d->dst_port);
    return ll==length;
}

bool RK_UdpNet::setDstPort(int port)
{
    Q_D(RK_UdpNet);
    d->dst_port = port;
    return true;
}

int RK_UdpNet::dstPort() const
{
    Q_CD(RK_UdpNet);
    return d->dst_port;
}

bool RK_UdpNet::setSrcPort(int port)
{
    Q_D(RK_UdpNet);
    d->src_port = port;
    return true;
}

int RK_UdpNet::srcPort()const
{
    Q_CD(RK_UdpNet);
    return d->src_port;
}

bool RK_UdpNet::setDstAddr(QString addr)
{
    Q_D(RK_UdpNet);
    bool ret =d->dst_addr.setAddress(addr);
    return ret;
}

QString RK_UdpNet::dstAddr() const
{
    Q_CD(RK_UdpNet);
    return d->dst_addr.toString();
}

QByteArray RK_UdpNet::_readData(QObject* sender)
{
    QUdpSocket *obj = qobject_cast<QUdpSocket *>(sender);
    QByteArray arr;
    if(obj)
    {
        arr.resize(obj->bytesAvailable());
        obj->readDatagram(arr.data(),arr.size());
    }
    return arr;
}
