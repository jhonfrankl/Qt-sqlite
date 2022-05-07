#include "head/mainwindow.h"
#include "ui_mainwindow.h"
#include "head/worker.h"
#include "head/compresultwidget.h"

#include <QAbstractButton>
#include <QAxObject>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExp>
#include <QRegularExpression>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStandardPaths>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>
#define cout qDebug()<<"["<<__FILE__<<":"<<__LINE__<<"]"

SIUnit g_SIUnit;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);//将treeView设为不可编辑
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);//创建treeView右击显示菜单
    connect(ui->treeView,&QTreeView::customContextMenuRequested, this, &MainWindow::treeViewMenu);//连接treeView与菜单控件
    connect(ui->treeView,&QTreeView::pressed,this,&MainWindow::on_treeView_pressed);//根据被点击的表格在右边显示
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);//创建tableView右击显示菜单
    connect(ui->tableView,&QTableView::customContextMenuRequested,this,&MainWindow::tableViewMenu);//连接tableView和菜单控件
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_ExitFlag = true;
    qDebug()<<m_ExitFlag;
    if(thread.isRunning()){
        thread.quit();
        thread.wait();
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::on_createDBAction_triggered()//新建数据库,如果路径中已经存在，则询问是否替换
{
    //移除所有已经建立的数据库连接
    QSqlDatabase::removeDatabase("view");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    //检查数据库文件夹路径是否有误
    QFile file;
    QString sDbFolderPath = setting.sendNewDBFP();
    if(!file.exists(sDbFolderPath)||sDbFolderPath.isEmpty()){
        QMessageBox::information(NULL,"路径错误","路径有误，请重新设置");
        return;
    }

    //获取数据库名并检查是否有重复
    QString sDbName = QInputDialog::getText(this,"新建数据库","请输入新建数据库的名称");
    if(sDbName.isEmpty())
        return;

    QString data = sDbFolderPath+"/"+sDbName+".db";
    QDir dir(sDbFolderPath);
    QStringList nameFilters;
    nameFilters <<"*.db";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);//获取所有后缀为.db的文件
    qDebug()<<files;

    //循环查找重名文件
    for(int i=0;i<files.count();i++){
        if(files.at(i)==(sDbName+".db")){
            int ok=QMessageBox::information(NULL,"发现重复","该数据库已经存在，您确定要替换它吗",
                                            QMessageBox::Yes|QMessageBox::No);//询问是否替换
            if(ok==QMessageBox::Yes){
                if(!file.remove(data)){//删除已有文件
                    QMessageBox::information(NULL,"创建失败","该数据库正在使用，暂时不能操作");
                    return;
                }
                for(int j=0;j<treeModel.rowCount();j++){//移除treeModel的item
                    QStandardItem *pDbItem=treeModel.item(j);
                    if(pDbItem->data().toString()==data){
                        treeModel.removeRow(j);
                    }
                }
                break;
            }
            else//若选择no,则直接返回
                return;
        }
    }
    file.setFileName(data);//创建新文件

    //检测是否创建成功
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(data);
    bool ok = db.open();
    if(ok){
        QMessageBox::information(NULL,"新建数据库","数据库"+sDbName+"创建成功");

        //添加节点并设置数据
        QStandardItem *databaseItem = new QStandardItem(sDbName+".db");
        databaseItem->setData(data);
        treeModel.appendRow(databaseItem);
        ui->treeView->setModel(&treeModel);
        db.close();
    }
    else{
        QMessageBox::information(NULL,"新建数据库","数据库"+sDbName+"创建失败");
        return;
    }
}

void MainWindow::createTable()//根据当前数据库新建表格
{
    //获取表格名
    QInputDialog * inputDialog = new QInputDialog(this);
    QModelIndex index = ui->treeView->currentIndex();//获取当前的index
    QStandardItem *pDbItem = treeModel.itemFromIndex(index); //获取当前数据库item
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
    QString sDbFilePath = pDbItem->data().toString();//获取当前节点的数据库文件路径
    db.setDatabaseName(sDbFilePath);
    if(!db.open())
        return;
    QString sTableName;
    //QInputDialog事件循环
    while(1){
        bool bOk = false;
        sTableName = inputDialog->getText(NULL,"新建表格","请输入新建表格的名字");
        if(sTableName.isEmpty())
            return;
        QSqlQuery query(db);
        QString sCreateTable = QString("create table "+sTableName+ "(类别 varchar,定值名称 varchar,定值范围 varchar,系数 varchar,单位 varchar,整定值 varchar)");
        if(!query.exec(sCreateTable)){ //不能执行成功则说明表格名称有问题
            bOk = true;
            QMessageBox::warning(this,"Error","表格创建失败(表格名称不能以数字开头或与已有表格名相同)");
        }
        if(!bOk){
            inputDialog->close();
            break;
        }
    }
    delete inputDialog;
    db.close();
    QStandardItem *pTableItem = new QStandardItem(sTableName);
    pTableItem->setCheckable(true);
    pDbItem->appendRow(pTableItem);
    ui->treeView->setCurrentIndex(pTableItem->index());
    ui->treeView->setSortingEnabled(true);
    ui->treeView->sortByColumn(0,Qt::AscendingOrder);//设置第0列,以 aaa-zzz的顺序排序
    emit ui->treeView->pressed(pTableItem->index());//将表格在treeView上展示
}

