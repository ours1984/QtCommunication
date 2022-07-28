#include "rk_savehelper.h"
#include "rk_serialport.h"
#include "rk_udpnet.h"
#include "rk_tcpclient.h"
#include "rk_tcpserver.h"
#include "rk_devicemanager.h"
#include <QCoreApplication>
#include <map>

#define PORT_NAME "PortName"
#define BAUT_RATE "BautRate"
#define DATA_BITS "DataBits"
#define STOP_BITS "StopBits"
#define PARITY_TYPE "Parity"
#define STATION "Station"

#define SRC_PORT "SrcPort"
#define DST_PORT "DstPort"
#define DST_ADDR "DstAddr"

static std::map<RK_Device::DeviceType,QString> s_d2s={
    {RK_Device::e_serial,"SerialPort"},
    {RK_Device::e_tcpClient,"TcpCline"},
    {RK_Device::e_tcpServer,"TcpServer"},
    {RK_Device::e_updNet,"UdpNet"}
};
static std::map<QString,RK_Device::DeviceType> s_s2d={
    {"SerialPort",RK_Device::e_serial},
    {"TcpCline",RK_Device::e_tcpClient},
    {"TcpServer",RK_Device::e_tcpServer},
    {"UdpNet",RK_Device::e_updNet}
};

#define RK_COMMUNICATION "Communication"

class GroupGaud
{
public:
    GroupGaud(QSettings& set,const QString &group):m_set(set){
        m_set.beginGroup(group);
    }
    ~GroupGaud(){
        m_set.endGroup();
    }

private:
    QSettings& m_set;
};

RK_SaveHelper::RK_SaveHelper()
    :m_set(QCoreApplication::organizationName(),QCoreApplication::applicationName())
{

}

bool RK_SaveHelper::loadServer(bool isSerial,RK_ServerParam& param)
{
    GroupGaud gaud(m_set,isSerial?"SerialServer":"NetServer");
    param.isClientMode = m_set.value("isClientMode",true).toBool();
    param.action_err = (decltype(param.action_err))m_set.value("action_err",RK_ServerParam::e_sfe_default).toInt();
    param.action_sign = (decltype(param.action_sign))m_set.value("action_sign",RK_ServerParam::e_sfe_default).toInt();
    param.action_finish = (decltype(param.action_finish))m_set.value("action_finish",RK_ServerParam::e_sfe_default).toInt();
    param.protocol = (decltype(param.protocol))m_set.value("protocol",RK_ServerParam::e_ptc_char).toInt();
    param.encode = (decltype(param.encode))m_set.value("encode",RK_ServerParam::e_ecd_ascii).toInt();
    param.user_err = m_set.value("user_err","").toString();
    param.user_sign = m_set.value("user_sign","").toString();
    param.user_finish = m_set.value("user_finish","").toString();
    return true;
}

void RK_SaveHelper::saveServer(bool isSerial,const RK_ServerParam& param)
{
    GroupGaud gaud(m_set,isSerial?"SerialServer":"NetServer");
    m_set.setValue("isClientMode",param.isClientMode);
    m_set.setValue("action_err",param.action_err);
    m_set.setValue("action_sign",param.action_sign);
    m_set.setValue("action_finish",param.action_finish);
    m_set.setValue("protocol",param.protocol);
    m_set.setValue("encode",param.encode);
    m_set.setValue("user_sign",param.user_sign);
    m_set.setValue("user_finish",param.user_finish);
    m_set.setValue("user_err",param.user_err);
}

void RK_SaveHelper::saveDevice()
{
    GroupGaud gaud(m_set,RK_COMMUNICATION);
    foreach(auto name,DEVICEMANAGER.AllDeviceName())
    {
        auto dev = DEVICEMANAGER.GetDevice(name);
        auto type = dev->type();
        GroupGaud gaud1(m_set,s_d2s[type]);
        switch(type){
        case RK_Device::e_serial:
            _saveSerialPort((RK_SerialPort*)dev,name);
            break;
        case RK_Device::e_tcpClient:
            _saveTcpClient((RK_TCPClient*)dev,name);
            break;
        case RK_Device::e_tcpServer:
            _saveTcpServer((RK_TCPServer*)dev,name);
            break;
        case RK_Device::e_updNet:
            _saveUdpnet((RK_UdpNet*)dev,name);
            break;
        }
    }
}

