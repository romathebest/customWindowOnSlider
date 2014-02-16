#include <QApplication>
#include "customwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CustomWindow* customWindow = new CustomWindow();
    customWindow->show();

    return a.exec();
}
