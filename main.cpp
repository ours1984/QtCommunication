
#include <QCoreApplication>
#include <QApplication>
#include <rk_commuser.h>

#include "qg_serialportsettings.h"
#include "qg_netportsettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("drcs");
    QCoreApplication::setOrganizationName("Ours1984");

    QG_SerialPortSettings ddd(nullptr);
    ddd.exec();

    QG_NetPortSettings dd(nullptr);
    dd.exec();
    return 0;
}
