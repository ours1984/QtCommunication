#include "qg_netportsettings.h"
#include "ui_qg_netportsettings.h"

#include "rk_devicemanager.h"
#include "rk_device_p.h"
#include "rk_commuser.h"
#include "rk_tcpserver.h"
#include "rk_tcpclient.h"
#include "rk_savehelper.h"

#include <QMessageBox>
#include <QDateTime>
#include <QButtonGroup>
#include <QHostInfo>
#include <QTcpSocket>

QG_NetPortSettings::QG_NetPortSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QG_NetPortSettings)
{
    ui->setupUi(this);
    QButtonGroup* gp = new QButtonGroup(this);
    gp->setExclusive(true);
    gp->addButton(ui->rb_client);
    gp->addButton(ui->rb_server);

    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            if(QHostAddress::LocalHost != address)
            {
                ui->cb_sip->addItem(address.toString());
                break;
            }
        }
    }

    _initData();

    connect(gp,SIGNAL(buttonClicked(QAbstractButton *)),this,SLOT(_updateState()));
    connect(ui->cb_sign,&QComboBox::currentTextChanged,this,&QG_NetPortSettings::_updateState);
    connect(ui->cb_finish,&QComboBox::currentTextChanged,this,&QG_NetPortSettings::_updateState);
    connect(ui->cb_err,&QComboBox::currentTextChanged,this,&QG_NetPortSettings::_updateState);
    _updateState();
}

QG_NetPortSettings::~QG_NetPortSettings()
{
    delete ui;
}

void QG_NetPortSettings::on_pb_open_clicked()
{
    RK_Device *dev =_getDevice(true);
    auto err = _saveData();
    if(!err.isEmpty())
    {
        QMessageBox::critical(this,"错误",err);
        return;
    }

    if(dev&&!dev->open())
    {
        QMessageBox::critical(this,"错误","网口打开失败！");
    }
    else
    {
        connect(dev,&RK_Device::connectChanged,this,&QG_NetPortSettings::_updateState);
    }
    _updateState();
}

void QG_NetPortSettings::on_pb_close_clicked()
{
    RK_Device *dev =_getDevice();
    if(dev&&dev->isOpen())
    {
        dev->close();
        ui->lw_client->clear();
    }
    _updateState();
}

void QG_NetPortSettings::on_pb_send_clicked()
{
    RK_Device *dev =_getDevice();
    if(dev)
    {
        auto msg = ui->le_cmd->text();
        if(!msg.isEmpty())
        {
            if(ui->rb_ascii->isChecked())
                dev->send(msg);
            else
                dev->send(QByteArray::fromHex(msg.toUtf8()));
        }
    }
}

void QG_NetPortSettings::on_pb_clr_clicked()
{
    ui->tb_rec->clear();
}

void QG_NetPortSettings::on_gb_debug_toggled(bool check)
{
    auto sp = _getDevice();
    if(check)
    {
        if(sp&&sp->isOpen())
        {
            connect(sp,&RK_Device::readReady,this,&QG_NetPortSettings::_reciveData);
        }
        else
        {
            QMessageBox::critical(this,"错误","网口非打开状态！");
            ui->gb_debug->blockSignals(true);
            ui->gb_debug->setChecked(false);
            ui->gb_debug->blockSignals(false);
        }
    }
    else
    {
        disconnect(sp,&RK_Device::readReady,this,&QG_NetPortSettings::_reciveData);
    }
}

void QG_NetPortSettings::on_dbb_over_clicked(QAbstractButton *button)
{
    if(button == ui->dbb_over->button(QDialogButtonBox::Cancel))
    {
        return;
    }

    _saveData();
}

void QG_NetPortSettings::on_lw_client_currentTextChanged(const QString &currentText)
{
    auto dev = _getDevice();
    auto tcps = qobject_cast<RK_TCPServer *>(dev);
    if(tcps)
    {
        tcps->setCurrentClient(currentText);
    }
}

void QG_NetPortSettings::on_QG_NetPortSettings_finished(int)
{
    auto sp = RK_CommUser().GetDefaultNetClient();
    if(sp)
        disconnect(sp,&RK_Device::readReady,this,&QG_NetPortSettings::_reciveData);

    ui->gb_debug->blockSignals(true);
    ui->gb_debug->setChecked(false);
    ui->gb_debug->blockSignals(false);
}

void QG_NetPortSettings::_reciveData(QObject*sok)
{   
    RK_Device *dev = qobject_cast<RK_Device *>(sender());
    if(dev)
    {
        auto data = dev->readAllCurrentData();
        if(data.isEmpty())
            return;

        QString msg ="时间：";
        msg.append(QDateTime::currentDateTime().toString ("hh:mm:ss"));
        msg.append("\n");

        if(ui->rb_server->isChecked())
        {
            auto tcp = qobject_cast<QTcpSocket*>(sok);
            if(tcp)
            {
                msg += "客户端：";
                msg += tcp->peerAddress().toString();
                msg.append("\n");
            }
        }

        msg += "Ascii：";
        msg += data;
        msg.append("\n");

        msg += "Hex：";
        msg += data.toHex();
        msg.append("\n");

        ui->tb_rec->append(msg);
        ui->tb_rec->moveCursor(QTextCursor::End);

        if(ui->cb_rep->isChecked())
        {
            auto msg = ui->le_repmsg->text();
            if(!msg.isEmpty())
                dev->send(msg);
        }
    }
}

