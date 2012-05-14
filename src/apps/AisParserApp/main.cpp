#include <QApplication>
#include "aisparsermainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AisParserMainWindow w;
    w.show();
    
    return a.exec();
}
