#ifndef PIECHART_H
#define PIECHART_H

#include <QWidget>

class pieChart : public QWidget
{
    Q_OBJECT
public:
    explicit pieChart(QWidget *parent = 0);

    void getlast(qint16 last);
    void getsell(qint16 sell);
    void drawPiechart(QPaintDevice *paintdevice);//绘制饼状图

protected:
    void paintEvent(QPaintEvent *event);//在窗口绘制图形必须在paintEvent中完成

signals:

public slots:

private:
    qreal roundcenterX;//圆心X坐标
    qreal roundcenterY;//圆心Y坐标
    qreal radius;//半径
    qint16 m_last;
    qint16 m_sell;
};

#endif // PIECHART_H
