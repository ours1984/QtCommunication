#include "rk_serialport.h"
#include "rk_device_p.h"
#include "QSerialPortInfo"

class RK_SerialPortPrivate:public RK_DevicePrivate
{
public:
    RK_SerialPortPrivate():RK_DevicePrivate(RK_Device::e_serial){

    }
    int station = -1;
};

RK_SerialPort::RK_SerialPort()
    :RK_Device(new RK_SerialPortPrivate())
{
    auto sp = new QSerialPort();
    sp->setBaudRate(QSerialPort::Baud115200);
    sp->setDataBits(QSerialPort::Data8);
    sp->setStopBits(QSerialPort::OneStop);
    sp->setParity(QSerialPort::NoParity);

    for(auto &info:QSerialPortInfo::availablePorts()){
        sp->setPortName(info.portName());
        break;
    }
    d_ptr->device = sp;
}

void RK_SerialPort::setPortName(const QString &name)
{
    RK_DD(QSerialPort);
    dd->setPortName(name);
}

QString RK_SerialPort::portName() const
{
    RK_DD(QSerialPort);
    return dd->portName();
}

QSerialPort::BaudRate RK_SerialPort::baudRate()const
{
    RK_DD(QSerialPort);
    return (QSerialPort::BaudRate)dd->baudRate();
}

bool RK_SerialPort::setBaudRate(QSerialPort::BaudRate bd)
{
    RK_DD(QSerialPort);
    return dd->setBaudRate(bd);
}

QSerialPort::DataBits RK_SerialPort::dataBits()const
{
    RK_DD(QSerialPort);
    return dd->dataBits();
}

bool RK_SerialPort::setDataBits(QSerialPort::DataBits bd)
{
    RK_DD(QSerialPort);
    return dd->setDataBits(bd);
}

QSerialPort::StopBits RK_SerialPort::stopBits()const
{
    RK_DD(QSerialPort);
    return dd->stopBits();
}

bool RK_SerialPort::setStopBits(QSerialPort::StopBits bd)
{

    RK_DD(QSerialPort);
    return dd->setStopBits(bd);
}

QSerialPort::Parity RK_SerialPort::parity()const
{
    RK_DD(QSerialPort);
    return dd->parity();
}

bool RK_SerialPort::setParity(QSerialPort::Parity bd)
{
    RK_DD(QSerialPort);
    return dd->setParity(bd);
}

bool RK_SerialPort::setStation(int st)
{
    Q_D(RK_SerialPort);
    d->station = st;
    return true;
}

int RK_SerialPort::station()const
{
    Q_CD(RK_SerialPort);
    return d->station;
}





