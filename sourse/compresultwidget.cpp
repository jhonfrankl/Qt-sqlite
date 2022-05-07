#include "head/compresultwidget.h"
#include "head/ui_compresultwidget.h"

#include <QFileInfo>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QMap>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QStandardItem>
#include <QStandardPaths>
#include <QtXlsx/xlsxdocument.h>

#define cout qDebug()<<"["<<__FILE__<<":"<<__LINE__<<"]"

compResultWidget::compResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::compResultWidget)
{
    ui->setupUi(this);
    setting = new defaultSetting;
    connect(ui->treeView,&QTreeView::pressed,this,&compResultWidget::on_Treeview_Pressed);
}

void compResultWidget::closeEvent(QCloseEvent *event)
{
    cout<<"closeEvent";
    QWidget::closeEvent(event);
}

compResultWidget::~compResultWidget()
{
    delete ui;
}

void compResultWidget::compare(QList<QStringList> lTableNameList)
{
    QElapsedTimer timer;
    timer.start();
    tableStructList.clear();
    treeModel.clear();
    for(int i=0;i<lTableNameList.count();i++){//QList<QStringList>中包含多个list，每个list第一个QString为数据库路径，往后是表格名
        QStringList cellDataList;
        QList<QBrush> cellColorList;

        //获取数据库路径并添加item
        QString sDbFilePath = lTableNameList.at(i).at(0);
        QFileInfo fileInfo(sDbFilePath);
        QString sFileName = fileInfo.fileName();
        QStandardItem *pDbItem = new QStandardItem(sFileName);
        pDbItem->setData(sDbFilePath);
        treeModel.appendRow(pDbItem);

        if(lTableNameList.at(i).count()>1){//判断当前节点下是否有表格
            for(int j=1;j<lTableNameList.at(i).count();j++){
                QString sTableName = lTableNameList.at(i).at(j);
                QStandardItem *pTableItem = new QStandardItem(sTableName);
                //获取当前数据库
                QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","get");
                db.setDatabaseName(sDbFilePath);
                db.open();
                db.transaction();//创建事务

                //通过tableModel获取表格的行列数，是一种比较简单的方法
                tableModel = new QSqlTableModel(this,db);
                tableModel->setTable(sTableName);
                tableModel->select();
                int iRowCount = tableModel->rowCount();
                int iColCount = tableModel->columnCount();

                //获取表头list
                QStringList headers;
                for(int a=0;a<iColCount;a++)
                    headers.append(tableModel->headerData(a,Qt::Horizontal).toString());                    

                //将数据暂时存储在结构体中的2个list中
                for(int a=0;a<iRowCount;a++){
                    for(int b=0;b<iColCount;b++){
                        QModelIndex index = tableModel->index(a,b);
                        cellDataList.append(tableModel->data(index).toString());
                        cellColorList.append(Qt::white);
                    }
                }

                QSqlQuery sqlQuery(db);
                //获取标准数据库并打开
                QString sStdDbFilePath = setting->sendDBFP();
                if(sStdDbFilePath.isEmpty()){
                    QMessageBox::warning(this,"Error","标准数据库路径有误,请重新设置");
                    return;
                }
                QSqlDatabase DB = QSqlDatabase::addDatabase("QSQLITE","standard");
                DB.setDatabaseName(sStdDbFilePath);
                DB.open();
                QStringList stdTableList = DB.tables();
                int iStdDbTablecount = stdTableList.count();//获取标准数据库表格数量
                QStringList slStdCoefficent;//系数list
                slStdCoefficent<<"In"<<"/In"<<"Ie"<<"/Ie";

                //获取定值名称、定值所在的列数
                int iValNameColCount=-1 ,iMaxValColCount=-1,iMinValColCount=-1,iUnitsValColCount=-1;
                for(int a=0;a<headers.count();a++){
                    if(headers.at(a)==setting->sendValueName())
                        iValNameColCount = a;
                    if(headers.at(a)==setting->sendMax())
                        iMaxValColCount =a;
                    if(headers.at(a)==setting->sendMin())
                        iMinValColCount =a;
                    if(headers.at(a)==setting->sendUnitsValueName())
                        iUnitsValColCount =a;
                }
                cout<<iValNameColCount<<""<<iMaxValColCount<<" "<<iMinValColCount<<" "<<iUnitsValColCount;
                //检查是否有定值名称所在的列,没有则直接执行下一次循环
                if(iValNameColCount<0)
                    continue;
                pDbItem->appendRow(pTableItem);//可以进行对比后再添加item

                QSqlQuery query(DB);
                float IN=1.f,IE=1.f;
                if(setting->sendIn().toFloat()!=0)
                    IN = setting->sendIn().toFloat();
                if(setting->sendIe().toFloat()!=0)
                    IE = setting->sendIe().toFloat();

                QString sSelectFromTable= QString("select *  from " +sTableName);
                if(sqlQuery.exec(sSelectFromTable)){//遍历查询
                    long  iNowRowCount=0;//当前查到的行数
                    while(sqlQuery.next()){
                        QString name = sqlQuery.value(iValNameColCount).toString();
                        float max,min;
                        QString units;
                        if(iMaxValColCount>=0)
                            max =sqlQuery.value(iMaxValColCount).toFloat();
                        if(iMinValColCount>=0)
                            min =sqlQuery.value(iMinValColCount).toFloat();
                        if(iUnitsValColCount>=0)
                            units= sqlQuery.value(iUnitsValColCount).toString();

                        DB.transaction();//创建事务
                        for(int j=0;j<iStdDbTablecount;j++){
                            QString querySameNameRow= QString("select * from %1 where 定值名称 = '%2'").arg(stdTableList.at(j)).arg(name);

                           //在标准库中查询
                            if (query.exec(querySameNameRow)){
                                if(query.first()){          //QSqlQuery返回的数据集，record是停在第一条记录之前的。获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                                    QString sStdValName = query.value("定值名称").toString();
                                    cout<<sStdValName;
                                    if(!sStdValName.isEmpty()){//如果查询到了，再做比较工作
                                        cellColorList.replace(iNowRowCount*iColCount+iValNameColCount,Qt::green);//查询到结果后，将定值名称的单元格标为绿色
                                        QString sStdValueRange = query.value("定值范围").toString();
                                        QString sStdCoeValue = query.value("系数").toString();
                                        QString sStdUnitValue = query.value("单位").toString();
                                        float fStdDownValue=  sStdValueRange.section(QRegExp("~"),0,0).trimmed().toFloat();  //获取定值范围的最小值
                                        float fStdUpValue  = sStdValueRange.section(QRegExp("~"),1,1).trimmed().toFloat();    //获取定值范围的最大值
                                        cout<<sStdValueRange<<" "<<sStdCoeValue<<" "<<sStdUnitValue<<" "<<fStdDownValue<<" "<<fStdUpValue;
                                        //检测系数为stringlist中的第几个
                                        int a=0;
                                        for(int b=0;b<slStdCoefficent.count();b++){
                                            if (sStdCoeValue==slStdCoefficent.at(b)){
                                                a=b+1;
                                                break;
                                            }
                                        }
                                        switch(a){
                                            case 1:
                                                    fStdDownValue *= IN;
                                                    fStdUpValue *=IN;
                                            case 2:
                                                    fStdDownValue /=  IN;
                                                    fStdUpValue  /= IN;
                                            case 3:
                                                    fStdDownValue *= IE;
                                                    fStdUpValue  *= IE;
                                            case 4:
                                                    fStdDownValue /=IE;
                                                    fStdUpValue  /=IE;
                                       }
                                        if(iMinValColCount>=0)
                                            if((min<fStdDownValue)||(min>fStdUpValue ))//比较
                                                cellColorList.replace(iNowRowCount*iColCount+iMinValColCount,Qt::red);
                                        if(iMaxValColCount>=0)
                                            if((max<fStdDownValue)||(max>fStdUpValue ))//比较
                                                cellColorList.replace(iNowRowCount*iColCount+iMaxValColCount,Qt::red);
                                        if(iUnitsValColCount>=0)
                                            if(units!=sStdUnitValue)
                                                cellColorList.replace(iNowRowCount*iColCount+iUnitsValColCount,Qt::red);
                                    }
                                }
                            }
                            else
                                qDebug()<<sqlQuery.lastError();
                        }
                        DB.commit();
                        iNowRowCount++;
                    }
                }
            else
                treeModel.removeRow(pDbItem->row());
             db.commit();
             tableStruct aTableStruct{sDbFilePath,sTableName,headers,iRowCount,iColCount,cellDataList,cellColorList};
             tableStructList.append(aTableStruct);
            }
        }
    }
    cout<<tableStructList.count();
    QSqlDatabase::removeDatabase("get");
    QSqlDatabase::removeDatabase("standard");
    ui->treeView->setModel(&treeModel);
    emit finish();
    qDebug()<<"对比用时"<<timer.elapsed()<<"ms";

}