void MainWindow::on_openMenu_triggered()//打开数据库
{
    QString sDbFilePath = QFileDialog::getOpenFileName(this,"选择数据库文件","","Sqlite数据库(*.db*)");
    QElapsedTimer time;
    time.start();
    QFileInfo fileInfo  = QFileInfo(sDbFilePath);
    QString    sFileName = fileInfo.fileName();
    if(sDbFilePath.isEmpty())
        return;

    //检测是否为数据库文件
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(sDbFilePath);
    if(!db.open()){
        QMessageBox::information(this,"提示","打开数据库失败!");
        return;
    }

    //检测该文件是否已经被打开
    for(int i=0;i<treeModel.rowCount();i++){
        QStandardItem *item = treeModel.item(i);
        if(item->data().toString()==sDbFilePath){
            QMessageBox::information(NULL,"Error","该数据库已经打开");
            return;
            }
    }

    QStandardItem *pDatabaseItem = new QStandardItem(sFileName);
    pDatabaseItem->setData(sDbFilePath);
    QStringList list = db.tables();
    int listCount= list.count();
    treeModel.appendRow(pDatabaseItem);
    for(int j=0;j<listCount;j++){
        QStandardItem *tableItem = new QStandardItem(list.at(j));
        tableItem->setCheckable(true); // checkbox 设置几点可选中
        pDatabaseItem->appendRow(tableItem);
    }

    ui->treeView->setModel(&treeModel);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->sortByColumn(0,Qt::AscendingOrder);//设置第0列,以 aaa-zzz的顺序排序
    ui->treeView->expand(pDatabaseItem->index());
    db.close();
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    qDebug()<<"打开"<<sFileName<<"用时"<<time.elapsed()<<"ms,"<<"共"<<list.count()<<"个表格";
}

void MainWindow::importExcelMenu()//导入Excel表格到当前数据库
{
    QString sExcelFilePath = QFileDialog::getOpenFileName(this,"打开",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                               "Excel 文件(*.xls *.xlsx)");
    if(sExcelFilePath .isEmpty())
        return;    
    QElapsedTimer time;
    time.start();
    int successCount =0;
    QStandardItem * pDbItem = treeModel.itemFromIndex(ui->treeView->currentIndex());
    QString sDbFilePath = pDbItem->data().toString();
    //打开数据库
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","excel");
    cout<<"设置数据库"<<sDbFilePath;
    db.setDatabaseName(sDbFilePath);
    if(!db.open()){
        QMessageBox::warning(NULL,"Error","无法打开数据库");
        return;
    }
    QSqlQuery sqlQuery(db);
    QStringList tableList = db.tables(); //获取表格名数组
    db.transaction();//创建事务

    //获取当前xlsx的sheetName 数组及当前sheet的行列数
    QXlsx::Document xlsxDoc(sExcelFilePath);
    QStringList sheetNameList = xlsxDoc.sheetNames();
    int sheetCount = sheetNameList.count();
    qDebug()<<sheetCount;
    for(int i=0;i<sheetCount;i++){
        xlsxDoc.selectSheet(sheetNameList[i]);
        QString sheetName = sheetNameList.at(i);
        qDebug()<<sheetName;
        int firstRow = xlsxDoc.dimension().firstRow();//起始行数
        int firstCol = xlsxDoc.dimension().firstColumn();//起始列数
        int finalRow = xlsxDoc.dimension().rowCount();//末尾行数
        int finalCol = xlsxDoc.dimension().columnCount();//末尾列数

        //创建table
        QString tableCreate = "create table "+sheetName+"(";
        for(int j=firstCol;j<=finalCol;j++){
            QString headName = xlsxDoc.read(firstRow,j).toString().remove(QRegExp("\\s"));
            if(headName.isEmpty())
                tableCreate +=QString("空表头%1").arg(j-firstRow+1)+" varchar,";
            else
                tableCreate+= (headName+"  varchar,"); //移除空格
        }
        tableCreate = (tableCreate.left(tableCreate.size()-1) +")");

        bool bHasRepeat = false;
        for(int a=0;a<tableList.count();a++){
            if(sheetName==tableList.at(a)){
                bHasRepeat = true;
                break;
            }
        }
        //插入数据
        if(!bHasRepeat){
            if(sqlQuery.exec(tableCreate)){
                successCount ++;//成功创建表格的数量
                QStandardItem *pTableItem = new QStandardItem(sheetName);
                pTableItem->setCheckable(true);
                pDbItem->appendRow(pTableItem);
                QString insert = "insert into "+ sheetName+"(";
                for(int j=firstCol;j<=finalCol;j++){
                    QString headName = xlsxDoc.read(firstRow,j).toString().remove(QRegExp("\\s"));
                    if(headName.isEmpty())
                        insert += QString("空表头%1").arg(j-firstRow+1)+" ,";
                    else
                        insert+= (xlsxDoc.read(firstRow,j).toString().remove(QRegExp("\\s"))+"  ,");
                }
                insert=insert.left(insert.size()-1) +" )values";

                for(int p=firstRow+1;p<=finalRow;p++){
                    insert +="(";
                    for(int q=firstCol;q<=finalCol;q++)
                        insert += ("'"+xlsxDoc.read(p,q).toString().remove(QRegExp("\\s"))+"'"+" ,");//移除空格
                    insert =(insert.left(insert.size()-1)+"),");
                 }
                insert=insert.left(insert.size()-1);
                if(!sqlQuery.exec(insert))
                    cout<<xlsxDoc.dimension().rowCount()<<"and"<<xlsxDoc.dimension().columnCount()
                           <<endl<<sqlQuery.lastError()<<sheetName<<tableCreate<<"插入失败"<<endl<<insert;
             }
            else
                qDebug()<<sqlQuery.lastError()<<"创建表格失败";
        }
        else
            qDebug()<<"发现重复的表格:"<<sheetName;
    }
    db.commit();//提交事务
    db.close();//关闭数据库
    QSqlDatabase::removeDatabase("excel");//断开连接
    int totalTime = time.elapsed();
    if(successCount==0){
        QMessageBox::information(NULL,"导入结束","表格名全部重复或创建表格失败");
    }
    else{
        QString infromation = QString("该文件共%1个表格,成功导入%2个表格。共用时%3ms,平均用时%4ms").arg(sheetCount).
                arg(successCount).arg(totalTime).arg(totalTime/successCount);
        QMessageBox::information(NULL,"导入结束",infromation);
    }
    ui->treeView->setModel(&treeModel);
    ui->treeView->expand(pDbItem->index());
}

