#include "domxml.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDomDocument>//用来表示xml对象
#include <QDomProcessingInstruction>//用来表示xml说明，即XML第一句
#include <QDomElement> //用来表示xml元素
#include <QDebug>
#include <QDateTime>

//__FILE__能打印出文件名，__LINE__能打印出行号
#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

Domxml::Domxml()
{

}

//创建xml文件，即在XML中加入说明和根元素
void Domxml::createXml(QString filePath, QString rootstr)
{
    QFile file(filePath);
    if(file.exists() == true)//判断文件是否存在
    {
        cout << QObject::tr(u8"文件已经存在");
        return;
    }
    else
    {
        bool ret = file.open(QIODevice::ReadWrite);//用可读写的方式打开文件
        if(ret == false)
        {
            cout << QObject::tr(u8"文件打开失败");
            return;
        }

        //新建QDomDocument对象，代表一个XML文档
        QDomDocument doc;
        //创建QDomProcessingInstruction对象，它用来创建XML说明<?xml version='1.0' encoding='utf-8'?>
        QDomProcessingInstruction instruction;
        instruction = doc.createProcessingInstruction("xml","version=\'1.0\' encoding=\'utf-8\'");//<?和?>会自动添加
        //将xml说明插入到XML中
        doc.appendChild(instruction);

        //创建根节点
        QDomElement root = doc.createElement(rootstr);
        doc.appendChild(root);

        //保存
        QTextStream stream(&file);
        doc.save(stream,4);

        file.close();
    }
}

//向xml文件加入节点(元素)
void Domxml::appendXml(QString filepath,QStringList list)
{
    QFile file(filepath);
    //注意如果以读写方式打开文件，写入时会直接从文件尾部追加要写入的内容，这样会导致文件内容（XML说明和根节点）载入doc之中再写
    //入文件之中会从文件尾部追加，从而导致最后文件有两份XML说明和根节点，而以只写方式打开文件，写入内容之前会将文件之前的内容清空
    if(!file.open(QIODevice::ReadOnly))
    {
        cout << QObject::tr(u8"文件只读方式打开失败");
        return;
    }

    //创建xml文档对象
    QDomDocument doc;
    bool ret = doc.setContent(&file);//将文件中的内容读取到doc中
    file.close();//关闭文件
    if(ret == false)
    {
        return;
    }

    //获取根节点
    QDomElement root = doc.documentElement();

    //获取当前日期
    QDateTime date = QDateTime::currentDateTime();
    QString formatdate = date.toString("yyyy-MM-dd");//设置日期格式

    //判断根节点后是否有子节点
    if(root.hasChildNodes() == true)
    {
        //获取最后一个节点
        QDomElement lastElement = root.lastChildElement();

        //比较要加入的节点是否有父节点
        if(lastElement.attribute("date") == formatdate)
        {
            writeXml(doc,lastElement,list);
            //保存至文件
            if(!file.open(QIODevice::WriteOnly))
            {
                cout << QObject::tr(u8"文件只写方式打开失败");
                return;
            }
            QTextStream stream(&file);
            doc.save(stream,4);
            file.close();//关闭文件
        }
        else
        {
            //没有说明要加入的节点日期在XML中没有，因此要重新创建一个新的节点来存储
            //创建子节点，日期
            QDomElement dateElement = doc.createElement(QObject::tr(u8"日期"));
            QDomAttr dateAttr = doc.createAttribute("date");//创建节点属性
            dateAttr.setNodeValue(formatdate);//设置节点属性的值
            //将节点属性和节点关联起来
            dateElement.setAttributeNode(dateAttr);
            //将新建子节点加入到根节点之下
            root.appendChild(dateElement);

            //向XML写入内容
            writeXml(doc,dateElement,list);

            //保存至文件
            if(!file.open(QIODevice::WriteOnly))
            {
                cout << QObject::tr(u8"文件只写方式打开失败");
                return;
            }
            QTextStream stream(&file);
            doc.save(stream,4);
            file.close();
        }
    }
    else
    {
        //创建子节点，日期
        QDomElement dateElement = doc.createElement(QObject::tr(u8"日期"));
        QDomAttr dateAttr = doc.createAttribute("date");//创建节点属性
        dateAttr.setNodeValue(formatdate);//设置节点属性的值
        //将节点属性和节点关联起来
        dateElement.setAttributeNode(dateAttr);
        //将新建子节点加入到根节点之下
        root.appendChild(dateElement);

        //向XML写入内容
        writeXml(doc,dateElement,list);

        //保存至文件
        if(!file.open(QIODevice::WriteOnly))
        {
            cout << QObject::tr(u8"文件只写方式打开失败");
            return;
        }
        QTextStream stream(&file);
        doc.save(stream,4);
        file.close();
    }
}

