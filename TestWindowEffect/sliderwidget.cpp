#include "sliderwidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QFontDatabase>
#include <QPropertyAnimation>

#include <QDebug>

SliderWidget::SliderWidget(QWidget *parent) :
    QWidget(parent),
    firstRun(true),
    pos(0),
    isCustomWindow(false),
    animation(new QPropertyAnimation(this))
{
    animation->setTargetObject(this);
    animation->setPropertyName("pos");

    setAutoFillBackground(true);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/font/AlternateGotNo3D.ttf");

    myFont = new QFont(fontDB.families().at(0), 12, QFont::Bold, false);

}

SliderWidget::~SliderWidget()
{
    delete myFont;
    delete animation;
}

void SliderWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    int width = this->width();
    int height = this->height();

    int radius = height / 4;

    centerLeft = QPoint(width / 2 - radius, height / 2);
    centerRight = QPoint(width / 2 + radius, height / 2);
    float scaleFactor = float(width)/63.2;

    if (firstRun)
    {
        pos = centerLeft.x();
        currentColor = colorStart;
    }

    if (this->windowState() == Qt::WindowFullScreen)
    {
        if (isCustomWindow)
        {
            pos = centerRight.x();
        }
        else
        {
            pos = centerLeft.x();
        }
    }

    myFont->setPointSize(int(scaleFactor));
    painter.setFont(*myFont);
    QString titleDown = "USE SLIDER TO SWITCH WINDOW STATES";
    painter.setPen(QPen(QColor(220, 220, 220)));
    QRect rect(0, 3 * height / 4 + radius / 2, width, QFontMetrics( *myFont ).height());
    painter.drawText( rect, Qt::AlignCenter, titleDown );

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
        myFont->setPointSize(1.5*int(scaleFactor));
        painter.setFont(*myFont);
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
        myFont->setPointSize(1.5*int(scaleFactor));
        painter.setFont(*myFont);
        painter.drawText(centerLeft, "ON");

    }
}

void SliderWidget::setPosition(int value)
{
    pos = value;

    int diff = centerRight.x() - centerLeft.x();
    float someValue = float(value - centerLeft.x()) / diff;

    int r, g, b;

    if (colorStart.red() < colorEnd.red())
    {
        r = someValue * (colorStart.red() - colorEnd.red()) + colorEnd.red();
    }
    else
    {
        r = someValue * (-colorStart.red() + colorEnd.red()) + colorStart.red();
    }

    if (colorStart.green() < colorEnd.green())
    {
        g = someValue * (colorStart.green() - colorEnd.green()) + colorEnd.green();
    }
    else
    {
        g = someValue * (-colorStart.green() + colorEnd.green()) + colorStart.green();
    }

    if (colorStart.blue() < colorEnd.blue())
    {
        b = someValue * (colorStart.blue() - colorEnd.blue()) + colorEnd.blue();
    }
    else
    {
        b = someValue * (-colorStart.blue() + colorEnd.blue()) + colorStart.blue();
    }

    currentColor = QColor(r, g, b);

    this->repaint();
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
        animation->setDuration(600);
        animation->setEasingCurve(QEasingCurve::InOutExpo);

        animate(true);
        // tigger update on animation finished
        connect(animation, SIGNAL(finished()), this, SLOT(update()));
        emit customWindowEnable(isCustomWindow);
    }
    repaint();
}

void SliderWidget::resizeEvent(QResizeEvent *)
{
    if (isCustomWindow)
    {
        pos = centerRight.x();
    }
    else
    {
        pos = centerLeft.x();
    }
}
