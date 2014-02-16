#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QCursor>
#include <QSettings>
#include <QGraphicsDropShadowEffect>
#include "CustomWindow.h"
#include "ui_CustomWindow.h"

CustomWindow::CustomWindow(QWidget *parent) : QWidget(parent), ui(new Ui::CustomWindow)
{
    customState = false;

    sliderWidget = new SliderWidget();

    connect(sliderWidget, SIGNAL(customWindowEnable(bool)), this, SLOT(changeState(bool)));
    connect(this, SIGNAL(setMaxPosition()), sliderWidget, SLOT(getMaxScreen()));

    ui->setupUi(this);

    ui->widget->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->titleBar->setVisible(false);

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    setMouseTracking(true);
    ui->titleBar->setMouseTracking(true);
    ui->LTitle->setMouseTracking(true);
    ui->tbMenu->setMouseTracking(true);
    ui->pbMin->setMouseTracking(true);
    ui->pbMax->setMouseTracking(true);
    ui->pbClose->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);

    centralLayout = new QHBoxLayout(ui->centralWidget);
    centralLayout->setContentsMargins(9,9,9,9);

    addAction(ui->actionClose);

    connect(ui->pbMin, SIGNAL(clicked()), this, SLOT(minimizeBtnClicked()));
    connect(ui->pbMax, SIGNAL(clicked()), this, SLOT(maximizeBtnClicked()));
    connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()));

    m_titleMode = FullTitle;
    moveWidget = false;
    inResizeZone = false;
    allowToResize = false;
    resizeVerSup = false;
    resizeHorEsq = false;
    resizeDiagSupEsq = false;
    resizeDiagSupDer = false;

    QGraphicsDropShadowEffect *bodyShadow = new QGraphicsDropShadowEffect;
    bodyShadow->setBlurRadius(12.0);
    bodyShadow->setColor(QColor(0, 0, 0, 80));
    bodyShadow->setOffset(0, 0);
    ui->widget->setGraphicsEffect(bodyShadow);

    QSettings settings("dialogs.ini", QSettings::IniFormat);
    QRect geo = settings.value("geometry").toRect();

    if (geo.height() > 0 and geo.x() < QApplication::desktop()->width() and geo.width() > 0 and geo.y() < QApplication::desktop()->height())
    {
        currentGeometry = geo;
        setGeometry(geo);
    }

    currentGeometry = this->geometry();

    if (settings.value("maximized").toBool())
    {
        showMaximized();
        ui->pbMax->setIcon(QIcon(":/images/images/restore.png"));
    }

    setCentralWidget(sliderWidget, "Custom Window");
}

CustomWindow::~CustomWindow()
{
    QSettings settings("dialogs.ini", QSettings::IniFormat);
    settings.setValue("geometry", geometry());
    settings.setValue("maximized", isMaximized());

    delete sliderWidget;
    delete ui;
    delete centralLayout;
}

void CustomWindow::changeState(bool st)
{
    customState = st;

    if (!customState)
    {
        ui->titleBar->setVisible(false);
        ui->widget->setStyleSheet("background-color: rgb(255, 255, 255);");

        Qt::WindowFlags flags = windowFlags();
        flags &= ~Qt::FramelessWindowHint;
        setWindowFlags(flags);

        flags &= ~Qt::CustomizeWindowHint;
        setWindowFlags(flags);

        currentGeometry = this->geometry();
        emit setMaxPosition();
        this->setGeometry(currentGeometry);
        show();
        update();
    }
    else
    {
        ui->widget->setStyleSheet("");
        ui->titleBar->setVisible(true);

        setWindowFlags(Qt::CustomizeWindowHint);
        setWindowFlags(Qt::FramelessWindowHint);

        emit setMaxPosition();
        currentGeometry = this->geometry();
        this->setGeometry(currentGeometry);
        show();
        update();
    }
}

