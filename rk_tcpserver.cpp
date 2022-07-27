#include "rk_tcpserver.h"
#include "rk_device_p.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QTcpServer>
#include <set>

class RK_TCPServerPrivate:public RK_DevicePrivate
{
public:
    RK_TCPServerPrivate():RK_DevicePrivate(RK_Device::e_tcpServer){
    }

    QTcpServer server;
    int port = 8080;
    std::set<QTcpSocket*> clients;
    QTcpSocket* current = nullptr;
};

RK_TCPServer::RK_TCPServer()
    :RK_Device(new RK_TCPServerPrivate())
{
    Q_D(RK_TCPServer);
    connect(&d->server,&QTcpServer::newConnection,this,&RK_TCPServer::newConnect);
}

QStringList RK_TCPServer::getClients() const
{
    Q_CD(RK_TCPServer);
    QStringList ret;
    for(auto sk:d->clients){
        ret.push_back(sk->peerAddress().toString());
    }
    return ret;
}

bool RK_TCPServer::setCurrentClient(QString tp)
{
    Q_D(RK_TCPServer);
    for(auto sk:d->clients){
        if(sk->peerAddress().toString() == tp)
        {
            d->current = sk;
            return true;
        }
    }
    return false;
}

QString RK_TCPServer::currentClinet() const
{
    Q_CD(RK_TCPServer);
    if(d->current)
    {
        return d->current->peerAddress().toString();
    }
    return "";
}

bool RK_TCPServer::setListenPort(int port)
{
    Q_D(RK_TCPServer);
    d->port = port;
    return true;
}

int RK_TCPServer::listenPort()const
{
    Q_CD(RK_TCPServer);
    return d->port;
}

void RK_TCPServer::clsConnect()
{
    Q_D(RK_TCPServer);
    auto dev = (QTcpSocket*)sender();
    d->clients.erase(dev);
    if(d->current==dev)
        d->current = nullptr;
}

void RK_TCPServer::newConnect()
{
    Q_D(RK_TCPServer);
    auto sk = d->server.nextPendingConnection();//与客户端通信的套接字
    connect(sk,&QTcpSocket::disconnected,this,&RK_TCPServer::clsConnect);
    d->device = sk;
    RK_Device::open();
    d->device = nullptr;
    d->clients.insert(sk);
    if(nullptr == d->current)
        d->current = sk;
    emit newConnection();
}

bool RK_TCPServer::send(const char *data, int length)
{
    Q_CD(RK_TCPServer);
    if(d->current)
    {
        return d->current->write(data,length) == length;
    }
    return false;
}

bool RK_TCPServer::isOpen() const
{
    Q_CD(RK_TCPServer);
    return d->server.isListening();
}

bool RK_TCPServer::open()
{
    Q_D(RK_TCPServer);
    return d->server.listen(QHostAddress::Any,d->port);
}

void RK_TCPServer::close()
{
    Q_D(RK_TCPServer);
    d->server.close();
    d->clients.clear();
    d->current = nullptr;
}