void compResultWidget::on_Treeview_Pressed(QModelIndex curIndex)
{
    QElapsedTimer timer;
    timer.start();
    QStandardItem *curItem = treeModel.itemFromIndex(curIndex);
    QString sCurDbFilePath,sCurTableName;
    if(!curItem->parent())
        return;
    else{
        sCurTableName = curItem->text();
        sCurDbFilePath=curItem->parent()->data().toString();
    }
    for(int i=0;i<tableStructList.count();i++){
        if(tableStructList.at(i).sDbFilePath==sCurDbFilePath&&tableStructList.at(i).sTabelName==sCurTableName){
            ui->tableWidget->setRowCount(tableStructList.at(i).iRowCount);
            ui->tableWidget->setColumnCount(tableStructList.at(i).iColCount);
            ui->tableWidget->setHorizontalHeaderLabels(tableStructList.at(i).slHeaders);
            for(int a=0;a<tableStructList.at(i).iRowCount;a++){
                for(int b=0;b<tableStructList.at(i).iColCount;b++){
                    QTableWidgetItem *item = new QTableWidgetItem(tableStructList.at(i).celldataList.at(a*tableStructList.at(i).iColCount+b));
                    ui->tableWidget->setItem(a,b,item);
                    ui->tableWidget->item(a,b)->setBackground(tableStructList.at(i).cellColorList.at(a*tableStructList.at(i).iColCount+b));
                }
            }
            break;
        }
    }
    cout<<"用时"<<timer.elapsed()<<"ms";
}