void RK_SaveHelper::loadDevice()
{
    GroupGaud gaud(m_set,RK_COMMUNICATION);
    foreach(auto gp,m_set.childGroups())
    {
        GroupGaud gaud1(m_set,gp);
        auto type = s_s2d[gp];
        foreach(auto name,m_set.childGroups())
        {
            bool ret = DEVICEMANAGER.AddDevice(type,name);
            if(ret)
            {
                auto dev = DEVICEMANAGER.GetDevice<>(name);
                switch (type) {
                case RK_Device::e_serial:
                    _loadSerialPort((RK_SerialPort*)dev,name);
                    break;
                case RK_Device::e_tcpClient:
                    _loadTcpClient((RK_TCPClient*)dev,name);
                    break;
                case RK_Device::e_tcpServer:
                    _loadTcpServer((RK_TCPServer*)dev,name);
                    break;
                case RK_Device::e_updNet:
                    _loadUdpnet((RK_UdpNet*)dev,name);
                    break;
                }
            }
        }
    }
}

void RK_SaveHelper::_saveUdpnet(RK_UdpNet* sp,QString name)
{
    GroupGaud gaud(m_set,name);
    m_set.setValue(DST_ADDR,sp->dstAddr());
    m_set.setValue(DST_PORT,sp->dstPort());
    m_set.setValue(SRC_PORT,sp->srcPort());
}

void RK_SaveHelper::_loadUdpnet(RK_UdpNet* ret,QString name)
{
    GroupGaud gaud(m_set,name);
    ret->setDstAddr(m_set.value(DST_ADDR).toString());
    ret->setDstPort(m_set.value(DST_PORT).toInt());
    ret->setSrcPort(m_set.value(SRC_PORT).toInt());
}

void RK_SaveHelper::_saveTcpClient(RK_TCPClient *sp, QString name)
{
    GroupGaud gaud(m_set,name);
    m_set.setValue(DST_ADDR,sp->dstAddr());
    m_set.setValue(DST_PORT,sp->dstPort());
}

void RK_SaveHelper::_loadTcpClient(RK_TCPClient *ret, QString name)
{
    GroupGaud gaud(m_set,name);
    ret->setDstAddr(m_set.value(DST_ADDR).toString());
    ret->setDstPort(m_set.value(DST_PORT).toInt());
}

void RK_SaveHelper::_saveTcpServer(RK_TCPServer *sp, QString name)
{
    GroupGaud gaud(m_set,name);
    m_set.setValue(SRC_PORT,sp->listenPort());
}

void RK_SaveHelper::_loadTcpServer(RK_TCPServer *ret, QString name)
{
    GroupGaud gaud(m_set,name);
    ret->setListenPort(m_set.value(SRC_PORT).toInt());
}


void RK_SaveHelper::_loadSerialPort(RK_SerialPort* ret,QString name)
{
    GroupGaud gaud(m_set,name);
    ret->setPortName(m_set.value(PORT_NAME).toString());
    ret->setBaudRate(m_set.value(BAUT_RATE).toInt());
    ret->setDataBits(m_set.value(DATA_BITS).toInt());
    ret->setStopBits(m_set.value(STOP_BITS).toInt());
    ret->setParity(m_set.value(PARITY_TYPE).toInt());
    ret->setStation(m_set.value(STATION).toInt());
}

void RK_SaveHelper::_saveSerialPort(RK_SerialPort* sp,QString name)
{
    GroupGaud gaud(m_set,name);
    m_set.setValue(PORT_NAME,sp->portName());
    m_set.setValue(BAUT_RATE,sp->baudRate());
    m_set.setValue(DATA_BITS,sp->dataBits());
    m_set.setValue(STOP_BITS,sp->stopBits());
    m_set.setValue(PARITY_TYPE,sp->parity());
    m_set.setValue(STATION,sp->station());
}
