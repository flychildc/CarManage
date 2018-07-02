#include "piechart.h"
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include <QRect>
#include <QPen>
#include <QDebug>

pieChart::pieChart(QWidget *parent) : QWidget(parent)
{
    //初始化数据
    roundcenterX = 90;
    roundcenterY = 140;
    radius = 90;
    m_sell = 0;
    m_last = 1;
}

void pieChart::getlast(qint16 last)
{
    m_last = last;
}

void pieChart::getsell(qint16 sell)
{
    m_sell = sell;
}

//绘制饼状图
void pieChart::drawPiechart(QPaintDevice *paintdevice)
{
    //计算比例
    qreal sellratio = 100.0 * m_sell / (m_last + m_sell);
    //计算扇形角度
    qreal sellpieAngle = 3.60 * sellratio;
    qreal lastpieAngle = 360 - sellpieAngle;

    qDebug() << "sellpieAngle:" << sellpieAngle << "lastpieAngle" << lastpieAngle;

    QPainter painter(paintdevice);
    painter.setBrush(QBrush(QColor(Qt::blue)));
    //绘制扇形,起始角度为水平方向，角度为正逆时针，为负顺时针，1代表1/16度，整个元为360*16
    painter.drawPie(roundcenterX-radius, roundcenterY-radius, 2*radius, 2*radius, 0, sellpieAngle*16);
    painter.setBrush(QBrush(QColor(Qt::darkYellow)));
    painter.drawPie(roundcenterX-radius, roundcenterY-radius, 2*radius, 2*radius, sellpieAngle*16, 360*16-sellpieAngle*16);//最后一个参数是从倒数第二个参数作为起始角度，往后的角度
    //painter.drawPie(roundcenterX-radius, roundcenterY-radius, 2*radius, 2*radius, 0, -lastpieAngle*16);
    //绘制比例
    QString sellratioText = QString("%1%").arg(sellratio);
    QString lastratioText = QString("%1%").arg(100 - sellratio);
    //painter.setBrush(Qt::NoBrush);
    painter.drawText(roundcenterX+30, roundcenterY-40, sellratioText);
    painter.drawText(roundcenterX-40, roundcenterY+40, lastratioText);
    //绘制两个方框
    QRect sellrect(roundcenterX+radius+20, roundcenterY, 20, 15);
    QRect lastrect(roundcenterX+radius+20, roundcenterY+20, 20 ,15);
    painter.setBrush(QBrush(QColor(Qt::blue)));
    painter.drawRect(sellrect);
    painter.setBrush(QBrush(QColor(Qt::darkYellow)));
    painter.drawRect(lastrect);
    //绘制方框文本
    QRect sellrectText(roundcenterX+radius+45, roundcenterY, 40, 15);
    QRect lastrectText(roundcenterX+radius+45, roundcenterY+20, 40, 15);
    QString sellText(tr(u8"已出售"));
    QString lastText(tr(u8"剩余"));
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255,255,255,0)));
    painter.drawRect(sellrectText);
    painter.drawRect(lastrectText);
    painter.setPen(QPen(QColor(0,0,0)));
    painter.drawText(sellrectText,Qt::AlignCenter,sellText);
    painter.drawText(lastrectText,Qt::AlignCenter,lastText);

    //this->update();
}

//在窗口绘制图形必须在paintEvent中完成
void pieChart::paintEvent(QPaintEvent *event)
{
    drawPiechart(this);
}
