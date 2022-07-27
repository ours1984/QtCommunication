#ifndef RK_SERIALPORT_H
#define RK_SERIALPORT_H

#include "rk_device.h"
#include <QSerialPort>

class RK_SerialPortPrivate;
class RK_SerialPort : public RK_Device
{
    Q_DECLARE_PRIVATE(RK_SerialPort);
public:
    RK_SerialPort();

    void setPortName(const QString &name);
    QString portName() const;

    bool setBaudRate(QSerialPort::BaudRate);
    bool setBaudRate(int bd){return setBaudRate(QSerialPort::BaudRate(bd));}

    QSerialPort::BaudRate baudRate()const;

    bool setDataBits(QSerialPort::DataBits);
    bool setDataBits(int bd){return setDataBits(QSerialPort::DataBits(bd));}

    QSerialPort::DataBits dataBits()const;

    bool setStopBits(QSerialPort::StopBits);
    bool setStopBits(int bd){return setStopBits(QSerialPort::StopBits(bd));}

    QSerialPort::StopBits stopBits()const;

    bool setParity(QSerialPort::Parity);
    bool setParity(int bd){return setParity(QSerialPort::Parity(bd));}

    QSerialPort::Parity parity()const;

    bool setStation(int);
    int station()const;

};

#endif // RK_SERIALPORT_H
