#include "head/worker.h"

#include <QAxObject>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardItemModel>
#include <QString>
#include <QTime>
#define sWarn "warning"
#define sInformation "information"
#define cout qDebug()<<"["<<__FILE__<<":"<<__LINE__<<"]"

//class deleteCol
deleteCol::deleteCol()
{

}

void deleteCol::dowork(int curCol, int cycleCount,QString tableName, QString dbfile)
{
    QElapsedTimer time;
    time.start();
    QString infromation;
    qDebug()<<"删除列当前线程ID为："<<QThread::currentThreadId();
    QSqlDatabase dB= QSqlDatabase::addDatabase("QSQLITE","removeCol");
    dB.setDatabaseName(dbfile);
    if(!dB.open()){
        emit deleteInfromation("无法打开数据库",sWarn);
        emit finish();
        return;
    }
    dB.transaction();//创建事务
    for(int i=0;i<cycleCount;i++){

        QSqlQuery query(dB);
        query.exec("select * from "+tableName);
        QSqlRecord record = query.record();//获取表头

        query.exec("alter table "+tableName+" rename to "+ tableName+"tempTable");//将table改名为临时表格tempTable
        //创建新表
        int columnCount = record.count();//获取表格的列数
        QString tableCreate = "create table "+tableName +"(";
        if((curCol+1)==columnCount){
            for(int i=0;i<(columnCount-1);i++)
                tableCreate+=record.fieldName(i)+"  varchar,";
         }
        else{
            for(int i=0;i<columnCount;i++){
                if(i==curCol)
                    i+=1;
                tableCreate+=record.fieldName(i) +"  varchar,";
            }
        }
        tableCreate = tableCreate.left(tableCreate.size()-1)+")";
        qDebug()<<tableCreate;
        if(!query.exec(tableCreate)) {
            infromation="表格创建失败";
            emit deleteInfromation(infromation,"warning");
            emit finish();
            return;
        }

        //插入数据
        query.prepare("select * from "+tableName+"tempTable");
        query.exec();
        QString insert = "insert into "+tableName+"(";
        if((curCol+1)==columnCount){  //判断是否为最后一列
            for(int i=0;i<(columnCount-1);i++){
                insert += (record.fieldName(i)+" ,");
            }
        }
        else{
            for(int i=0;i<columnCount;i++){
                if(i==curCol)
                    i+=1;
                insert += (record.fieldName(i)+" ,");
            }
        }

        insert = insert.left(insert.size()-1);
        insert+=")values";
        while (query.next()){
            insert+="(";
            if((curCol+1)==columnCount){    //判断是否为最后一列
                for(int i=0;i<(columnCount-1);i++){
                    insert += "'"+query.value(record.fieldName(i)).toString()+"'"+",";
                }
            }
            else{
                for(int i=0;i<columnCount;i++){
                    if(i==curCol)
                        i+=1;
                    insert += "'"+query.value(record.fieldName(i)).toString()+"'"+",";
                }
            }
            insert = insert.left(insert.size()-1) ;
            insert+="),";
        }
        insert = insert.left(insert.size()-1) ;        
        if(!query.exec(insert)){
            qDebug()<<insert;
            infromation="插入数据失败";
            emit deleteInfromation(infromation,sWarn);
            emit finish();
            return;
        }
        query.exec("drop table "+tableName+"tempTable");//删除旧表   
    }
    dB.commit();//提交事务
    dB.close();
    QSqlDatabase::removeDatabase("removeCol");
    emit finish();
    qDebug()<<"删除本列用时"<<time.elapsed()<<"ms";
}

//class addCol
addCol::addCol()
{

}

