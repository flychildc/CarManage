﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QDebug>
#include <QTextCodec>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QItemSelectionModel>
#include <QtGlobal>
#include <QTime>
#include <QVector>
#include <QCompleter>
#include "domxml.h"
//#include "histogram.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始界面为用户登陆界面
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);//初始界面为用户登陆界面
    ui->labelPagename->setText(tr(u8"用户登录"));

    //创建用户登陆信息存储XML文件
    Domxml::createXml("E:/Project/QTProgram/video/day06/carManage/LoginData.xml", tr(u8"登录信息"));
//    QStringList list;
//    list << QString(tr(u8"普通用户")) <<QString(tr(u8"苏珊")) << QString("888888");
//    Domxml::wrtieLoginXml("E:/Project/QTProgram/video/day06/carManage/LoginData.xml",list);


    //连接数据库
    connectDatabase();
    //qDebug() << QSqlDatabase::drivers();

    //初始化数据
    initData();

    //创建SQL表格模型
    model = new QSqlTableModel(this);
    model->setTable("brand");
    model->select();
    //设计编辑策略,手动提交更改
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//所有的更改都会在模型中缓存，直到调用submitAll或revertAll

    //创建销售xml文件
    Domxml::createXml("E:/Project/QTProgram/video/day06/carManage/salelist.xml");
    //到车入库xml文件，需要重新写一下读写xml的文件
    //Domxml::createXml("E:/Project/QTProgram/video/day06/carManage/storeCarlist.xml");
    //改变按钮风格

    //绘制柱状图
    ui->widgetHistogram->setYmaxvalue(100);//设置Y轴坐标最大值
}

MainWindow::~MainWindow()
{
    delete ui;
}

//车辆管理
void MainWindow::on_actionCarManagge_triggered()
{
    //设置当前页面为车辆管理
    ui->stackedWidget->setCurrentWidget(ui->carManage);
    ui->labelPagename->setText(tr(u8"车辆管理"));
}

//销售统计
void MainWindow::on_actionSaleCalc_triggered()
{
    //设置当前页面为销售统计
    ui->stackedWidget->setCurrentWidget(ui->Salecalc);
    ui->labelPagename->setText(tr(u8"销售统计"));
}

//系统设置
void MainWindow::on_actionsabout_triggered()
{
    QMessageBox::about(this,tr(u8"汽车销售管理系统"),tr(u8"这是一个基于Qt5.8平台的汽车销售管理系统！"));
}

//连接数据库
void MainWindow::connectDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("0517");
    db.setDatabaseName("car");

    //解决中文乱码问题，要在数据库打开之前，查看mysql中的my.ini配置文件发现mysql默认是utf-8编码，
    //而windows下QT是GBK,因此要设置成utf-8
    QTextCodec *code = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(code);

    if(db.open() == false)
    {
        QMessageBox::warning(this,tr(u8"数据库打开错误"),db.lastError().text());
        return;
    }
}

//初始化数据
void MainWindow::initData()
{
    QSqlQueryModel *querymodel = new QSqlQueryModel(this);//创建查询模型
    querymodel->setQuery("select name from factory");//sql语句
    ui->comboBoxFactory->setModel(querymodel);
    ui->comboBoxnfactory->setModel(querymodel);
    ui->comboBoxTfactory->setModel(querymodel);
    ui->comboBoxSfactory->setModel(querymodel);

    //用户登陆界面初始化
    ui->actionCarManagge->setEnabled(false);//设置销售管理菜单无效
    ui->actionSaleCalc->setEnabled(false);
    ui->actionadduser->setEnabled(false);
    ui->actionLogout->setEnabled(false);
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);//设置显示模式为密码样式
    //用户登陆界面用户输入行编辑器输入自动补全
    Domxml::readLoginXml("E:/Project/QTProgram/video/day06/carManage/LoginData.xml", userType, userlist, pwdlist);
    QCompleter *completer = new QCompleter(userlist,this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);//设置大小写不敏感
    ui->lineEditUser->setCompleter(completer);

    //初始出售车辆页面，总金额编辑框不可写
    ui->lineEdittTotal->setEnabled(false);
    ui->spinBox->setEnabled(false);
    ui->pushButtonOk->setEnabled(false);
    //初始化新车入库页面
    ui->spinBoxN->setEnabled(false);
    ui->labelstore->clear();
    ui->pushButtonNOk->setEnabled(false);
    //表格页面初始化
    ui->pushButtonTInquire->setEnabled(false);
}

