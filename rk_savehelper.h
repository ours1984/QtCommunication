#ifndef RK_SAVEHELPER_H
#define RK_SAVEHELPER_H

#include "rk_commuser.h"
#include <QSettings>
class RK_SerialPort;
class RK_UdpNet;
class RK_TCPClient;
class RK_TCPServer;

class RK_SaveHelper
{
public:

    RK_SaveHelper();

    void saveDevice();

    void loadDevice();

    bool loadServer(bool isSerial,RK_ServerParam& param);

    void saveServer(bool isSerial,const RK_ServerParam& param);

private:

    void _saveSerialPort(RK_SerialPort* sp,QString name);
    void _loadSerialPort(RK_SerialPort*,QString name);

    void _saveUdpnet(RK_UdpNet*,QString name);
    void _loadUdpnet(RK_UdpNet*,QString name);

    void _saveTcpClient(RK_TCPClient*,QString name);
    void _loadTcpClient(RK_TCPClient*,QString name);

    void _saveTcpServer(RK_TCPServer*,QString name);
    void _loadTcpServer(RK_TCPServer*,QString name);

    QSettings m_set;
};

#endif // RK_SAVEHELPER_H
