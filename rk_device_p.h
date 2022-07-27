#ifndef RK_DEVICE_P_H
#define RK_DEVICE_P_H

#include<QIODevice>
#include"rk_device.h"

#define RK_DD(Class) Class * const dd = static_cast<Class *>(d_ptr->device)
#define Q_CD(Class) const Class##Private * const d = d_func()
#define DSPD "DefaultSirialPort"
#define DNCD "DefaultNetCline"
#define DNSD "DefaultNetServer"

class RK_DevicePrivate
{
public:
    RK_DevicePrivate(RK_Device::DeviceType);
    virtual ~RK_DevicePrivate();

    QIODevice* device = nullptr;

    QByteArray revData;
    int revCount = 0;

    RK_Device::DeviceType type;
};

#endif // RK_DEVICE_P_H