//出售车辆页面厂家下拉框槽函数
void MainWindow::on_comboBoxFactory_currentIndexChanged(const QString &arg1)
{

    //请选择厂家选项和其他的不同
    if(arg1 == tr(u8"请选择厂家"))
    {
        //清空
        ui->comboBoxBrand->clear();
        ui->labelLast->clear();
        ui->lineEditPrice->clear();
        ui->lineEdittTotal->clear();
        ui->spinBox->setValue(0);
    }
    else
    {
        //恢复选择框
        ui->spinBox->setEnabled(true);
        //将之前厂家的品牌清除
        ui->comboBoxBrand->clear();
        //ui->lineEdittTotal->setEnabled(true);
        QSqlQuery query;//不能使用QSqlQueryModel，因为它只能查询整张表格
        QString command = QString("select name from brand where factory = '%1'").arg(arg1);
        query.exec(command);

        while(query.next())
        {
            QString name = query.value(0).toString();
            ui->comboBoxBrand->addItem(name);
        }
    }
}

//出售车辆页面品牌下拉框槽函数
void MainWindow::on_comboBoxBrand_currentIndexChanged(const QString &arg1)
{
    //如果没有选择品牌
    if(arg1.isEmpty())
    {
        return;
    }

    QSqlQuery query;
    QString command = QString("select price,last from brand where factory = '%1' and name = '%2'")
            .arg(ui->comboBoxFactory->currentText())
            .arg(arg1);
    query.exec(command);

    //执行mysql语句查询的结果只有一行2列
    query.first();//必须找到查询结果的第一条记录，才能获取查询结果
    int price = query.value("price").toInt();
    int last = query.value("last").toInt();
    ui->lineEditPrice->setText(QString::number(price));
    ui->labelLast->setText(QString::number(last));
}

//出售车辆页面选择框发生改变槽函数（销售车辆）
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    //使用数据库的last作为基准，因为窗口labellast中的数据会改变
    QString factoryname = ui->comboBoxFactory->currentText();
    QString brandname = ui->comboBoxBrand->currentText();
    if(brandname.isEmpty())
    {
        return;
    }

    QSqlQuery query;
    QString command = QString("select last from brand where factory = '%1' and name = '%2'")
            .arg(factoryname)
            .arg(brandname);
    query.exec(command);
    query.first();//必须找到查询结果的第一条记录，才能获取查询结果
    int last = query.value("last").toInt();

    //检查选择框的数据是否比剩余车辆数还大
    if(arg1 > last)
    {
        ui->spinBox->setValue(last);
        return;
    }

    //如果arg1大于0
    if(arg1 > 0)
    {
        ui->pushButtonOk->setEnabled(true);
    }

    //更新剩余车辆数量
    int currentN = last - arg1;
    ui->labelLast->setText(QString::number(currentN));

    //显示销售总额
    int price = ui->lineEditPrice->text().toInt();
    int salenum = ui->spinBox->text().toInt();
    int total = price * salenum;
    ui->lineEdittTotal->setText(QString::number(total));
}

void MainWindow::on_pushButtonCancel_clicked()
{
    ui->comboBoxFactory->setCurrentIndex(0);//会调用on_comboBoxFactory_currentIndexChanged函数
}

void MainWindow::on_pushButtonOk_clicked()
{
    QString factoryname = ui->comboBoxFactory->currentText();
    QString brandname = ui->comboBoxBrand->currentText();
    if(brandname.isEmpty())
    {
        return;
    }

    //今天销售之后的剩余量
    int last = ui->labelLast->text().toInt();

    QSqlQuery query;
    //查询数据库中该品牌的总量
    QString command0 = QString("select sum from brand where factory = '%1' and name = '%2'")
            .arg(factoryname)
            .arg(brandname);
    query.exec(command0);
    query.next();
    int sum = query.value("sum").toInt();

    //之前销售额加上今天的销售额
    int sell = sum - last;

    //更新数据库
    QString command1 = QString("update brand set sell = %1,last = %2 where factory = '%3' and name = '%4'")
            .arg(sell)
            .arg(last)
            .arg(factoryname)
            .arg(brandname);
    query.exec(command1);

    //每提交一次，文本编辑框就刷新一遍销售数据
    QStringList list;
    list << factoryname << brandname << ui->lineEditPrice->text() << QString::number(ui->spinBox->value())
         << ui->lineEdittTotal->text();//将厂家、品牌、报价、数量、金额存入list中
    Domxml::appendXml("E:/Project/QTProgram/video/day06/carManage/salelist.xml",list);//将每日销售情况写入Xml文件中
    //qDebug() << QObject::tr(u8"list所存储的值：") <<list.at(0) << list.at(1) << list.at(2) << list.at(3) << list.at(4);
    QStringList flist;
    QStringList blist;
    QStringList plist;
    QStringList nlist;
    QStringList tlist;
    QStringList timelist;
    Domxml::readXml("E:/Project/QTProgram/video/day06/carManage/salelist.xml",
                    flist,blist,plist,nlist,tlist,timelist);//将当天销售情况读出来

    //先清空文本编辑区
    ui->textEdit->clear();

    for(int i = 0; i < flist.size(); i++)
    {
        QString str = QString(tr(u8"今天%1 ：厂家：%2，品牌：%3售出了%4,单价：%5,总价：%6"))
                .arg(timelist.at(i))
                .arg(flist.at(i))
                .arg(blist.at(i))
                .arg(nlist.at(i))
                .arg(plist.at(i))
                .arg(tlist.at(i));
        ui->textEdit->append(str);
        qDebug() << str.toUtf8().data();
    }

    //数据库更新完后,设置确定按钮无效，避免重复提交
    ui->pushButtonOk->setEnabled(false);
}