void MainWindow::on_queryMenu_triggered()//查询表格，可模糊查询
{
    if(treeModel.rowCount()==0)//如果treeModel上没有内容则直接返回
        return;

    // 如果获取到的表格名为空，直接返回
    QString tableName=QInputDialog::getText(NULL,"查询","请输入您要查询的表格");
    if(tableName.isEmpty())
        return;
    int iQueryrResult=1;//如果查询到了结果，将值置为0，用于最后判断查询结果
    QModelIndex curIndex = ui->treeView->currentIndex();
    if(curIndex.isValid()){
        //如果有当前选中的节点，则从此节点开始向后查询
        QStandardItem *pCurItem = treeModel.itemFromIndex(curIndex);
        int iDbRowCount,iTableRowCount=0;
        if(!pCurItem->parent()){  //判断节点类型(数据库或者表格)
            iDbRowCount = pCurItem->row();
            qDebug()<<"iDbRowCount:"<<iDbRowCount<<endl<<"iTableRowCount:"<<iTableRowCount;
        }
        else{
            iDbRowCount = pCurItem->parent()->row();
            iTableRowCount = pCurItem->row();
            qDebug()<<"iDbRowCount:"<<iDbRowCount<<endl<<"iTableRowCount:"<<iTableRowCount;
        }
        //从当前节点开始查询
        for(int i=iDbRowCount;i<treeModel.rowCount();i++){
                QStandardItem *pDbItem = treeModel.item(i);
                //当查询下一个数据库节点时，从它的0行开始
                int iStartQueryRow;
                if(i==iDbRowCount)
                    iStartQueryRow = iTableRowCount+1;
                else
                    iStartQueryRow =0;
            for(int j =iStartQueryRow ;j<pDbItem->rowCount();j++){
                QStandardItem *pTableItem = pDbItem->child(j);
                if(pTableItem->text().contains(tableName)||tableName.contains(pTableItem->text())){
                    iQueryrResult =0;  //用于判断是否查询成功
                    QModelIndex tableIndex = pTableItem->index();//选到当前节点
                    ui->treeView->setCurrentIndex(tableIndex);
                    emit ui->treeView->pressed(tableIndex);//将表格的内容显示在tableView上
                    return;
                }
            }
        }
    }
    //若result仍为1，则说明查找失败
    if(iQueryrResult)
        QMessageBox::information(this,"失败","未查询到您要查找的内容");
}

