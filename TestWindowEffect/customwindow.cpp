#include "customwindow.h"
#include "ui_customwindow.h"

CustomWindow::CustomWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomWindow)
{
    ui->setupUi(this);
}

CustomWindow::~CustomWindow()
{
    delete ui;
}
