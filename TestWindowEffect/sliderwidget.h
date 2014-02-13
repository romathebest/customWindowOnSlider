#ifndef SLIDERWIDGET_H
#define SLIDERWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>

const QColor colorStart = QColor(205, 186, 150);
const QColor colorEnd = QColor(102, 205, 0);

class SliderWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int pos READ position WRITE setPosition)
public:
    explicit SliderWidget(QWidget *parent = 0);
    ~SliderWidget();

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *);

    int position() const;

public slots:
    void animate(bool);
    void setPosition(int value);

signals:
    void customWindowEnable(bool state);

private:
    QFont * myFont;

    QColor currentColor;
    bool firstRun;
    int pos;
    QPoint centerLeft, centerRight;
    bool isCustomWindow;
    QPropertyAnimation *animation;
};

#endif // SLIDERWIDGET_H