/*
 ************新车入库页面操作
 */

//新车入库页面的厂家下拉选泽框槽函数
void MainWindow::on_comboBoxnfactory_currentIndexChanged(const QString &arg1)
{
    //请选择厂家选项和其他的不同,并且这里判断的时候注意中文编码要一致
    if(arg1 == tr(u8"请选择厂家"))
    {
        //清空
        ui->comboBoxnbrand->clear();
        ui->labelstore->clear();
        ui->spinBoxN->setValue(0);
    }
    else
    {
        //恢复选择框
        ui->spinBoxN->setEnabled(true);
        //将之前厂家的品牌清除
        ui->comboBoxnbrand->clear();
        QSqlQuery query;//不能使用QSqlQueryModel，因为它只能查询整张表格
        QString command = QString("select name from brand where factory = '%1'").arg(arg1);
        query.exec(command);

        while(query.next())
        {
            QString name = query.value(0).toString();
            ui->comboBoxnbrand->addItem(name);
        }
    }
}

//新车入库页面的新到车辆选择框槽函数
void MainWindow::on_spinBoxN_valueChanged(int arg1)
{
    QString factoryname = ui->comboBoxnfactory->currentText();
    QString brandname = ui->comboBoxnbrand->currentText();
    if(brandname.isEmpty())
    {
        return;
    }

    QSqlQuery query;
    QString command = QString("select last from brand where factory = '%1' and name = '%2'")
            .arg(factoryname)
            .arg(brandname);
    query.exec(command);
    query.first();//必须找到查询结果的第一条记录，才能获取查询结果
    int last = query.value("last").toInt();

    //如果arg1大于0
    if(arg1 > 0)
    {
        ui->pushButtonNOk->setEnabled(true);
    }

    //更新剩余车辆数量,即库存
    int currentN = last + arg1;
    ui->labelstore->setText(QString::number(currentN));

}

//新车入库页面确定按钮的槽函数
void MainWindow::on_pushButtonNOk_clicked()
{
    QString factoryname = ui->comboBoxnfactory->currentText();
    QString brandname = ui->comboBoxnbrand->currentText();
    if(brandname.isEmpty())
    {
        return;
    }

    //今天新到的数量
    int newcome = ui->spinBoxN->value();

    //库存，即剩余的数量
    int storenum = ui->labelstore->text().toInt();

    QSqlQuery query;
    //查询数据库中该品牌的总量
    QString command0 = QString("select sum from brand where factory = '%1' and name = '%2'")
            .arg(factoryname)
            .arg(brandname);
    query.exec(command0);
    query.next();
    int sum = query.value("sum").toInt();

    //更新总数
    int newsum = sum + newcome;

    //更新数据库
    QString command1 = QString("update brand set sum = %1,last = %2 where factory = '%3' and name = '%4'")
            .arg(newsum)
            .arg(storenum)
            .arg(factoryname)
            .arg(brandname);
    query.exec(command1);

    //数据库更新完后,设置确定按钮无效，避免重复提交
    ui->pushButtonOk->setEnabled(false);
}

//新车入库页面取消按钮的槽函数
void MainWindow::on_pushButtonNCancel_clicked()
{
    ui->comboBoxnfactory->setCurrentIndex(0);//会调用on_comboBoxnfactory_currentIndexChanged函数
}

