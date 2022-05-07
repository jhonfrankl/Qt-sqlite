#include "F:/documents/QtPro/creator/settingValue/head/defaultsetting.h"
#include "ui_defaultsetting.h"

#include <QDebug>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSettings>
#define cout qDebug()<<"["<<__FILE__<<":"<<__LINE__<<"]"

defaultSetting::defaultSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::defaultSetting)
{
    ui->setupUi(this);
    QString iniFilePath = QString("F:/documents/QtPro/creator/settingValue/settingValue.ini");//路径为程序路径，文件不存在会自动创建
    settings = new QSettings(iniFilePath,QSettings::IniFormat);//新建QSettings类，并设置路径和格式。
    settings->setIniCodec("UTF-8");//设置编码格式
    ui->filePathLE->setText(settings->value("defaultSetting/defaultFP").toString());
    ui->InSpinBox->setValue(settings->value("defaultSetting/In").toString().toFloat());
    ui->IeSpinBox->setValue(settings->value("defaultSetting/Ie").toString().toFloat());
    ui->valueNameLE->setText(settings->value("defaultSetting/valueName").toString());
    ui->maxValueLE  ->setText(settings->value("defaultSetting/maxValue").toString());
    ui->minValueLE  ->setText(settings->value("defaultSetting/minValue").toString());
    ui->newDBFPLE ->setText(settings->value("defaultSetting/dbFP").toString());
    ui->unitsLE->setText(settings->value("defaultSetting/unitsValue").toString());

    connect(ui->unitsLE,&QLineEdit::textEdited,this,[=](QString text){//单位设置
        settings->setValue("defaultSetting/unitsValue",text);
    });

    connect(ui->filePathLE,&QLineEdit::textEdited,this,[=](QString text){//标准数据库文件设置
        settings->setValue("defaultSetting/defaultFP",text);
    });

    connect(ui->newDBFPLE,&QLineEdit::textEdited,this,[=](QString text){//新建数据库文件夹设置
        settings->setValue("defaultSetting/dbFP",text);
    });

    void(QDoubleSpinBox:: *pValueChangedFunction)(double) = &QDoubleSpinBox::valueChanged;//valueChanged为重载函数，需要先创建指向它的指针

    connect(ui->InSpinBox,pValueChangedFunction,this,[=](double dData){//Ie的值
       settings->setValue("defaultSetting/In",dData) ;
    });

    connect(ui->IeSpinBox,pValueChangedFunction,this,[=](double dData){//Ie的值
       settings->setValue("defaultSetting/Ie",dData) ;
    });

    connect(ui->valueNameLE,&QLineEdit::textEdited,this,[=](QString text){//定值名称
    settings->setValue("defaultSetting/valueName",text);
    });

    connect(ui->maxValueLE,&QLineEdit::textEdited,this,[=](QString text){//最大值
    settings->setValue("defaultSetting/maxValue",text);
    });

    connect(ui->minValueLE,&QLineEdit::textEdited,this,[=](QString text){//最小值
    settings->setValue("defaultSetting/minValue",text);
    });
}

defaultSetting::~defaultSetting()
{
    delete ui;
}

void defaultSetting::on_stdDbFilePathBtn_clicked()//设置标准数据库路径
{
    QString filePath = QFileDialog::getOpenFileName(this,"选择标准数据库","","Sqlite(*.db*)");
    if(filePath.isEmpty())
        return;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);
    if(!db.open()){
        QMessageBox::warning(NULL,"Error","该数据库并不能打开，请重新选择");
        return;
    };
    ui->filePathLE->setText(filePath);
    defaultFP = ui->filePathLE->text();
    settings->setValue("defaultSetting/defaultFP",filePath);
}

void defaultSetting::on_newDBFPPB_clicked()//设置新数据库存储路径
{
    QString filePath = QFileDialog::getExistingDirectory(this,"","");
    if(filePath.isEmpty())
        return;
    ui->newDBFPLE->setText(filePath);
    settings->setValue("defaultSetting/dbFP",filePath);
}

QString defaultSetting::sendDBFP()//传递标准数据库路径
{
    QString sStdDbFilePath = settings->value("defaultSetting/defaultFP").toString();
    if(sStdDbFilePath.isEmpty())//判断是否为空
        return NULL;
    QFile file(sStdDbFilePath);//判断路径是否存在
    if(!file.exists())
        return NULL;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(sStdDbFilePath);//判断数据库可否被打开
    if(db.open()){
        db.close();
        return sStdDbFilePath;
    }
    else
        return NULL;
}

QString defaultSetting::sendIn() //传递In的值
{
    return settings->value("defaultSetting/In").toString();
}

QString defaultSetting::sendIe()//传递Ie的值
{
    return settings->value("defaultSetting/Ie").toString();
}

QString defaultSetting::sendValueName()//传递定值名称
{   
    return  settings->value("defaultSetting/valueName").toString();
}

QString defaultSetting::sendMax()//传递最大值名称
{
    return  settings->value("defaultSetting/maxValue").toString();
}

QString defaultSetting::sendMin()//传递最小值
{
    return  settings->value("defaultSetting/minValue").toString();
}

QString defaultSetting::sendNewDBFP()//传递新建数据库文件夹路径
{
    QString sDbFolderFilePath = settings->value("defaultSetting/dbFP").toString();
    QFile file(sDbFolderFilePath);
    if(sDbFolderFilePath.isEmpty())
        return NULL;
    if(!file.exists())
        return NULL;
    else
        return sDbFolderFilePath;
}

QString defaultSetting::sendUnitsValueName()
{
    return settings->value("defaultSetting/unitsValue").toString();
}