void addCol::dowork(int curCol,QString headName, QString tableName, QString dbfile)
{
    QElapsedTimer time;
    time.start();
    QString infromation;
    qDebug()<<"删除列当前线程ID为："<<QThread::currentThreadId();
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE","addCol");
    db.setDatabaseName(dbfile);
    if(!db.open()){
        emit addInformation("无法打开数据库",sWarn);
        emit finish();
        return;
    }

    //事务开始
    db.transaction();
    QSqlQuery query(db);
    query.exec("select * from "+tableName);
    QSqlRecord record = query.record();//获取表头

   //创建新表
    int columnCount = record.count();//获取表格的列数
    for(int i=0;i<columnCount;i++)
        qDebug()<<record.fieldName(i);
    QString tableCreate = "create table "+tableName+"tempTAble" +"(";
    for(int i=0;i<columnCount;i++){
        tableCreate+=record.fieldName(i) +"  varchar,";
        if(i==curCol)
            tableCreate+= headName +"  varchar,";
    }
    tableCreate = tableCreate.left(tableCreate.size()-1)+")";

    //创建表格失败则返回
    if(!query.exec(tableCreate)){
        infromation="插入失败,表头不可以是关键词且不能与已有表头相同";
        emit addInformation(infromation,sWarn);
        emit finish();
        return;
     }

    //插入数据
    query.prepare("select * from "+tableName);
    query.exec();
    QString insert = "insert into "+tableName+"tempTable  (";
    for(int i=0;i<columnCount;i++){
        insert += (record.fieldName(i)+" ,");
        if(i==curCol)
            insert+=headName+" ,";
    }
    insert = insert.left(insert.size()-1);
    insert+=")values";

    while (query.next()){
        insert+="(";
        for(int i=0;i<columnCount;i++){
            insert += "'"+query.value(record.fieldName(i)).toString()+"'"+",";
            if(i==curCol)
                insert += "' ',";
         }
        insert = insert.left(insert.size()-1) ;
        insert+="),";
    }
    insert = insert.left(insert.size()-1) ;
    qDebug()<<insert;
    if(!query.exec(insert)){     //执行插入命令
        infromation="插入数据失败";
        emit addInformation(infromation,sWarn);
        emit finish();
        return;
    }
    if(!query.exec("drop table "+tableName)){//删除旧表
        emit finish();
        return;
    }
    query.exec("alter table "+tableName+"tempTable rename to "+ tableName);//将临时表格改名为原表格名
    db.commit();//提交事务
    db.close();
    QSqlDatabase::removeDatabase("addCol");
    qDebug()<<"增加本列用时"<<time.elapsed()<<"ms";
    emit finish();
}

//class exportExcel
exportExcel::exportExcel()
{

}

void exportExcel::dowork(QSqlTableModel *tableModel,QString filePath,QTableWidget *tableWidget)
{
    QElapsedTimer time;
    time.start();
    qDebug()<<QThread::currentThreadId();

    //获取行和列
    int rowCount= tableWidget->rowCount();
    int colCount= tableWidget->columnCount();

    QXlsx::Document xlsxDoc(filePath);
    QString tableName = tableModel->tableName();
    QString tempTableName = tableName;
    int i=1;
    while(1){
        if(xlsxDoc.addSheet(tableName))
            break;
        else
            tableName = QString(tempTableName+"(%1)").arg(i);
        i++;
    }
    xlsxDoc.selectSheet(tableName);
    QXlsx::Format format;
    for(int i=0;i<colCount;i++){
        cout<<"正在获取文本内容";
        QString itemText = tableWidget->horizontalHeaderItem(i)->text();
        qDebug()<<itemText;
        xlsxDoc.write(1,i+1,itemText);
    }
    for(int i=0;i<rowCount;i++){
        for(int j=0;j<colCount;j++){
            format.setFontColor(tableWidget->item(i,j)->backgroundColor());//设置字体颜色为tableWidget的颜色           
            xlsxDoc.write(i+2,j+1,tableWidget->item(i,j)->text(),format);
        }
    }
    xlsxDoc.saveAs(filePath);
    QString information = "导出成功";
    emit exportInformation(information,sInformation);
    emit finish();
    qDebug()<<"导出用时"<<time.elapsed()<<"ms";
}