void MainWindow::tableViewMenu(const QPoint &pos)//设置tableView菜单
{
    if(tableModel==nullptr||tableModel->tableName().isEmpty())
        return;
    if(bIsConnecting)
        return;
    QModelIndex curTableIndex = ui->tableView->indexAt(pos);//获取当前index
    QMenu menu;
    if(!curTableIndex.isValid()&&tableModel->rowCount()==0){
        menu.addAction(QStringLiteral("增加行"), this, SLOT(addRowMenu()));//增加行
        menu.exec(ui->tableView->mapToGlobal(pos));  //显示菜单,全局模式
        return;
    }
    if(curTableIndex.isValid()){
        menu.addAction(QStringLiteral("删除行"), this, SLOT(deleteRowMenu()));//删除行
        menu.addAction(QStringLiteral("增加行"), this, SLOT(addRowMenu()));//增加行
        menu.addAction(QStringLiteral("删除列"), this, SLOT(deleteColMenu()));//删除列
        menu.addAction(QStringLiteral("增加列"), this, SLOT(addColMenu()));//增加列
        menu.addAction(QStringLiteral("重命名该列"), this, SLOT(renameColMenu()));//重命名列
        menu.exec(ui->tableView->mapToGlobal(pos));  //显示菜单,全局模式
    }
    else
        return;
}

void MainWindow::addRowMenu()//增加行
{
    QElapsedTimer timer;
    timer.start();
    QModelIndex curIndex = ui->tableView->currentIndex();//获取当前index
    if(!curIndex.isValid()){            //如果该表格一行都没有，则插入第一行
        tableModel->insertRow(0);
        tableModel->submitAll();
        qDebug()<<"增加1行用时"<<timer.elapsed()<<"ms";
        return;
    }
    QString sTableName = tableModel->tableName();
    QItemSelectionModel * selections = ui->tableView->selectionModel();//返回当前的选择模式
    QModelIndexList selecteds = selections->selectedIndexes();
    int iCycleCount = selecteds.count()/(selecteds.at(selecteds.count()-1).column()-selecteds.at(0).column()+1);//循环次数，也就是行数
    int iStartRowCount = selecteds.at(selecteds.count()-1).row();
    cout<<"iStartRowCount:"<<iStartRowCount<<endl<<"iCycleCount:"<<iCycleCount;
    QString sDbName = tableModel->database().databaseName();
    QSqlDatabase::removeDatabase("view");
    qDebug()<<sDbName;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(sDbName);
    db.open();
    db.transaction();
    QSqlQuery query(db);
    query.exec("select * from "+sTableName);
    QSqlRecord record = query.record();

    QString sCreateTable = QString("create table "+sTableName+"TempTable(");
    qDebug()<<sCreateTable;
    for(int i=0;i<record.count();i++){
        sCreateTable += record.fieldName(i) +"  varchar,";
    }
    sCreateTable = sCreateTable.left(sCreateTable.size()-1) +")";
    cout<<sCreateTable;
    if(query.exec(sCreateTable))
        cout<<"创建表格成功";
    query.exec("select * from "+sTableName);
    QString insert = "insert into "+sTableName+"TempTable values";
    int iCurRowCount=0;
    while(query.next()){
        insert +=QString("(");
        for(int i=0;i<record.count();i++)
            insert +="'"+query.value(i).toString()+"',";
        insert = insert.left(insert.size()-1)+"),";

        if(iCurRowCount==iStartRowCount){
            insert +=QString("(");
            for(int i=0;i<record.count();i++)
                insert+="'',";
            insert = insert.left(insert.size()-1)+"),";
        }
        iCurRowCount++;
    }
    insert =insert.left(insert.size()-1);
    qDebug()<<insert;
    if(!query.exec(insert)){
        query.exec("drop table "+sTableName+"TempTable");
        cout<<"插入失败";
        return;
    }
    cout<<"插入成功";
    if(query.exec("drop table "+sTableName))
        cout<<"删除表格成功";
    query.exec("alter table "+sTableName+"TempTable rename to "+sTableName);
    db.commit();
    db.close();
    emit ui->treeView->pressed(ui->treeView->currentIndex());//发送点击信号，重新显示treeView
}

void MainWindow::addColMenu()//增加列，SQlite数据库不支持增加列，必须新建一个表格取代原有的表格
{
    QModelIndex curTableIndex = ui->tableView->currentIndex();
    int curCol =curTableIndex.column();//获取被点击的列
    QString headName = QInputDialog::getText(this,"表头名称","请输入您要插入的表头",
                                             QLineEdit::Normal);//获取新插入的表头
    QElapsedTimer time;
    time.start();
    if(headName.isEmpty())
        return;

    QString tableName = tableModel->tableName();//获取当前表格名
    class addCol *insertCol = new class addCol;
    insertCol->moveToThread(&thread);
    connect(insertCol,&addCol::finish,this,[=](){//结束线程
        qDebug()<<"线程结束";
        emit ui->treeView->pressed(ui->treeView->currentIndex());
    });
    connect(this,&MainWindow::addCol,insertCol,&addCol::dowork);//建立连接，以启动任务
    connect(insertCol,&addCol::addInformation,this,&MainWindow::recvInformation);//接收错误信息
    emit addCol(curCol,headName,tableName,getDatabase());//发送信号
    thread.start();
}