//新车入库页面更多按钮的槽函数
void MainWindow::on_pushButtonMore_clicked()
{
    //更多按钮的提示说明
    QMessageBox::information(this,tr(u8"提示对话框"),tr(u8"你将直接通过表格的形式直接操作数据库"));

    //使用model/view方式将数据库在界面中显示出来
    ui->stackedWidget->setCurrentWidget(ui->formOperation);
    ui->tableView->setModel(model);
}

/*
 ************表格页面操作数据库
 */

//表格页面厂家下拉框槽函数
void MainWindow::on_comboBoxTfactory_currentIndexChanged(const QString &arg1)
{
    //请选择厂家选项和其他的不同,并且这里判断的时候注意中文编码要一致
    if(arg1 == tr(u8"请选择厂家"))
    {
        //清空
        ui->comboBoxTbrand->clear();
        ui->pushButtonTInquire->setEnabled(false);
    }
    else
    {
        ui->pushButtonTInquire->setEnabled(true);
        //将之前厂家的品牌清除
        ui->comboBoxnbrand->clear();
        QSqlQuery query;//不能使用QSqlQueryModel，因为它只能查询整张表格
        QString command = QString("select name from brand where factory = '%1'").arg(arg1);
        query.exec(command);

        while(query.next())
        {
            QString name = query.value(0).toString();
            ui->comboBoxTbrand->addItem(name);
        }
    }
}

//表格页面查询按钮槽函数
void MainWindow::on_pushButtonTInquire_clicked()
{
    QString factoryname = ui->comboBoxTfactory->currentText();
    QString brandname = ui->comboBoxTbrand->currentText();

    //qDebug() << "factoryname:" << factoryname << "brandname:" <<brandname;

    if(ui->checkBox->isChecked())
    {
        model->setFilter(QString("factory = '%1' and name = '%2'").arg(factoryname).arg(brandname));
    }
    else
    {
        model->setFilter(QString("factory = '%1'").arg(factoryname));
    }

    model->select();
}

//表格页面显示全表槽函数
void MainWindow::on_pushButtonTdisplay_clicked()
{
    model->setTable("brand");
    model->select();
}

//表格页面升序排序按钮槽函数
void MainWindow::on_pushButtonTincrease_clicked()
{
    //获取排序下拉框的内容
    QString combotext = ui->comboBoxTsort->currentText();
    int comboindex = ui->comboBoxTsort->currentIndex();//下拉框下标从0开始
    //qDebug() << "comboindex:" << comboindex;
    if(combotext == "id")
    {
        model->setSort(0,Qt::AscendingOrder);
    }
    else
    {
        model->setSort(comboindex+2,Qt::AscendingOrder);
    }

    model->select();
}

//表格页面降序排序按钮槽函数
void MainWindow::on_pushButtondescrease_clicked()
{
    //获取排序下拉框的内容
    QString combotext = ui->comboBoxTsort->currentText();
    int comboindex = ui->comboBoxTsort->currentIndex();
    if(combotext == "id")
    {
        model->setSort(0,Qt::DescendingOrder);
    }
    else
    {
        model->setSort(comboindex+2,Qt::DescendingOrder);
    }

    model->select();
}

//表格页面添加按钮槽函数
void MainWindow::on_pushButtonTadd_clicked()
{
    //创建空记录
    QSqlRecord record = model->record();
    int rownum = model->rowCount();//获取行数
    model->insertRecord(rownum,record);//将空记录插入
}

//表格页面删除按钮槽函数
void MainWindow::on_pushButtonTDel_clicked()
{
    QItemSelectionModel *submodel = ui->tableView->selectionModel();
    QModelIndexList subindexlist = submodel->selectedRows();//返回选择的所有行的索引
    //删除所有行
    for(int i = 0; i < subindexlist.size(); i++)
    {
        model->removeRow(subindexlist.at(i).row());//索引要转化成行号
    }
}

//表格页面提交按钮槽函数
void MainWindow::on_pushButtonTCommit_clicked()
{
    model->database().transaction();//开启事务操作
    if(model->submitAll())//提交更改
    {
        if(model->database().commit())//提交事务操作
        {
            QMessageBox::information(this,tr(u8"提交"),tr(u8"提交数据库成功！"));
        }
    }
    else
    {
        model->database().rollback();//回滚事务操作
        QMessageBox::information(this,tr(u8"提交出错"),tr(u8"数据库错误：%1").arg(model->lastError().text()));
    }

}

