#include "rk_device.h"
#include "rk_device_p.h"
#include <QElapsedTimer>
#include <QCoreApplication>

RK_Device::~RK_Device()
{
    delete d_ptr;
}

RK_Device::RK_Device(RK_DevicePrivate *ptr):d_ptr(ptr)
{

}

auto RK_Device::type()const->DeviceType
{
    Q_CD(RK_Device);
    return d->type;
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
    Q_CD(RK_Device);
    return d->device->isOpen();
}

void RK_Device::close()
{
    Q_D(RK_Device);
    disconnect(d->device, &QIODevice::readyRead,this, &RK_Device::_receiveInfo);
    disconnect();
    d->device->disconnect();
    if(d->device->isOpen()){
        d->device->close();
    }
}

bool RK_Device::open()
{
    Q_D(RK_Device);
    if(d->device->isOpen())
    {
        connect(d->device, &QIODevice::readyRead,this, &RK_Device::_receiveInfo);
        return true;
    }

    if(d->device->open(QIODevice::ReadWrite))
    {
        connect(d->device, &QIODevice::readyRead,this, &RK_Device::_receiveInfo);
        return true;
    }
    return false;
}

bool RK_Device::send(const char*data,int length)
{
    Q_D(RK_Device);
    int ret = d->device->write(data,length);
    return ret == length;
}

QByteArray RK_Device::readWaitNextData(int msecs)
{
    Q_CD(RK_Device);
    d->device->waitForReadyRead(msecs);
    return readAllCurrentData();
}

QByteArray RK_Device::readWaitNextDataSem(int msecs,bool *sem)
{
    if(!sem)
        return "";

    QElapsedTimer timer;
    timer.start();

    Q_D(RK_Device);
    while(d->revCount ==0 && timer.elapsed()<msecs && *sem)
    {
        QCoreApplication::processEvents();
    }

    return readAllCurrentData();
}

QByteArray RK_Device::readAllCurrentData()
{
    Q_D(RK_Device);
    QByteArray ret = d->revData;
    d->revCount =0;
    d->revData.clear();
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