void CustomWindow::mouseMoveEvent(QMouseEvent *e)
{
    int xMouse = e->pos().x();
    int yMouse = e->pos().y();
    int wWidth = geometry().width();
    int wHeight = geometry().height();

    if (moveWidget)
    {
        inResizeZone = false;
        moveWindow(e);
    }
    else if (allowToResize)
        resizeWindow(e);

    else if (xMouse >= wWidth - PIXELS_TO_ACT or allowToResize)
    {
        inResizeZone = true;

        if (yMouse >= wHeight - PIXELS_TO_ACT)
            setCursor(Qt::SizeFDiagCursor);
        else if (yMouse <= PIXELS_TO_ACT)
            setCursor(Qt::SizeBDiagCursor);
        else
            setCursor(Qt::SizeHorCursor);

        resizeWindow(e);
    }

    else if (xMouse <= PIXELS_TO_ACT or allowToResize)
    {
        inResizeZone = true;

        if (yMouse >= wHeight - PIXELS_TO_ACT)
            setCursor(Qt::SizeBDiagCursor);
        else if (yMouse <= PIXELS_TO_ACT)
            setCursor(Qt::SizeFDiagCursor);
        else
            setCursor(Qt::SizeHorCursor);

        resizeWindow(e);
    }

    else if ((yMouse >= wHeight - PIXELS_TO_ACT) or allowToResize)
    {
        inResizeZone = true;
        setCursor(Qt::SizeVerCursor);

        resizeWindow(e);
    }

    else if (yMouse <= PIXELS_TO_ACT or allowToResize)
    {
        inResizeZone = true;
        setCursor(Qt::SizeVerCursor);

        resizeWindow(e);
    }
    else
    {
        inResizeZone = false;
        setCursor(Qt::ArrowCursor);
    }

    e->accept();
}

void CustomWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (inResizeZone)
        {
            allowToResize = true;

            if (e->pos().y() <= PIXELS_TO_ACT)
            {
                if (e->pos().x() <= PIXELS_TO_ACT)
                    resizeDiagSupEsq = true;
                else if (e->pos().x() >= geometry().width() - PIXELS_TO_ACT)
                    resizeDiagSupDer = true;
                else
                    resizeVerSup = true;
            }
            else if (e->pos().x() <= PIXELS_TO_ACT)
                resizeHorEsq = true;
        }
        else if (e->pos().x() >= PIXELS_TO_ACT and e->pos().x() < ui->titleBar->geometry().width()
                 and e->pos().y() >= PIXELS_TO_ACT and e->pos().y() < ui->titleBar->geometry().height())
        {
            moveWidget = true;
            dragPosition = e->globalPos() - frameGeometry().topLeft();
        }
    }

    e->accept();
}

void CustomWindow::mouseReleaseEvent(QMouseEvent *e)
{
    moveWidget = false;
    allowToResize = false;
    resizeVerSup = false;
    resizeHorEsq = false;
    resizeDiagSupEsq = false;
    resizeDiagSupDer = false;

    e->accept();
}

void CustomWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->pos().x() < ui->tbMenu->geometry().right() and e->pos().y() < ui->tbMenu->geometry().bottom()
        and e->pos().x() >=  ui->tbMenu->geometry().x() and e->pos().y() >= ui->tbMenu->geometry().y()
        and ui->tbMenu->isVisible())
        close();
    else if (e->pos().x() < ui->titleBar->geometry().width()
             and e->pos().y() < ui->titleBar->geometry().height()
             and m_titleMode != FullScreenMode)
        maximizeBtnClicked();
    e->accept();
}

void CustomWindow::paintEvent (QPaintEvent *)
{
    QStyleOption opt;
    opt.init (this);
    QPainter p(this);
    style()->drawPrimitive (QStyle::PE_Widget, &opt, &p, this);
}

void CustomWindow::moveWindow(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        move(e->globalPos() - dragPosition);
        e->accept();
    }
}