//表格页面取消按钮槽函数，只能撤销没有提交到数据库的操作，提交操作之后就无法撤销，无论是界面上还是数据库
void MainWindow::on_pushButtonTCancel_clicked()
{
    model->revertAll();
    model->submitAll();
}

//表格页面退出按钮槽函数
void MainWindow::on_pushButtonTExit_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->carManage);
}

/*
 * *****销量统计页面
 */
//销量统计页面厂家下拉框槽函数
void MainWindow::on_comboBoxSfactory_currentIndexChanged(const QString &arg1)
{
    //请选择厂家选项和其他的不同,并且这里判断的时候注意中文编码要一致
    if(arg1 == tr(u8"请选择厂家"))
    {
        //清空
        ui->comboBoxSbrand->clear();
        ui->widgetHistogram->histogramClear();//清空柱状图
    }
    else
    {
        //将之前厂家的品牌清除
        ui->comboBoxSbrand->clear();
        ui->widgetHistogram->histogramClear();//清空柱状图
        QSqlQuery query;//不能使用QSqlQueryModel，因为它只能查询整张表格
        QString command = QString("select name,sell from brand where factory = '%1'").arg(arg1);
        query.exec(command);

        while(query.next())
        {
            QString name = query.value("name").toString();
            qint16 sell = query.value("sell").toInt();

            ui->comboBoxSbrand->addItem(name);//给品牌下拉框添加内容
            //产生随机值作为RGB的值，用来设置颜色
            QColor color(rand() % 256,rand() % 256,rand() % 256);
            ui->widgetHistogram->addPillar(name,sell,color);//向柱状图添加元素
        }
    }
}

//销量统计页面品牌下拉框槽函数
void MainWindow::on_comboBoxSbrand_currentIndexChanged(const QString &arg1)
{
    if(ui->comboBoxSbrand->currentText() == "")
    {
        ui->widgetPie->getlast(1);
        ui->widgetPie->getsell(0);
        ui->widgetPie->update();//手动更新
    }
    else
    {
        //绘制饼状图
        QString factoryname = ui->comboBoxSfactory->currentText();
        QString brandname = arg1;
        QString command = QString("select sell,last from brand where factory = '%1' and name = '%2'")
                .arg(factoryname)
                .arg(brandname);
        QSqlQuery query;
        query.exec(command);

        query.first();
        qint16 sell = query.value("sell").toInt();
        qint16 last = query.value("last").toInt();

        ui->widgetPie->getsell(sell);
        ui->widgetPie->getlast(last);

        ui->widgetPie->update();
    }
}

/*
 ******登录页面槽函数
*/
//登录页面登陆槽函数
void MainWindow::on_pushButtonLogin_clicked()
{
    bool userExist = false;//用户是否存在标志
    QString username = ui->lineEditUser->text();
    QString password = ui->lineEditPassword->text();

    Domxml::readLoginXml("E:/Project/QTProgram/video/day06/carManage/LoginData.xml", userType, userlist, pwdlist);
    for(int i = 0; i < userlist.size(); i++)
    {
        //qDebug() << "userlist.at(i):" << userlist.at(i) << "pwdlist.at(i):" << pwdlist.at(i);

        if(username == userlist.at(i))//用户存在
        {
            userExist = true;
            if(password == pwdlist.at(i))//密码正确
            {
                if(userType.at(i) == tr(u8"管理员"))
                {
                    QMessageBox::information(this, "information", tr(u8"这是管理员账户！"));
                    ui->actionadduser->setEnabled(true);
                }
                ui->stackedWidget->setCurrentWidget(ui->carManage);//进入车辆管理页面
                ui->labelPagename->setText(tr(u8"车辆管理"));
                ui->actionCarManagge->setEnabled(true);
                ui->actionSaleCalc->setEnabled(true);
                ui->actionLogout->setEnabled(true);
                break;
            }
            else
            {
                QMessageBox::information(this, "error", tr(u8"密码输入错误，请重新输入！"));
                break;
            }
        }
    }

    if(userExist == false)
    {
        QMessageBox::information(this, "information", tr(u8"用户不存在！"));
    }
}
//登录页面取消槽函数
void MainWindow::on_pushButtonLogcancel_clicked()
{
    ui->lineEditUser->clear();
    ui->lineEditPassword->clear();
}

//退出登陆子菜单
void MainWindow::on_actionLogout_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    ui->labelPagename->setText(tr(u8"用户登录"));
    on_pushButtonLogcancel_clicked();
}
