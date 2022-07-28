
#include <QCoreApplication>
#include <QApplication>
#include <rk_commuser.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("drcs");
    QCoreApplication::setOrganizationName("Ours1984");

    RK_CommUser().OpenCommSettingUI(0);
    RK_CommUser().OpenCommSettingUI(1);

    return 0;
}