void MainWindow::deleteRowMenu()//删除行
{   
    int ok = QMessageBox::warning(this,tr("删除当前行!"),tr("你确定删除当前行吗?"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)//如果不删除，则撤销
        tableModel->revertAll();
    else{   //否则提交，在数据库中删除该行
        QElapsedTimer timer;
        timer.start();
        QItemSelectionModel * selections = ui->tableView->selectionModel();//返回当前的选择模式
        QModelIndexList selecteds = selections->selectedIndexes();
        foreach (QModelIndex index, selecteds){
        int iCurRow = index.row(); //删除所有被选中的行
        tableModel->removeRow(iCurRow);
        }
        tableModel->submitAll();//提交,删除行
        emit ui->treeView->pressed(ui->treeView->currentIndex());
        qDebug()<<"删除行用时"<<timer.elapsed()<<"ms";
    }
}

void MainWindow::renameColMenu()//列的重命名
{
    QModelIndex iCurIndex = ui->tableView->currentIndex();
    int iCurCol =iCurIndex.column();//获取被点击的列
    QString newHeadName = QInputDialog::getText(this,"表头名称","请输入您要重写的表头",
                                                QLineEdit::Normal,tableModel->headerData(iCurCol,Qt::Horizontal).toString());//获取新插入的表头
    qDebug()<<newHeadName;
    QElapsedTimer time;
    time.start();
    if(newHeadName.isEmpty()||newHeadName==tableModel->headerData(iCurCol,Qt::Horizontal).toString())//如果表头和原表头一样，或者为空，则直接返回
        return;
    QString tableName = tableModel->tableName();//获取当前表格名

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","renameHead");
    db.setDatabaseName(getDatabase());
    if(!db.open())
        return;

    db.transaction();//创建事务
    QSqlQuery query(db);
    query.exec("select * from "+tableName);
    QSqlRecord record = query.record();//获取表头

    //创建新表
    int columnCount = record.count();//获取表格的列数
    qDebug()<<columnCount;
    QString tableCreate = "create table "+tableName+"tempTAble" +"(";
    for(int i=0;i<columnCount;i++){
        if(i==iCurCol)
            tableCreate+= newHeadName +"  varchar,";
        else
            tableCreate+=record.fieldName(i) +"  varchar,";
    }
        tableCreate = tableCreate.left(tableCreate.size()-1)+")";
    if(!query.exec(tableCreate)){
        QMessageBox::warning(this,tr("Error"),tr("重命名失败,请确认表头名称是否有误"));
        return;
    }
    //插入数据
    query.prepare("select * from "+tableName);
    query.exec();
    QString insert = "insert into "+tableName+"tempTable  (";
    for(int i=0;i<columnCount;i++){
        if(i==iCurCol)
            insert+=newHeadName+" ,";
        else
            insert += (record.fieldName(i)+" ,");
    }
    insert = insert.left(insert.size()-1);
    insert+=")values";

    while (query.next()){
        insert+="(";
        for(int i=0;i<columnCount;i++)
            insert += "'"+query.value(record.fieldName(i)).toString()+"'"+",";
        insert = insert.left(insert.size()-1) ;
        insert+="),";
    }
    insert = insert.left(insert.size()-1) ;
    if(!query.exec(insert)){     //执行插入命令
        qDebug()<<insert;
        QMessageBox::warning(this,tr("Error"),tr("重命名失败,请确认表头名称是否有误"));
        return;
    }
    query.exec("drop table "+tableName);//删除旧表
    query.exec("alter table "+tableName+"tempTable rename to "+ tableName);//将临时表格改名为原表格名
    db.commit();//提交事务
    db.close();//关闭数据库
    QSqlDatabase::removeDatabase("renameHead");//断开连接
    emit ui->treeView->pressed(ui->treeView->currentIndex());//发送点击信号，重新显示treeView
    qDebug()<<"重命名本列用时"<<time.elapsed()<<"ms";
}

void MainWindow::deleteColMenu()//删除列，SQlite数据库不支持删除列，必须新建一个表格取代原有的表格
{
    int ok = QMessageBox::warning(this,tr("删除当前列!"),tr("你确定删除当前列吗?"),
                              QMessageBox::Yes,QMessageBox::No);
    if(ok ==QMessageBox::No )//如果不删除，则撤销
        return;


    QItemSelectionModel * selections = ui->tableView->selectionModel();//返回当前的选择模式
    QModelIndexList selecteds = selections->selectedIndexes();
    int iCurCol = selecteds.at(0).column();
    int iCycleCount = selecteds.at(selecteds.count()-1).column()-selecteds.at(0).column()+1;//循环次数
    if(iCycleCount == tableModel->columnCount()){//判断是不是选中了所有的列
        QMessageBox::warning(this,"Error","您不能删除所有的列");
        return;
    }
    QString sTableName = tableModel->tableName();
    //创建任务并与线程连接
    class deleteCol  *removeCol = new  class deleteCol;
    removeCol->moveToThread(&thread);
    connect(&thread,&QThread::finished,removeCol,&deleteCol::deleteLater);
    connect(this,&MainWindow::deleteCol,removeCol,&deleteCol::dowork);//建立连接，以启动任务
    connect(removeCol,&deleteCol::finish,this,[=](){
        qDebug()<<"线程结束";
        thread.quit();
        emit ui->treeView->pressed(ui->treeView->currentIndex());
    });    
    connect(removeCol,&deleteCol::deleteInfromation,this,&MainWindow::recvInformation);//接收删除列结果
    emit deleteCol(iCurCol,iCycleCount,sTableName,getDatabase());//发送信号
    thread.start();
}

inline QString MainWindow::getDatabase()//内联函数，获取数据库文件路径
{
    QString sDbFilePath = nullptr;
    QModelIndex index = ui->treeView->currentIndex();
    QStandardItem *item = treeModel.itemFromIndex(index); //获取当前item

    if(item==nullptr) {
        QMessageBox::warning(NULL,"Error","无可操作的数据库");
        return sDbFilePath;
    }
    if(item->parent())   //若item有父节点，则取父节点的data,否则取其本身的data
        sDbFilePath = item->parent()->data().toString();
    else
        sDbFilePath = item->data().toString();

    return sDbFilePath;
}

void MainWindow::on_treeView_pressed(QModelIndex modelIndex)//根据被点击的表格名称在右边显示内容
{
    QElapsedTimer timer;
    timer.start();
    QStandardItem *pTableItem =treeModel.itemFromIndex(modelIndex);
    if(!pTableItem->parent()){//判断被点击的是表格还是数据库，是表格则设置tableView的内容为空并直接返回
       tableModel = new QSqlTableModel(this);
       ui->tableView->setModel(tableModel);
       return;
    }

    QString sDbFilePath = pTableItem->parent()->data().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","view");
    db.setDatabaseName(sDbFilePath);
    db.open();

    //获取表格名
    QString sTableName = pTableItem->text();
    tableModel = new QSqlTableModel(this,db);
    tableModel->setTable(sTableName);
    tableModel->select();
    ui->tableView->setModel(tableModel);
    ui->tableView->resizeColumnsToContents();//根据内容调整行宽
    ui->tableView->resizeRowsToContents();//根据内容调整行高
    qDebug()<<"处理显示事件用时"<<timer.elapsed()<<"ms";
}

void MainWindow::on_compareTableMenu_triggered()//比较
{
    QList<QStringList> lTableNameList;
    for(int i=0;i<treeModel.rowCount();i++){
        QStringList strList;
        QStandardItem *pDbItem = treeModel.item(i);
        strList.append(pDbItem->data().toString());
        for(int j=0;j<pDbItem->rowCount();j++){
            QStandardItem *pTableItem = pDbItem->child(j);
            if(pTableItem->checkState()==Qt::Checked)
                strList.append(pTableItem->text());
        }
        lTableNameList.append(strList);
    }
    connect(this,&MainWindow::startCompare,&widget,&compResultWidget::compare);
    connect(&widget,&compResultWidget::finish,this,[=](){
        widget.show();
    });
    emit startCompare(lTableNameList);
}

void MainWindow::recvInformation(QString information,QString sParameter)//接收来自子线程的各种信息
{
    if(sParameter == "warning")
        QMessageBox::warning(NULL,"Error",information);
    if(sParameter == "information")
        QMessageBox::warning(NULL,NULL,information);
    if(sParameter == "widgetShow")
        pWidget->show();
}

void MainWindow::on_refrashAction_triggered()//刷新
{
    QElapsedTimer time;
    time.start();
    QModelIndex curTreeIndex = ui->treeView->currentIndex();
    if(curTreeIndex.isValid())
        emit ui->treeView->pressed(curTreeIndex);//手动发射点一下动作
    else
        return;
    qDebug()<<"刷新用时"<<time.elapsed()<<"ms";
}

void MainWindow::treeViewMenu(const QPoint &pos)//设置treeView的菜单
 {
    if(bIsConnecting)
        return;
    QMenu menu;
    QModelIndex curTreeIndex = ui->treeView->indexAt(pos);      //当前点击的元素的index
    if(!curTreeIndex.isValid())//判断当前treeView的index是否有效
        return;
    QStandardItem *item = treeModel.itemFromIndex(curTreeIndex);//当前的item

    if(item->parent()){//判断是表格还是数据库
        menu.addAction(QStringLiteral("重命名"), this, SLOT(onActionRename()));//为action添加槽函数,重命名表格
        menu.addAction(QStringLiteral("删除"), this, SLOT(onActionDelete()));//为action添加槽函数,删除表格
     }
    else{
        QAction *connectAction = new QAction("联机",&menu);
        connect(connectAction,&QAction::triggered,this,&MainWindow::connectDevice);
        menu.addAction(connectAction);
        if(bIsConnecting)
            connectAction->setEnabled(false);
        menu.addAction(QStringLiteral("新建"), this, SLOT(createTable()));//为action添加槽函数,新建表格
        menu.addAction(QStringLiteral("导入"), this, SLOT(importExcelMenu()));//为action添加槽函数,删除表格
    }
    menu.exec(ui->treeView->mapToGlobal(pos));  //显示菜单,全局模式
 }

void MainWindow::onActionRename()//treeView菜单"重命名"的槽函数
{
    QString oldName = tableModel->tableName();//获取原表格名
    QString newName = QInputDialog::getText(this,"重命名","请输入新表格名",QLineEdit::Normal,oldName);//获取新表格名
    QElapsedTimer time;
    time.start();
    if(newName.isEmpty()||newName==oldName)
        return;   

    //连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","rename");
    if(!getDatabase().isEmpty())
        db.setDatabaseName(getDatabase());
    if(!db.open())
        return;

    //执行重命名命令
    QString rename="alter table "+oldName+"  rename to "+newName;
    QSqlQuery query(db);
    if(!query.exec(rename)) {
        QMessageBox::warning(this,"Error","表格名不能以数字开头,且不能与已有表格名相同,请先确保表格名正确");
        return;
    }

    //将item改名
    QModelIndex index = ui->treeView->currentIndex();
    QStandardItem *item =treeModel.itemFromIndex(index);
    item->setText(newName);
    db.close();
    QSqlDatabase::removeDatabase("rename");
    qDebug()<<"重命名表格用时"<<time.elapsed()<<"ms";
}

void MainWindow::onActionDelete()//treeView菜单"删除"表格的槽函数
{
    QElapsedTimer time;//启动计时器
    time.start();

    //获取数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","delete");
    db.setDatabaseName(getDatabase());
    if(!db.open())
        return;

    //获取当前表格名
    QModelIndex curTreeIndex = ui->treeView->currentIndex();
    QStandardItem *pTableItem = treeModel.itemFromIndex(curTreeIndex);
    QString tableName = pTableItem->text();//获取表格名

    //执行删除命令
    QSqlQuery query(db);
    int ok = QMessageBox::critical(NULL,"删除表格","您确定要删除表格"+tableName+"吗？",
                                   QMessageBox::Yes|QMessageBox::No);
    if(ok==QMessageBox::Yes){
        if(!query.exec("drop table "+tableName)){
            qDebug()<<tableName;
            QMessageBox::warning(this,"Error","删除表格失败");
            qDebug()<<query.lastError();
            return;
        }
      pTableItem->parent()->removeRow(curTreeIndex.row());
    }
    db.close();
    QSqlDatabase::removeDatabase("delete");
    //展示下一个表格   
    emit ui->treeView->pressed(ui->treeView->currentIndex());
    qDebug()<<"删除表格用时"<<time.elapsed()<<"ms";
}

void MainWindow::on_defaultSetAction_triggered()//连接defauktSetting类
{
    setting.show();
}

void MainWindow::connectDevice()//联机
{   
    QElapsedTimer timer;
    timer.start();
    m_ExitFlag = false;
    bIsConnecting = true;//联机开始时，treeView菜单的联机按钮设置为不可点击

    //1.首先判断是否联机成功
    m_pComAgent = new MMSComAgent;
    if(!m_pComAgent->connectDevice(QLatin1String("192.168.6.205"))){
        cout<<"联机失败";
        QMessageBox::warning(this,"Error","联机失败");
        bIsConnecting = false;
        return;
    }
    //得到数据库
    QStandardItem * pDbItem = treeModel.itemFromIndex(ui->treeView->currentIndex());
    QString sDbFilePath = pDbItem->data().toString(); //数据库路径
    cout<<sDbFilePath;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","connectDevice");
    db.setDatabaseName(sDbFilePath);
    db.open();
    db.transaction();//创建事务
    QSqlQuery query(db);
    QInputDialog *inputDialog = new QInputDialog(this);
    QString sTableName;

    //将获取的内容存到表格，需先得到表格名,开始inputdialog事件循环
    while(1){
        bool bOk= true;
        sTableName = inputDialog->getText(this,"表格名","您连机获得的数据将被储存在表格中，请为该表格命名");
        if(sTableName.isEmpty())
            return;
        QString sCreateTable =QString("create table "+sTableName+"(dipath varchar,valueType varchar,valueName varchar,value varchar,"
                                                                "units varchar,minVal varchar,maxVal varchar,stepSize varchar)");
        if(!query.exec(sCreateTable)){
            QMessageBox::warning(this,"Error","表格创建失败,请重新命名");
            bOk = false;
        }
        if(bOk){
            inputDialog->close();
            break;
        }
    }

    //表格创建成功后添加item
    QStandardItem * pTableItem = new QStandardItem(sTableName);
    pDbItem->appendRow(pTableItem);
    ui->treeView->setModel(&treeModel);
    ui->treeView->setCurrentIndex(pTableItem->index());//选到当前节点

    //2、获取定值path (domain/doPath)
    QStringList strList = m_pComAgent->getSetValPath();

    //创建进度条用于展示目前插入数据的进度
    QWidget * widget = new QWidget(NULL);
    widget->setFixedSize(600,150);
    QProgressBar * pProgressBar = new QProgressBar(widget);
    widget->setWindowTitle("正在往"+sTableName+"中存储数据，请等待");
    pProgressBar->setFixedSize(600,50);
    pProgressBar->setOrientation(Qt::Horizontal);  // 进度条设置为水平方向
    pProgressBar->setMinimum(0);  // 最小值
    pProgressBar->setMaximum(strList.count());  // 最大值

    //停止联机按钮，用于打断联机进程
    QPushButton * stopBtn = new QPushButton("停止联机",widget);
    stopBtn->resize(100,40);
    stopBtn->move(300,80);
    connect(stopBtn,&QPushButton::clicked,this,[=](){
        int ok = QMessageBox::question(widget,"询问","已经获取的数据将被存储，您确定要停止联机吗",QMessageBox::Yes,QMessageBox::No);
        if(ok == QMessageBox::Yes)
            m_ExitFlag = true;
    });
    widget->show();

    //3、获取每个定值的属性,开始插入数据
    QString sInsert = QString("insert into "+sTableName+" values");
    for(int i=0;i<strList.count();i++){
        if(!m_ExitFlag){ //主页面关闭时将值设为true，停止联机按钮被点击时将值设为true
            QString strPath = strList.at(i);
            sInsert +="(";
            QString diPath = m_pComAgent->getSetValDiPath(strPath);//获取定值dipath
            sInsert+="'" +diPath+"'" +",";
            strPath.replace(QRegularExpression("\\$\\w\\w\\$"), ".");
            strPath.replace('$', '.');

            //1.读取定值描述(名称)
            QVariant variant = m_pComAgent->readObject(strPath + QLatin1String(".dU"),QLatin1String("DC"));
            if(variant.isValid()){
                QString valueCombine = variant.toString();
                QString valueType = valueCombine.mid(valueCombine.indexOf("(")+1,valueCombine.indexOf(")")- valueCombine.indexOf("(")-1);
                QString valueName = valueCombine.section(QRegExp("\\("),0,0);
                sInsert+="'" +valueType+"'" +","+"'" +valueName+"'" +",";
            }
            else
                sInsert+=("' ',' ',");

            //2 获取定值
            QString fullpath = strPath + QLatin1Char('.') + diPath;
            variant = m_pComAgent->readObject(fullpath,QLatin1String("SG"));
            if(variant.isValid()){
                QString value= variant.toString();
                sInsert+="'"+value+"'"+",";
            }
            else
                sInsert+=" ' ',";

            //3.读取定值量纲(单位)
            variant = m_pComAgent->readObject(strPath + QLatin1String(".units.SIUnit"),QLatin1String("CF"));
            if(variant.isValid())
                sInsert+=("'"+g_SIUnit.getSIUnit(variant.toInt())+"'"+",");
            else
                sInsert+=" ' ',";

            //4.读取最小值
            variant = m_pComAgent->readObject(strPath + QLatin1String(".minVal"),QLatin1String("CF"));
            if(variant.isValid()){
                QString minVal =variant.toString();
                sInsert+="'"+minVal+"'"+",";
            }
            else
                sInsert+="' ',";

            //5.读取最大值
            variant = m_pComAgent->readObject(strPath + QLatin1String(".maxVal"),QLatin1String("CF"));
            if(variant.isValid()){
                QString maxVal =variant.toString();
                sInsert+="'"+maxVal+"'"+",";
            }
            else
                sInsert+=" ' ',";

            //6.读取步长
            variant = m_pComAgent->readObject(strPath + QLatin1String(".stepSize"),QLatin1String("CF"));
            if(variant.isValid()){
                QString stepSize =variant.toString();
                sInsert+="'"+stepSize+"'"+"),";
            }
            else
               sInsert+=" ' '),";
            pProgressBar->setValue(i);
            qDebug()<<i;
        }

        if(m_ExitFlag||i==strList.count()-1){//如果联机被中断或者联机结束，则插入数据
            widget->close();//关闭进度条
            sInsert = sInsert.left(sInsert.size()-1);
            cout<<sInsert;
            if(!query.exec(sInsert)){
                bIsConnecting = false;
                qDebug()<<"插入失败";
                return;
            }
            db.commit();
            db.close();
            QSqlDatabase::removeDatabase("connectDevice");
            emit ui->treeView->pressed(ui->treeView->currentIndex());
            bIsConnecting = false;
            QString sInformation = QString("联机结束,共用时%1ms").arg(timer.elapsed());
            cout<<sInformation;
            QMessageBox::information(this,"联机",sInformation);
            return;
        }
    }
}

