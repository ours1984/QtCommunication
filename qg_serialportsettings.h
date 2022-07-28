#ifndef QG_SERIALPORTSETTINGS_H
#define QG_SERIALPORTSETTINGS_H

#include <QDialog>

namespace Ui {
class QG_SerialPortSettings;
}
class QAbstractButton;
class RK_CommUser;

class QG_SerialPortSettings : public QDialog
{
    friend class RK_CommUser;
    Q_OBJECT

public:

    ~QG_SerialPortSettings();

private slots:
    void on_pb_open_clicked();

    void on_pb_close_clicked();

    void on_pb_clr_clicked();

    void on_pb_send_clicked();

    void on_gb_debug_toggled(bool check);

    void on_dbb_over_clicked(QAbstractButton *button);

    void on_QG_SerialPortSettings_finished(int result);


    void _reciveData();

    void _updateState();

private:
    explicit QG_SerialPortSettings(QWidget *parent = nullptr);
    void _initData();
    void _saveData();

private:
    Ui::QG_SerialPortSettings *ui;

};

#endif // QG_SERIALPORTSETTINGS_H
