#include <QApplication>
#include "aisparsermainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("ICODE-MDA");
    QCoreApplication::setApplicationName("AisParserApp");

    QApplication a(argc, argv);
    AisParserMainWindow w;
    w.show();
    
    return a.exec();
}
