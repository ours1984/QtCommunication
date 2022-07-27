#ifndef RK_COMMUSER_H
#define RK_COMMUSER_H

#include <QString>
#include "rk_serialport.h"
#include "rk_tcpclient.h"
#include "rk_tcpserver.h"
#include <string>

//服务器模式参数
struct RK_ServerParam
{
    enum {
        e_ptc_char = 0,
        e_ptc_modbus,
        e_ptc_userdefine
    }protocol=e_ptc_char;

    enum{
        e_ecd_ascii = 0,
        e_ecs_unicode
    }encode=e_ecd_ascii;

    enum{
        e_sfe_none = 0,
        e_sfe_default,
        e_sfe_userdefine,
        e_sfe_sign,//标刻内容
    }action_sign=e_sfe_default
    ,action_finish=e_sfe_default
    ,action_err=e_sfe_default;

    //模式
    bool isClientMode = true;

    //action类型为userdefine时的输出语句
    QString user_sign,user_finish,user_err;
};

class RK_CommUser
{
public:
    /**@brief 获取串口设备
    * - nullptr 若用户没有设置过
    * - RK_SerialPort* 串口设置界面配置好的客户端
    * @see :: RK_SerialPort
    */
    RK_SerialPort* GetDefaultSerialPort();

    /**@brief 获取网络客户端设备
    * @return
    * - nullptr 若用户没有设置过
    * - RK_TCPClient* 网口设置界面配置好的客户端
    * @see :: RK_TCPClient
    */
    RK_TCPClient* GetDefaultNetClient();

    /**@brief 获取网络服务器设备
    * @return
    * - nullptr 若用户没有设置过
    * - RK_TCPServer* 网口设置界面配置好的服务器
    * @see :: RK_SerialPort
    */
    RK_TCPServer* GetDefaultNetServer();

    /**@brief 获取服务器配置
    */
    RK_ServerParam GetNetServerParam();
    RK_ServerParam GetSerialServerParam();

    /**@brief 查询通信数据
    * @param[in] tp 设备类型
    * - RK_Device::e_serial 串口设备
    * - RK_Device::e_tcpClient tcp客户端
    * - RK_Device::e_tcpServer tcp服务器
    * @param[in] inqMsg 查询语句
    * @param[in] msec 等待超时设置.若确认无响应则设置为0，否则会阻塞 msec 毫秒
    * @return 设备应答消息。若msec后还没收到则返回空
    * @warning 确保用户进行过网口/串口设置，并打开设备，否则发送失败
    * @see :: RK_TCPClient,RK_TCPServer,RK_SerialPort
    */
    QByteArray ReadResponseMsg(RK_Device::DeviceType tp,QString inqMsg,int msec = 3000);
};

#endif // RK_COMMUSER_H