void QG_NetPortSettings::_updateState()
{
    RK_Device *dev =_getDevice();
    if(dev&&dev->isOpen())
    {       
        ui->gb_debug->setEnabled(true);
        ui->lay_param->setEnabled(false);
        ui->pb_open->setEnabled(false);
        ui->pb_close->setEnabled(true);
        ui->lay_mode->setEnabled(false);
    }
    else
    {
        ui->gb_debug->blockSignals(true);
        ui->gb_debug->setChecked(false);
        ui->gb_debug->blockSignals(false);

        ui->gb_debug->setEnabled(false);
        ui->lay_param->setEnabled(true);
        ui->pb_open->setEnabled(true);
        ui->pb_close->setEnabled(false);
        ui->lay_mode->setEnabled(true);
    }

    if(ui->rb_client->isChecked())
    {
        ui->lay_server->setEnabled(false);
        ui->lw_client->hide();
        ui->lay_param->setCurrentIndex(0);
    }
    else
    {
        if(dev&&dev->isOpen())
            ui->lay_server->setEnabled(false);
        else
            ui->lay_server->setEnabled(true);

        ui->lw_client->show();
        ui->lay_param->setCurrentIndex(1);

        auto sp = qobject_cast<RK_TCPServer*>(dev);
        if(sp)
        {
            ui->lw_client->clear();
            ui->lw_client->addItems(sp->getClients());
            auto ddd = ui->lw_client->findItems(sp->currentClinet(),Qt::MatchExactly);
            if(!ddd.empty())
                ui->lw_client->setCurrentItem(ddd.front());
        }
    }

    ui->le_sigh->setEnabled(ui->cb_sign->currentIndex()==RK_ServerParam::e_sfe_userdefine);
    ui->le_err->setEnabled(ui->cb_err->currentIndex()==RK_ServerParam::e_sfe_userdefine);
    ui->le_finish->setEnabled(ui->cb_finish->currentIndex()==RK_ServerParam::e_sfe_userdefine);
}

void QG_NetPortSettings::_initData()
{
    //init server data
    RK_ServerParam param;
    RK_SaveHelper().loadServer(false,param);
    if(param.isClientMode)
    {
        ui->rb_client->setChecked(true);
    }
    else
    {
        ui->rb_server->setChecked(true);
    }
    ui->cb_sign->setCurrentIndex(param.action_sign);
    ui->cb_finish->setCurrentIndex(param.action_finish);
    ui->cb_err->setCurrentIndex(param.action_err);
    ui->le_sigh->setText(param.user_sign);
    ui->le_finish->setText(param.user_finish);
    ui->le_err->setText(param.user_err);

    //init port data
    RK_SaveHelper().loadDevice();
    RK_Device *dev =_getDevice();
    if(nullptr != dev)
    {
        if(ui->rb_client->isChecked())
        {
            auto sp = dynamic_cast<RK_TCPClient*>(dev);
            if(sp)
            {
                ui->le_cport->setText(QString::number(sp->dstPort()));
                ui->le_cip->setText(sp->dstAddr());
            }
        }
        else
        {
            auto sp = qobject_cast<RK_TCPServer*>(dev);
            if(sp)
            {
                ui->le_sport->setText(QString::number(sp->listenPort()));
            }
        }
    }
}

QString QG_NetPortSettings::_saveData()
{
    QHostAddress test;
    //save port data
    if(ui->rb_client->isChecked())
    {
        if (!test.setAddress(ui->le_cip->text()))
        {
            return "IP地址非法！";
        }

        auto sp = RK_CommUser().GetDefaultNetClient();
        if(sp)
        {
            if(!sp->setDstAddr(ui->le_cip->text()))
                ui->le_cip->setText(sp->dstAddr());
            sp->setDstPort(ui->le_cport->text().toInt());
        }
    }
    else
    {
        auto sp = RK_CommUser().GetDefaultNetServer();
        if(sp)
        {
            sp->setListenPort(ui->le_sport->text().toInt());
        }
    }
    RK_SaveHelper().saveDevice();

    //save server data
    RK_ServerParam param;
    param.isClientMode=ui->rb_client->isChecked();
    param.protocol=(decltype(param.protocol))ui->cb_proto->currentIndex();
    param.encode=(decltype(param.encode))ui->cb_encode->currentIndex();
    param.action_sign=(decltype(param.action_sign))ui->cb_sign->currentIndex();
    param.action_finish=(decltype(param.action_finish))ui->cb_finish->currentIndex();
    param.action_err=(decltype(param.action_err))ui->cb_err->currentIndex();
    param.user_finish=ui->le_finish->text();
    param.user_err=ui->le_err->text();
    param.user_sign=ui->le_sigh->text();
    RK_SaveHelper().saveServer(false,param);

    return "";
}

RK_Device* QG_NetPortSettings::_getDevice(bool create)
{
    RK_Device *sp =nullptr;
    if(ui->rb_client->isChecked())
    {
        sp = RK_CommUser().GetDefaultNetClient();
        if(create&&nullptr == sp)
        {
            DEVICEMANAGER.AddDevice(RK_Device::e_tcpClient,DNCD);
            sp = RK_CommUser().GetDefaultNetClient();
        }
    }
    else
    {
        sp = RK_CommUser().GetDefaultNetServer();
        if(create&&nullptr == sp)
        {
            DEVICEMANAGER.AddDevice(RK_Device::e_tcpServer,DNSD);
            sp = RK_CommUser().GetDefaultNetServer();
        }
    }
    return sp;
}
