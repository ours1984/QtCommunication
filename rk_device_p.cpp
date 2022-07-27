#include "rk_device_p.h"

RK_DevicePrivate::~RK_DevicePrivate()
{
    if(device)
        delete device;
}

RK_DevicePrivate::RK_DevicePrivate(RK_Device::DeviceType tp)
    :type(tp)
{

}
