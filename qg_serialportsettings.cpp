#include "qg_serialportsettings.h"
#include "ui_qg_serialportsettings.h"

#include "rk_devicemanager.h"
#include "rk_device_p.h"
#include "rk_commuser.h"
#include "rk_savehelper.h"

#include <QMessageBox>
#include <QDateTime>
#include <QButtonGroup>
#include <QSerialPortInfo>

QG_SerialPortSettings::QG_SerialPortSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QG_SerialPortSettings)
{
    ui->setupUi(this);
    QButtonGroup* gp = new QButtonGroup(this);
    gp->setExclusive(true);
    gp->addButton(ui->rb_client);
    gp->addButton(ui->rb_server);

    _initData();

    connect(gp,SIGNAL(buttonClicked(QAbstractButton *)),this,SLOT(_updateState()));
    connect(ui->cb_sign,&QComboBox::currentTextChanged,this,&QG_SerialPortSettings::_updateState);
    connect(ui->cb_finish,&QComboBox::currentTextChanged,this,&QG_SerialPortSettings::_updateState);
    connect(ui->cb_err,&QComboBox::currentTextChanged,this,&QG_SerialPortSettings::_updateState);
    //connect(ui->cb_port,&QComboBox::currentTextChanged,this,&QG_SerialPortSettings::_updateState);
    _updateState();
}

QG_SerialPortSettings::~QG_SerialPortSettings()
{
    delete ui;
}

void QG_SerialPortSettings::on_pb_open_clicked()
{
    auto sp = RK_CommUser().GetDefaultSerialPort();
    if(nullptr == sp)
    {
        DEVICEMANAGER.AddDevice(RK_Device::e_serial,DSPD);
        sp = RK_CommUser().GetDefaultSerialPort();
    }

    _saveData();

    if(!sp->open())
    {
        QMessageBox::critical(this,"错误","串口打开失败！");
    }
    _updateState();
}

void QG_SerialPortSettings::on_pb_close_clicked()
{
    auto sp = RK_CommUser().GetDefaultSerialPort();
    if(sp&&sp->isOpen())
    {
        sp->close();
    }
    _updateState();
}

void QG_SerialPortSettings::on_pb_send_clicked()
{
    auto dev = RK_CommUser().GetDefaultSerialPort();
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

void QG_SerialPortSettings::on_gb_debug_toggled(bool check)
{
    auto sp = RK_CommUser().GetDefaultSerialPort();
    if(check)
    {
        if(sp&&sp->isOpen())
        {
            connect(sp,&RK_SerialPort::readReady,this,&QG_SerialPortSettings::_reciveData);
        }
        else
        {
            QMessageBox::critical(this,"错误","串口非打开状态！");
            ui->gb_debug->blockSignals(true);
            ui->gb_debug->setChecked(false);
            ui->gb_debug->blockSignals(false);
        }
    }
    else
    {
        disconnect(sp,&RK_SerialPort::readReady,this,&QG_SerialPortSettings::_reciveData);
    }
}

void QG_SerialPortSettings::on_dbb_over_clicked(QAbstractButton *button)
{
    if(button == ui->dbb_over->button(QDialogButtonBox::Cancel))
    {
        return;
    }

    _saveData();
}

void QG_SerialPortSettings::on_QG_SerialPortSettings_finished(int result)
{
    auto sp = RK_CommUser().GetDefaultSerialPort();
    if(sp)
        disconnect(sp,&RK_SerialPort::readReady,this,&QG_SerialPortSettings::_reciveData);
}

void QG_SerialPortSettings::on_pb_clr_clicked()
{
    ui->tb_rec->clear();
}

void QG_SerialPortSettings::_reciveData()
{
    RK_Device *dev = qobject_cast<RK_Device *>(sender());
    if(dev)
    {
        QString msg ="时间：";
        msg.append(QDateTime::currentDateTime().toString ("hh:mm:ss"));
        msg.append("\n");

        auto data = dev->readAllCurrentData();
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

void QG_SerialPortSettings::_updateState()
{
    auto sp = RK_CommUser().GetDefaultSerialPort();
    if(sp&&sp->isOpen())
    {
        ui->gb_debug->setEnabled(true);
        ui->lay_param->setEnabled(false);
        ui->pb_open->setEnabled(false);
        ui->pb_close->setEnabled(true);
    }
    else
    {
        ui->gb_debug->setEnabled(false);
        ui->lay_param->setEnabled(true);
        ui->pb_open->setEnabled(true);
        ui->pb_close->setEnabled(false);
    }

    if(ui->rb_client->isChecked())
    {
        ui->lay_server->setEnabled(false);
    }
    else
    {
        ui->lay_server->setEnabled(true);
    }

    ui->le_sigh->setEnabled(ui->cb_sign->currentIndex()==RK_ServerParam::e_sfe_userdefine);
    ui->le_err->setEnabled(ui->cb_err->currentIndex()==RK_ServerParam::e_sfe_userdefine);
    ui->le_finish->setEnabled(ui->cb_finish->currentIndex()==RK_ServerParam::e_sfe_userdefine);
}

void QG_SerialPortSettings::_initData()
{
    //init server data
    RK_ServerParam param;
    RK_SaveHelper().loadServer(true,param);
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
    for(auto &info:QSerialPortInfo::availablePorts()){
        ui->cb_port->addItem(info.portName());
    }
    RK_SaveHelper().loadDevice();
    auto sp = RK_CommUser().GetDefaultSerialPort();
    if(nullptr != sp)
    {
        ui->cb_port->setCurrentText(sp->portName());

        ui->cb_baud->setCurrentText(QString::number(sp->baudRate()));

        ui->cb_data->setCurrentText(QString::number(sp->dataBits()));

        ui->cb_parity->setCurrentIndex(sp->parity()?sp->parity()+1:sp->parity());

        ui->cb_parity->setCurrentIndex(sp->stopBits()-1);

        ui->cb_station->setCurrentIndex(sp->station()+1);
    }
}

void QG_SerialPortSettings::_saveData()
{
    //save port data
    auto sp = RK_CommUser().GetDefaultSerialPort();
    if(nullptr != sp)
    {
        sp->setPortName(ui->cb_port->currentText());
        sp->setBaudRate(ui->cb_baud->currentText().toInt());
        sp->setDataBits(ui->cb_data->currentText().toInt());
        int par = ui->cb_parity->currentIndex();
        sp->setParity(par==0?par:par+1);
        sp->setStopBits(ui->cb_stop->currentIndex()+1);
        sp->setStation(ui->cb_station->currentIndex()-1);
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
    RK_SaveHelper().saveServer(true,param);
}
