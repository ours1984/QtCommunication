#ifndef QG_NETPORTSETTINGS_H
#define QG_NETPORTSETTINGS_H

#include <QDialog>

namespace Ui {
class QG_NetPortSettings;
}
class QAbstractButton;
class RK_Device;

class QG_NetPortSettings : public QDialog
{
    Q_OBJECT

public:
    explicit QG_NetPortSettings(QWidget *parent = nullptr);
    ~QG_NetPortSettings();

private slots:
    void on_pb_open_clicked();

    void on_pb_close_clicked();

    void on_pb_send_clicked();

    void on_pb_clr_clicked();

    void on_gb_debug_toggled(bool check);

    void on_dbb_over_clicked(QAbstractButton *button);

    void on_QG_NetPortSettings_finished(int result);

    void on_lw_client_currentTextChanged(const QString &currentText);

    void _reciveData(QObject*);

    void _updateState();

private:

    void _initData();
    void _saveData();
    RK_Device* _getDevice(bool create=false);

private:
    Ui::QG_NetPortSettings *ui;
};

#endif // QG_NETPORTSETTINGS_H
