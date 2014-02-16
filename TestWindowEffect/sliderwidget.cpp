#include "sliderwidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QFontDatabase>
#include <QPropertyAnimation>
#include <QTimer>

SliderWidget::SliderWidget(QWidget *parent) :
    QWidget(parent),
    firstRun(true),
    isMaxState(false),
    pos(0),
    isCustomWindow(false),
    animation(new QPropertyAnimation(this))
{
    animation->setTargetObject(this);
    animation->setPropertyName("pos");

    timer = new QTimer;
    timer->setInterval(ANIMATION_TIME);

    connect(timer, SIGNAL(timeout()), this, SLOT(canChangeState()));

    setAutoFillBackground(true);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/font/AlternateGotNo3D.ttf");

    myFont = new QFont(fontDB.families().at(0), 12, QFont::Bold, false);

}

SliderWidget::~SliderWidget()
{
    delete myFont;
    delete timer;
    delete animation;
}

void SliderWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int width = this->width();
    int height = this->height();

    int radius = height / 4;

    centerLeft = QPoint(width / 2 - radius, height / 2);
    centerRight = QPoint(width / 2 + radius, height / 2);
    float scaleFactor = float(width)/63.2;

    if (firstRun)
    {
        pos = centerLeft.x();
        currentColor = COLOR_START;
    }

    myFont->setPointSize(int(scaleFactor));
    painter.setFont(*myFont);
    QString titleDown = "USE SLIDER TO SWITCH WINDOW STATES";
    painter.setPen(QPen(QColor(220, 220, 220)));
    QRect rect(0, 3 * height / 4 + radius / 2, width, QFontMetrics( *myFont ).height());
    painter.drawText( rect, Qt::AlignCenter, titleDown );

    myFont->setPointSize(1.5*int(scaleFactor));
    painter.setFont(*myFont);

    if (!isCustomWindow)
    {
        //draw big ellipse
        painter.setPen(QPen(currentColor));
        painter.setBrush(QBrush(currentColor));

        painter.drawEllipse(centerLeft, radius, radius);
        painter.drawEllipse(centerRight, radius, radius);
        painter.drawRect(centerLeft.x(), centerLeft.y() - radius, radius * 2 , radius * 2);

        //draw small circle inside
        painter.setPen(QPen(Qt::white));
        painter.setBrush(QBrush(Qt::white));
        painter.drawEllipse(QPoint(pos, centerLeft.y()), int(radius - scaleFactor), int(radius - scaleFactor));

        if (animation->state() == QPropertyAnimation::Running)
            return;

        //draw text
        painter.drawText(centerRight, "OFF");
    }
    else
    {
        //draw big ellipse
        painter.setPen(QPen(currentColor));
        painter.setBrush(QBrush(currentColor));

        painter.drawEllipse(centerLeft, radius, radius);
        painter.drawEllipse(centerRight, radius, radius);
        painter.drawRect(centerLeft.x(), centerLeft.y() - radius, radius * 2 , radius * 2);

        //draw small circle inside
        painter.setPen(QPen(Qt::white));
        painter.setBrush(QBrush(Qt::white));
        painter.drawEllipse(QPoint(pos, centerRight.y()), int(radius - scaleFactor), int(radius - scaleFactor));

        if (animation->state() == QPropertyAnimation::Running)
            return;

        //draw text
        painter.drawText(centerLeft, "ON");
    }
}

void SliderWidget::setPosition(int value)
{
    pos = value;

    int diff = centerRight.x() - centerLeft.x();
    float someValue = float(value - centerLeft.x()) / diff;

    int r, g, b;

    if (COLOR_START.red() > COLOR_END.red())
    {
        r = (1-someValue) * (COLOR_START.red() - COLOR_END.red()) + COLOR_END.red();
    }
    else
    {
        r = (1-someValue) * (-COLOR_START.red() + COLOR_END.red()) + COLOR_START.red();
    }

    if (COLOR_START.green() > COLOR_END.green())
    {
        g = (1-someValue) * (COLOR_START.green() - COLOR_END.green()) + COLOR_END.green();
    }
    else
    {
        g = (1-someValue) * (-COLOR_START.green() + COLOR_END.green()) + COLOR_START.green();
    }

    if (COLOR_START.blue() > COLOR_END.blue())
    {
        b = (1-someValue) * (COLOR_START.blue() - COLOR_END.blue()) + COLOR_END.blue();
    }
    else
    {
        b = (1-someValue) * (-COLOR_START.blue() + COLOR_END.blue()) + COLOR_START.blue();
    }

    if (r < 0) r=0;
    if (g < 0) g=0;
    if (b < 0) b=0;

    currentColor = QColor(r, g, b);

    this->repaint();
}

void SliderWidget::getMaxScreen()
{
    if (isCustomWindow)
    {
        pos = centerRight.x();
        currentColor = COLOR_END;
    }
    else
    {
        pos = centerLeft.x();
        currentColor = COLOR_START;
    }
    repaint();
}

void SliderWidget::canChangeState()
{
    timer->stop();
    emit customWindowEnable(isCustomWindow);
}

int SliderWidget::position() const
{
    return pos;
}

void SliderWidget::animate(bool checked)
{
    animation->setDirection(checked ? QPropertyAnimation::Forward : QPropertyAnimation::Backward);
    animation->start();
}

void SliderWidget::mousePressEvent(QMouseEvent *event)
{
    if (animation->state() == QPropertyAnimation::Running)
        return;

    if (event->button() == Qt::LeftButton)
    {
        firstRun = false;
        isCustomWindow = !isCustomWindow;
        if (isCustomWindow)
        {
            animation->setStartValue(centerLeft.x());
            animation->setEndValue(centerRight.x());
        }
        else
        {
            animation->setStartValue(centerRight.x());
            animation->setEndValue(centerLeft.x());
        }
        animation->setDuration(ANIMATION_TIME);
        animation->setEasingCurve(QEasingCurve::InOutExpo);

        animate(true);

        // tigger update on animation finished
        connect(animation, SIGNAL(finished()), this, SLOT(update()));

        timer->start();
    }
    repaint();
}

void SliderWidget::resizeEvent(QResizeEvent *)
{
    if (isCustomWindow)
    {
        pos = centerRight.x();
        currentColor = COLOR_END;
    }
    else
    {
        pos = centerLeft.x();
        currentColor = COLOR_START;
    }
    repaint();
}
