#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QVector>

//前置声明，比直接加头文件效率要高
QT_BEGIN_NAMESPACE

    class QString;
    class QRect;
    class QColor;
    //class QVector;

QT_END_NAMESPACE

//起到宏定义的作用
enum histogramarguments
{
    blankwideth = 50, //柱子间空白宽度
    fpillarIndent = 20,//第一个柱子与Y轴的距离
    xOffset = 20,//x轴与底边的距离
    yOffset = 30,//y轴与左边的距离
    pillarTextheight = 30//柱子文本框的高度
};

//柱状图中的每个柱子类
class Pillarinhistogram
{
public:
    QString p_name;
    qint16  p_value;
    QColor  p_color;
    QRect   p_rect;
};

//柱状图类
class Histogram : public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = 0);

    void addPillar(QString name,qint16 value,QColor color);//向柱状图增加柱子
    void drawHistogram(QPaintDevice *paintdevice);//绘制柱状图
    void histogramClear();//清空柱状图
    void setYmaxvalue(qint16 maxvalue);//设置Y轴最大值

protected:
    void paintEvent(QPaintEvent *event);//在窗口绘图必须在绘图事件实现

private:
    void drawCoordinate(QPainter &painter,QPaintDevice *paintdevice);//绘制x、y轴
    void drawPillar(QPainter &painter,QPaintDevice *paintdevice);//绘制每一条柱子
    void drawText(QPainter &painter);//绘制柱子的说明文本，包括品牌和销售数量
    void drawScale(QPainter &painter,QPaintDevice *paintdevice);//绘制y轴刻度

    qreal m_ymaxvalue;//y轴最大值
    QVector<Pillarinhistogram> m_pillars;//用vector容器存储每一个要在柱状图显示的柱子
};

#endif // HISTOGRAM_H
