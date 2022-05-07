#ifndef THREAD_H
#define THREAD_H
#include "defaultsetting.h"
#include "xlsxdocument.h"
#include <QObject>
#include <QTableWidget>
#include <QThread>
#include <QSqlTableModel>
#include <QStandardItemModel>

class deleteCol:public QObject
{
    Q_OBJECT
public:
    deleteCol();
public slots:
    void dowork(int curCol,int cycleCount,QString tableName,QString dbfile);
signals:
    void deleteInfromation(QString,QString);
    void finish();
};

class addCol:public QObject
{
    Q_OBJECT
public:
    addCol();
public slots:
    void dowork(int curCol,QString headName,QString tablename,QString dbfile);
signals:
    void addInformation(QString,QString);
    void finish();
};

class exportExcel:public QObject
{
    Q_OBJECT
public:
    exportExcel();
public slots:
    void dowork(QSqlTableModel *tableModel,QString filePath,QTableWidget *tableWidget);
signals:
   void exportInformation(QString,QString);
   void finish();
};



#endif // THREAD_H
