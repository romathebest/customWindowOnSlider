#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    customState(false)
{
    ui->setupUi(this);

    connect(ui->widget, SIGNAL(customWindowEnable(bool)), this, SLOT(changeState(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeState(bool st)
{
    customState = st;


    if (!customState)
    {
        Qt::WindowFlags flags = windowFlags();
        flags &= ~Qt::FramelessWindowHint;
        setWindowFlags(flags);
    }
    else
    {
//        setWindowFlags(Qt::CustomizeWindowHint);
//        setWindowFlags(Qt::FramelessWindowHint);
//        setAttribute(Qt::WA_DeleteOnClose);
//        setMouseTracking(true);
    }

}
