#ifndef RK_DEVICEMANAGER_H
#define RK_DEVICEMANAGER_H

#include "rk_device.h"
#include <QMap>

#define DEVICEMANAGER RK_DeviceManager::Instance()
class RK_DeviceManager
{
public:
    static RK_DeviceManager& Instance();
    ~RK_DeviceManager();

    bool AddDevice(RK_Device::DeviceType, const QString& name);

    bool RemoveDevice(QString name);

    bool RenameDevice(QString oldd,QString newd);

    QStringList AllDeviceName()const;

    template<typename T=RK_Device>
    T* GetDevice(const QString& name)const
    {
        auto ret = GetDeviceImpl(name);
        return dynamic_cast<T*>(ret);
    }

private:
    RK_DeviceManager() = default;
    RK_Device* GetDeviceImpl(const QString&)const;
    QMap<QString,RK_Device*> m_device;
};

#endif // RK_DEVICEMANAGER_H