void compResultWidget::on_pushButton_clicked()
{
    QElapsedTimer time;
    time.start();
    QString  filePath =
                  QFileDialog::getOpenFileName(this,"打开",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                               "Excel 文件(*.xls *.xlsx)");
    for(int i=0;i<tableStructList.count();i++){
        //获取行和列
        int rowCount= tableStructList.at(i).iRowCount;
        int colCount=tableStructList.at(i).iColCount ;
        cout<<rowCount<<" "<<colCount;
        QXlsx::Document xlsxDoc(filePath);
        QString tableName =tableStructList.at(i).sTabelName;
        QString tempTableName = tableName;
        int j=1;
        while(1){
            if(xlsxDoc.addSheet(tableName))
                break;
            else
                tableName = QString(tempTableName+"(%1)").arg(i);
            j++;
        }
        xlsxDoc.selectSheet(tableName);

        for(int a=0;a<colCount;a++)
            xlsxDoc.write(1,a+1,tableStructList.at(i).slHeaders.at(a));

        QXlsx::Format format;
        for(int a=0;a<rowCount;a++){
            for(int b=0;b<colCount;b++){
                if(tableStructList.at(i).cellColorList.at(a*colCount+b)!=Qt::white)
                    format.setFontColor(tableStructList.at(i).cellColorList.at(a*colCount+b).color());//设置字体颜色为tableWidget的颜色
                else
                    format.setFontColor(Qt::black);
                xlsxDoc.write(a+2,b+1,tableStructList.at(i).celldataList.at(a*colCount+b),format);
            }
        }
        xlsxDoc.saveAs(filePath);
    }
    qDebug()<<"导出用时"<<time.elapsed()<<"ms";
}