//向XML写入，dateElement作为写入的头节点
void Domxml::writeXml(QDomDocument doc,QDomElement dateElement,QStringList list)
{
    //获取系统当前时间
    QDateTime time = QDateTime::currentDateTime();
    QString formattime = time.toString("hh:mm:ss");

    //创建时间子节点
    QDomElement timeElement = doc.createElement(QObject::tr(u8"时间"));
    QDomAttr timeAttr = doc.createAttribute("time");//创建节点属性
    timeAttr.setNodeValue(formattime);
    timeElement.setAttributeNode(timeAttr);//将节点属性和节点关联起来
    //将时间节点添加到根节点之下
    dateElement.appendChild(timeElement);

    //创建时间节点之下的子节点
    QDomElement factory = doc.createElement(QObject::tr(u8"厂家"));
    QDomElement brand = doc.createElement(QObject::tr(u8"品牌"));
    QDomElement price = doc.createElement(QObject::tr(u8"报价"));
    QDomElement salenumber = doc.createElement(QObject::tr(u8"数量"));
    QDomElement saletotal = doc.createElement(QObject::tr(u8"金额"));
    //创建子节点的文本内容
    QDomText ftext = doc.createTextNode(list.at(0));
    QDomText btext = doc.createTextNode(list.at(1));
    QDomText ptext = doc.createTextNode(list.at(2));
    QDomText ntext = doc.createTextNode(list.at(3));
    QDomText ttext = doc.createTextNode(list.at(4));
    //将文本内容加入到对应的子节点之下
    factory.appendChild(ftext);
    brand.appendChild(btext);
    price.appendChild(ptext);
    salenumber.appendChild(ntext);
    saletotal.appendChild(ttext);
    //将子节点加入到时间节点之下
    timeElement.appendChild(factory);
    timeElement.appendChild(brand);
    timeElement.appendChild(price);
    timeElement.appendChild(salenumber);
    timeElement.appendChild(saletotal);
}

//读取XML
void Domxml::readXml(QString filepath,QStringList &flist,QStringList &blist,QStringList &plist,QStringList &nlist,QStringList &tlist,QStringList &timelist)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly))
    {
        cout << QObject::tr(u8"文件只读方式打开失败");
        return;
    }

    //创建xml文档对象
    QDomDocument doc;
    bool ret = doc.setContent(&file);//将文件中的内容读取到doc中
    file.close();
    if(ret == false)
    {
        return;
    }

    //获取根节点
    QDomElement root = doc.documentElement();

    //获取当前日期
    QDateTime date = QDateTime::currentDateTime();
    QString formatdate = date.toString("yyyy-MM-dd");//设置日期格式

    //判断是否有子节点
    if(root.hasChildNodes() == true)
    {
        //从最后一个节点读，因为最后一个节点日期最新
        QDomElement lastElement = root.lastChildElement();
        QString lastAttr = lastElement.attribute("date");
        //qDebug() << QObject::tr(u8"最后一个节点的日期属性:") << lastAttr << QObject::tr(u8"当前日期:") << formatdate;
        //判断有没有当天日期
        if(lastAttr == formatdate)
        {
            //找出当前日期下的所有时间节点
            QDomNodeList nodelist = lastElement.childNodes();
            //遍历当前日期下所有时间节点
            for(int i = 0; i < nodelist.size(); i++)
            {
                //获取时间节点下的所有子节点
                QDomNodeList childnlist = nodelist.at(i).toElement().childNodes();//要将节点转换成元素
                //获取时间节点的time属性
                timelist << nodelist.at(i).toElement().attribute("time");

                QString ftext = childnlist.at(0).toElement().text();//获取节点的文本内容
                flist.append(ftext);//将厂家节点的文本内容存入flist中
                QString btext = childnlist.at(1).toElement().text();//获取节点的文本内容
                blist.append(btext);//将品牌节点的文本内容存入blist中
                QString ptext = childnlist.at(2).toElement().text();//获取节点的文本内容
                plist.append(ptext);//将报价节点的文本内容存入plist中
                QString ntext = childnlist.at(3).toElement().text();//获取节点的文本内容
                nlist.append(ntext);//将数量节点的文本内容存入flist中
                QString ttext = childnlist.at(4).toElement().text();//获取节点的文本内容
                tlist.append(ttext);//将金额节点的文本内容存入flist中
            }
        }
        else
        {
            cout << QObject::tr(u8"XML没有当天日期数据,今天还没有卖出");
            return;
        }
    }
    else
    {
        cout << QObject::tr(u8"XML中无子节点数据，还未有销售数据");
        return;
    }
}

