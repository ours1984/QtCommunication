#include "rk_devicemanager.h"
#include "rk_serialport.h"
#include "rk_udpnet.h"
#include "rk_tcpclient.h"
#include "rk_tcpserver.h"


RK_DeviceManager& RK_DeviceManager::Instance()
{
    static RK_DeviceManager s_ins;
    return s_ins;
}

RK_DeviceManager::~RK_DeviceManager()
{
    for(auto &dev:m_device){
        delete dev;
    }
    m_device.clear();
}

QStringList RK_DeviceManager::AllDeviceName()const
{
    return m_device.keys();
}

RK_Device* RK_DeviceManager::GetDeviceImpl(const QString& name)const
{
    auto it = m_device.find(name);
    if(it != m_device.end())
        return it.value();
    return nullptr;
}

bool RK_DeviceManager::RemoveDevice(QString name)
{
    auto it = m_device.find(name);
    if(it != m_device.end()){
        delete it.value();
        m_device.erase(it);
        return true;
    }
    return false;
}

bool RK_DeviceManager::RenameDevice(QString oldd,QString newd)
{
    auto it = m_device.find(oldd);
    if(it != m_device.end()){
        auto dev = it.value();
        m_device.erase(it);
        m_device[newd] = dev;
        return true;
    }
    return false;
}

bool RK_DeviceManager::AddDevice(RK_Device::DeviceType tp, const QString& name)
{
    auto it = m_device.find(name);
    if(it == m_device.end()){
        RK_Device* ptr = nullptr;
        switch(tp){
        case RK_Device::e_serial:
            ptr = new RK_SerialPort;
            break;
        case RK_Device::e_tcpClient:
            ptr = new RK_TCPClient;
            break;
        case RK_Device::e_tcpServer:
            ptr = new RK_TCPServer;
            break;
        case RK_Device::e_updNet:
            ptr = new RK_UdpNet;
            break;
        }
        m_device[name] = ptr;
        return true;
    }
    return false;
}
