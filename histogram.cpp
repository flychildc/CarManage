#include "histogram.h"
#include <QPainter>
#include <QPaintDevice>
#include <QRect>
#include <QBrush>
#include <QPen>
#include <QDebug>

Histogram::Histogram(QWidget *parent) : QWidget(parent)
{
    m_pillars.clear();
}

//向柱状图增加柱子
void Histogram::addPillar(QString name,qint16 value,QColor color)
{
    Pillarinhistogram pillar;

    pillar.p_name = name;
    pillar.p_value = value;
    pillar.p_color = color;
    pillar.p_rect = QRect();

    m_pillars.push_back(pillar);//将pillar添加到m_pillars中

    this->update();
}

//绘制柱状图
void Histogram::drawHistogram(QPaintDevice *paintdevice)
{
    QPainter painter(paintdevice);//关联画家和绘图设备

    drawCoordinate(painter,paintdevice);
    drawScale(painter,paintdevice);
    drawPillar(painter,paintdevice);
    drawText(painter);
}

//清空柱状图
void Histogram::histogramClear()
{
    m_pillars.clear();
    //this->update;
    QWidget::update();
}

//设置Y轴最大值
void Histogram::setYmaxvalue(qint16 maxvalue)
{
    m_ymaxvalue = maxvalue;
}

//绘制x、y轴
void Histogram::drawCoordinate(QPainter &painter,QPaintDevice *paintdevice)
{
    //绘制X轴，要注意的是原点是在绘图设备的左上角，和窗口类似
    qint16 xAxiscoordinatex = paintdevice->height() - xOffset;//x轴在绘图设备的坐标系统中的x坐标
    painter.drawLine(0,xAxiscoordinatex,paintdevice->width(),xAxiscoordinatex);
    //绘制y轴
    painter.drawLine(yOffset,0,yOffset,paintdevice->height());
}

//绘制每一条柱子
void Histogram::drawPillar(QPainter &painter,QPaintDevice *paintdevice)
{
    //先判断是否有柱子
    if(m_pillars.size() == 0)
    {
        qDebug() << tr(u8"柱状图中还没有数据！");
        return;
    }

    //确定每一根柱子的宽度
    qreal pillarwidth = (paintdevice->width() - 10 - yOffset - fpillarIndent -blankwideth*(m_pillars.size()-1)) / m_pillars.size();//减10是为了给最右边的柱子留点余量
    //柱子的高度通过绘图设备的高度除以y轴刻度最大值得到一个比例因子，然后柱子的值乘以这个比例因子即可得到柱子在坐标系中的y值
    qreal pillarheightfactor = (paintdevice->height()-20) / m_ymaxvalue;

//    qDebug() << "paintdevice->height():" << paintdevice->height();
//    qDebug() << "paintdevice->width():" << paintdevice->width();
//    qDebug() << "pillarwidth:" << pillarwidth;
//    qDebug() << "pillarheightfactor:" << pillarheightfactor;

    //绘制柱子
    for(int i = 0; i < m_pillars.size(); i++)
    {
        //确定柱子的位置
        qint16 pillarPosition = yOffset + fpillarIndent + (blankwideth+pillarwidth) * i;
        //确定柱子的高度
        qreal pillarheight = pillarheightfactor * m_pillars.at(i).p_value;

        //qDebug() << "pillarPosition:" <<pillarPosition << "pillarheight:" << pillarheight;

        //每根柱子左上角的x、y坐标
        qreal leftupX = pillarPosition;
        qreal leftupY = paintdevice->height() - pillarheight - xOffset;

        QRect &rect = (QRect&)m_pillars.at(i).p_rect;//使用引用设置Pillarinhistogram中的rect成员
        rect.setRect(leftupX,leftupY,pillarwidth,pillarheight);
        painter.setPen(Qt::SolidLine);
        painter.setPen(m_pillars.at(i).p_color);
        QBrush brush(m_pillars.at(i).p_color);
        painter.setBrush(brush);
        painter.drawRect(rect);
    }
}

//绘制柱子的说明文本，包括品牌和销售数量
void Histogram::drawText(QPainter &painter)
{
    //设置画笔颜色
    painter.setBrush(Qt::NoBrush);//不使用画刷
    //绘制文本
    for(int i = 0; i < m_pillars.size(); i++)
    {
        //绘制框
        qreal leftupX = m_pillars.at(i).p_rect.left() - blankwideth/2;//设置文本框的左上角X坐标为柱子左边x坐标减去空格一半
        qreal leftupY = m_pillars.at(i).p_rect.top() - pillarTextheight;
        QRect rect(leftupX,leftupY,m_pillars.at(i).p_rect.width()+blankwideth,pillarTextheight);
        painter.setPen(QPen(QColor(255,255,255,0)));//设置绘制的方框边框是白色，最后参数0表明不透明，运行时就看不到方框
        painter.drawRect(rect);
        //在框中绘制文本
        QString text = QString(tr(u8"%1(%2)")).arg(m_pillars.at(i).p_name).arg(m_pillars.at(i).p_value);
        painter.setPen(QPen(QColor(0,0,0)));//将画笔设置成黑色
        painter.drawText(rect,Qt::AlignCenter,text);
    }

}

//绘制y轴刻度,从坐标原点顺着Y轴方向绘制刻度
void Histogram::drawScale(QPainter &painter,QPaintDevice *paintdevice)
{
    qreal paintAreaH = paintdevice->height() - 20;//为了避免最上面的刻度被挡住，因此在绘图设备的高度方向留点余量
    qreal segmentscaleH = paintAreaH / 8;//在y轴上标8个刻度
    qreal factor = m_ymaxvalue / paintAreaH;

    for(int i = 0; i < 9; i++)
    {
        //qint16 n = i + 1;
        qreal scalevalue = (i * segmentscaleH) * factor;//计算出每一个刻度的值
        //绘制刻度线
        qreal scalelineY = paintdevice->height() - xOffset - segmentscaleH*i;
        painter.drawLine(yOffset-2, scalelineY, yOffset+2, scalelineY);
        //绘制刻度值
        qint16 scaleVrectwidth = 25;
        qint16 scaleVrectheigh = pillarTextheight;
        QRect rect(0,scalelineY,scaleVrectwidth,scaleVrectheigh);
        painter.setPen(QPen(QColor(255,255,255,0)));//最后参数0表明不透明
        painter.drawRect(rect);
        painter.setPen(QPen(QColor(0,0,0)));
        painter.drawText(rect,Qt::AlignLeft,QString("%1").arg(scalevalue));
    }
}

//在窗口绘图必须在绘图事件实现,每次运行会自动调用
void Histogram::paintEvent(QPaintEvent *event)
{
    drawHistogram(this);
}
