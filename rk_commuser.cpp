#include "rk_commuser.h"
#include "rk_savehelper.h"

#include "rk_devicemanager.h"
#include "rk_device_p.h"

#include "qg_netportsettings.h"
#include "qg_serialportsettings.h"

void RK_CommUser::OpenCommSettingUI(int type)
{
    switch (type) {
    case 0:
    {
        QG_SerialPortSettings set;
        set.exec();
        break;
    }
    case 1:
    {
        QG_NetPortSettings set;
        set.exec();
        break;
    }
    default:
        break;
    }
}

RK_SerialPort* RK_CommUser::GetDefaultSerialPort()
{
    return DEVICEMANAGER.GetDevice<RK_SerialPort>(DSPD);
}

RK_TCPClient* RK_CommUser::GetDefaultNetClient()
{
    return DEVICEMANAGER.GetDevice<RK_TCPClient>(DNCD);
}

RK_TCPServer* RK_CommUser::GetDefaultNetServer()
{
    return DEVICEMANAGER.GetDevice<RK_TCPServer>(DNSD);
}

RK_ServerParam RK_CommUser::GetNetServerParam()
{
    RK_ServerParam param;
    RK_SaveHelper().loadServer(false,param);
    return param;
}

RK_ServerParam RK_CommUser::GetSerialServerParam()
{
    RK_ServerParam param;
    RK_SaveHelper().loadServer(true,param);
    return param;
}

QByteArray RK_CommUser::ReadResponseMsg(RK_Device::DeviceType tp,QString inqMsg,int msec,bool *sem)
{
    QString id;
    switch(tp)
    {
    case RK_Device::e_serial:
        id = DSPD;
        break;
    case RK_Device::e_tcpClient:
        id = DNCD;
        break;
    case RK_Device::e_tcpServer:
        id = DNSD;
        break;
    default:
        break;
    }
    auto device = DEVICEMANAGER.GetDevice(id);
    if(device&&device->isOpen())
    {
        if(device->send(inqMsg))
        {
            if(sem)
                return device->readWaitNextData(msec);
            else
                return device->readWaitNextDataSem(msec,sem);
        }
    }
    return "";
}
