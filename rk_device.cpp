#include "rk_device.h"
#include "rk_device_p.h"

RK_Device::~RK_Device()
{
    delete d_ptr;
}

RK_Device::RK_Device(RK_DevicePrivate *ptr):d_ptr(ptr)
{

}

auto RK_Device::type()const->DeviceType
{
    return d_ptr->type;
}

QByteArray RK_Device::_readData(QObject* sender)
{
    QIODevice *obj = qobject_cast<QIODevice *>(sender);
    if(obj)
        return obj->readAll();
    return "";
}

bool RK_Device::isOpen()const
{
    return d_ptr->device->isOpen();
}

void RK_Device::close()
{
    disconnect();
    d_ptr->device->disconnect();
    if(d_ptr->device->isOpen()){
        d_ptr->device->close();
    }
}

bool RK_Device::open()
{
    if(d_ptr->device->isOpen())
    {
        connect(d_ptr->device, &QIODevice::readyRead,this, &RK_Device::_receiveInfo);
        return true;
    }

    if(d_ptr->device->open(QIODevice::ReadWrite))
    {
        connect(d_ptr->device, &QIODevice::readyRead,this, &RK_Device::_receiveInfo);
        return true;
    }
    return false;
}

bool RK_Device::send(const char*data,int length)
{
    int ret = d_ptr->device->write(data,length);
    return ret == length;
}

QByteArray RK_Device::readWaitNextData(int msecs)
{
    Q_CD(RK_Device);
    d->device->waitForReadyRead(msecs);
    return readAllCurrentData();
}

#include <QTimer>
#include <QCoreApplication>
QByteArray RK_Device::readWaitNextDataSem(bool &sem, int msecs)
{
    bool timeout =false;
    QTimer::singleShot(msecs,[&timeout](){timeout=true;});

    while(d_ptr->revCount ==0 && timeout == false && sem)
    {
        QCoreApplication::processEvents();
    }

    return readAllCurrentData();
}

QByteArray RK_Device::readAllCurrentData()
{
    QByteArray ret = d_ptr->revData;
    d_ptr->revCount =0;
    d_ptr->revData.clear();
    return ret;
}

void RK_Device::_receiveInfo()
{
    Q_D(RK_Device);
    d->revCount ++;
    auto send = sender();
    d->revData += _readData(send);
    emit readReady(send);
}
