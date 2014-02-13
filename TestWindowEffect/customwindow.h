#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include <QWidget>

namespace Ui {
class CustomWindow;
}

class CustomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWindow(QWidget *parent = 0);
    ~CustomWindow();

private:
    Ui::CustomWindow *ui;
};

#endif // CUSTOMWINDOW_H
