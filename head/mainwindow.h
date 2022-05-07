#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "defaultsetting.h"
#include "xlsxdocument.h"
#include "mmscomagent.h"
#include "mmsdef.h"
#include "worker.h"
#include "compresultwidget.h"

#include <QLabel>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QThread>
#include <QtXlsx/QtXlsx>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    bool m_ExitFlag;
    bool bIsConnecting = false;
    defaultSetting setting;
    compResultWidget widget;
    inline QString getDatabase();
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MMSComAgent *m_pComAgent = nullptr;
    QThread  thread ;
    QWidget  * pWidget =  NULL;
    QTableWidget     *tableWidget=NULL;
    QSqlTableModel *tableModel=NULL;
    QStandardItemModel treeModel;

private slots:
    void on_createDBAction_triggered();//新建数据库
    void on_openMenu_triggered();//打开数据库
    void on_queryMenu_triggered();//根据名字查询表格
    void on_refrashAction_triggered();//刷新
    void recvInformation(QString,QString sParameter);//接收来自子线程的各种信息
    void connectDevice();//联机
    void closeEvent(QCloseEvent *event) override;

private slots://增加列线程槽函数
    void addColMenu();//增加列

private slots://删除列线程槽函数
    void deleteColMenu();//删除列

private slots://对表格进行的操作
    void tableViewMenu(const QPoint &pos);
    void createTable();//新建表格
    void addRowMenu();//增加行
    void deleteRowMenu();//删除行
    void renameColMenu();//列的重命名

private slots://完成导入表格需要的槽函数
    void importExcelMenu();//导入Excel

private slots://属性的设置
    void on_defaultSetAction_triggered();

private slots://完成比较需要的槽函数
    void on_compareTableMenu_triggered();//比较

private slots://treeView及其菜单的槽函数
    void on_treeView_pressed(QModelIndex modelIndex);//当treeView中的表格被点击时，在右边显示内容
    void treeViewMenu(const QPoint &pos);//为treeView添加菜单选项，实现删除，重命名等功能
    void onActionDelete();//删除表格
    void onActionRename();//重命名表格

signals:
    void startCompare(QList<QStringList>);//对比发出信号
    void deleteCol(int curCol,int cycleCount,QString tableName,QString dbfile);//删除列发出信号
    void addCol(int curCol,QString headName,QString tablename,QString dbfile);//增加列发起信号
    void exportExcel(QSqlTableModel *tableModel,QString filePath,QTableWidget *tableWidget);//导出表格

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
