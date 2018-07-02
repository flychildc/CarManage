#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void connectDatabase();//连接数据库
    void initData();//初始化数据

private slots:
    void on_actionCarManagge_triggered();

    void on_actionSaleCalc_triggered();

    void on_actionsabout_triggered();

    void on_comboBoxFactory_currentIndexChanged(const QString &arg1);

    void on_comboBoxBrand_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

    void on_comboBoxnfactory_currentIndexChanged(const QString &arg1);

    void on_spinBoxN_valueChanged(int arg1);

    void on_pushButtonNOk_clicked();

    void on_pushButtonNCancel_clicked();

    void on_pushButtonMore_clicked();

    void on_comboBoxTfactory_currentIndexChanged(const QString &arg1);

    void on_pushButtonTInquire_clicked();

    void on_pushButtonTdisplay_clicked();

    void on_pushButtonTincrease_clicked();

    void on_pushButtondescrease_clicked();

    void on_pushButtonTadd_clicked();

    void on_pushButtonTDel_clicked();

    void on_pushButtonTCommit_clicked();

    void on_pushButtonTCancel_clicked();

    void on_pushButtonTExit_clicked();

    void on_comboBoxSfactory_currentIndexChanged(const QString &arg1);

    void on_comboBoxSbrand_currentIndexChanged(const QString &arg1);

    void on_pushButtonLogin_clicked();

    void on_pushButtonLogcancel_clicked();

    void on_actionLogout_triggered();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    QStringList userType, userlist, pwdlist;//用户登陆类型、用户名、密码
};

#endif // MAINWINDOW_H