void CustomWindow::resizeWindow(QMouseEvent *e)
{
    if (allowToResize)
    {
        int xMouse = e->pos().x();
        int yMouse = e->pos().y();
        int wWidth = geometry().width();
        int wHeight = geometry().height();

        if (cursor().shape() == Qt::SizeVerCursor)
        {
            if (resizeVerSup)
            {
                int newY = geometry().y() + yMouse;
                int newHeight = wHeight - yMouse;

                if (newHeight > minimumSizeHint().height())
                {
                    resize(wWidth, newHeight);
                    move(geometry().x(), newY);
                }
            }
            else
                resize(wWidth, yMouse+1);
        }
        else if (cursor().shape() == Qt::SizeHorCursor)
        {
            if (resizeHorEsq)
            {
                int newX = geometry().x() + xMouse;
                int newWidth = wWidth - xMouse;

                if (newWidth > minimumSizeHint().width())
                {
                    resize(newWidth, wHeight);
                    move(newX, geometry().y());
                }
            }
            else
                resize(xMouse, wHeight);
        }
        else if (cursor().shape() == Qt::SizeBDiagCursor)
        {
            int newX = 0;
            int newWidth = 0;
            int newY = 0;
            int newHeight = 0;

            if (resizeDiagSupDer)
            {
                newX = geometry().x();
                newWidth = xMouse;
                newY = geometry().y() + yMouse;
                newHeight = wHeight - yMouse;
            }
            else
            {
                newX = geometry().x() + xMouse;
                newWidth = wWidth - xMouse;
                newY = geometry().y();
                newHeight = yMouse;
            }

            if (newWidth >= minimumSizeHint().width() and newHeight >= minimumSizeHint().height())
            {
                resize(newWidth, newHeight);
                move(newX, newY);
            }
            else if (newWidth >= minimumSizeHint().width())
            {
                resize(newWidth, wHeight);
                move(newX, geometry().y());
            }
            else if (newHeight >= minimumSizeHint().height())
            {
                resize(wWidth, newHeight);
                move(geometry().x(), newY);
            }
        }
        else if (cursor().shape() == Qt::SizeFDiagCursor)
        {
            if (resizeDiagSupEsq)
            {
                int newX = geometry().x() + xMouse;
                int newWidth = wWidth - xMouse;
                int newY = geometry().y() + yMouse;
                int newHeight = wHeight - yMouse;

                if (newWidth >= minimumSizeHint().width() and newHeight >= minimumSizeHint().height())
                {
                    resize(newWidth, newHeight);
                    move(newX, newY);
                }
                else if (newWidth >= minimumSizeHint().width())
                {
                    resize(newWidth, wHeight);
                    move(newX, geometry().y());
                }
                else if (newHeight >= minimumSizeHint().height())
                {
                    resize(wWidth, newHeight);
                    move(geometry().x(), newY);
                }
            }
            else
                resize(xMouse+1, yMouse+1);
        }

        e->accept();
    }
}

void CustomWindow::setCentralWidget(QWidget *widget, const QString &widgetName)
{
    centralLayout->addWidget(widget);
    ui->centralWidget->setLayout(centralLayout);
    ui->LTitle->setText(widgetName);
}

void CustomWindow::setTitlebarMode(const TitleMode &flag)
{
    m_titleMode = flag;

    switch (m_titleMode)
    {
        case CleanTitle:
            ui->tbMenu->setHidden(true);
            ui->pbMin->setHidden(true);
            ui->pbMax->setHidden(true);
            ui->pbClose->setHidden(true);
            break;
        case OnlyCloseButton:
            ui->tbMenu->setHidden(true);
            ui->pbMin->setHidden(true);
            ui->pbMax->setHidden(true);
            break;
        case MenuOff:
            ui->tbMenu->setHidden(true);
            break;
        case MaxMinOff:
            ui->pbMin->setHidden(true);
            ui->pbMax->setHidden(true);
            break;
        case FullScreenMode:
            ui->pbMax->setHidden(true);
            showMaximized();
            break;
        case MaximizeModeOff:
            ui->pbMax->setHidden(true);
            break;
        case MinimizeModeOff:
            ui->pbMin->setHidden(true);
            break;
        case FullTitle:
            ui->tbMenu->setVisible(true);
            ui->pbMin->setVisible(true);
            ui->pbMax->setVisible(true);
            ui->pbClose->setVisible(true);
            break;
            break;
        default:
            ui->tbMenu->setVisible(true);
            ui->pbMin->setVisible(true);
            ui->pbMax->setVisible(true);
            ui->pbClose->setVisible(true);
            break;
    }
    ui->LTitle->setVisible(true);
}

void CustomWindow::setTitlebarMenu(QMenu *menu, const QString &icon)
{
    ui->tbMenu->setMenu(menu);
    ui->tbMenu->setIcon(QIcon(icon));
}

void CustomWindow::maximizeBtnClicked()
{
    if (isFullScreen() or isMaximized())
    {
        ui->pbMax->setIcon(QIcon(":/images/images/maximize.png"));
        setWindowState(windowState() & ~Qt::WindowFullScreen & ~Qt::WindowMaximized);
        emit setMaxPosition();
    }
    else
    {
        ui->pbMax->setIcon(QIcon(":/images/images/restore.png"));
        setWindowState(windowState() | Qt::WindowFullScreen | Qt::WindowMaximized);
        emit setMaxPosition();
    }
}

void CustomWindow::minimizeBtnClicked()
{
    if (isMinimized())
    {
        setWindowState(windowState() & ~Qt::WindowMinimized);
    }
    else
    {
        setWindowState(windowState() | Qt::WindowMinimized);
    }
}
