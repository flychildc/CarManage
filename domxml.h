#ifndef DOMXML_H
#define DOMXML_H
#include <QString>
#include <QStringList>
#include <QDomDocument>
#include <QDomElement>
#include <QObject>

class Domxml
{
public:
    Domxml();

    //静态函数，方便调用
    static void createXml(QString filePath, QString rootstr = QObject::tr(u8"日销售清单"));//创建xml文件
    static void appendXml(QString filepath, QStringList list);//向记录日销售清单xml文件加入节点(元素)
    static void writeXml(QDomDocument doc, QDomElement dateElement, QStringList list);//向记录日销售清单xml文件写入，dateElement作为写入的头节点
    static void readXml(QString filepath, QStringList &flist, QStringList &blist, QStringList &plist,
                        QStringList &nlist, QStringList &tlist, QStringList &timelist);//读取记录日销售清单xml文件

    //读写存储用户登陆信息的XML文件
    static void wrtieLoginXml(QString filepath, QStringList list);
    static void readLoginXml(QString filepath, QStringList &userType, QStringList &userlist, QStringList &passwordlist);
};

#endif // DOMXML_H