//读写存储用户登陆信息的XML文件
void Domxml::wrtieLoginXml(QString filepath, QStringList list)
{
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly) == false)
    {
        cout << QObject::tr(u8"文件只读方式打开失败");
        return;
    }

    QDomDocument doc;
    bool ret = doc.setContent(&file);
    file.close();
    if(ret == false)
    {
        return;
    }

    QDomElement root = doc.documentElement();

    //写入普通用户
    QDomElement userType = doc.createElement(QObject::tr(u8"用户类型"));
    QDomAttr Attr = doc.createAttribute("userType");
    Attr.setNodeValue(list.at(0));
    userType.setAttributeNode(Attr);
    root.appendChild(userType);
    //在普通用户节点下加入用户名、密码节点
    QDomElement username = doc.createElement(QObject::tr(u8"用户名"));
    userType.appendChild(username);
    QDomText nametext = doc.createTextNode(list.at(1));
    username.appendChild(nametext);
    QDomElement password = doc.createElement(QObject::tr(u8"密码"));
    userType.appendChild(password);
    QDomText pwdtext = doc.createTextNode(list.at(2));
    password.appendChild(pwdtext);

    //保存至文件
    if(!file.open(QIODevice::WriteOnly))
    {
        cout << QObject::tr(u8"文件只写方式打开失败");
        return;
    }
    QTextStream stream(&file);
    doc.save(stream,4);
    file.close();
}

void Domxml::readLoginXml(QString filepath, QStringList &userType, QStringList &userlist, QStringList &passwordlist)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly))
    {
        cout << QObject::tr(u8"文件只读方式打开失败");
        return;
    }

    //创建xml文档对象
    QDomDocument doc;
    bool ret = doc.setContent(&file);//将文件中的内容读取到doc中
    file.close();
    if(ret == false)
    {
        return;
    }

    //获取根节点
    QDomElement root = doc.documentElement();

    QDomNodeList nodelist = root.childNodes();//获取根节点下的所有子节点
    //遍历所有子节点
    for(int i = 0; i < nodelist.size(); i++)
    {
        QString usertype = nodelist.at(i).toElement().attribute("userType");
        if(usertype == QObject::tr(u8"管理员"))
        {
            userType << usertype;
            QDomNodeList subnodelist = nodelist.at(0).toElement().childNodes();
            userlist << subnodelist.at(0).toElement().text();
            passwordlist << subnodelist.at(1).toElement().text();
        }
        if(usertype == QObject::tr(u8"普通用户"))
        {
            userType << usertype;
            QDomNodeList subnodelist = nodelist.at(i).toElement().childNodes();
            userlist << subnodelist.at(0).toElement().text();
            passwordlist << subnodelist.at(1).toElement().text();
        }
    }
}
