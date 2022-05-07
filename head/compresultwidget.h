#ifndef COMPRESULTWIDGET_H
#define COMPRESULTWIDGET_H
#include "head\defaultsetting.h"
#include <QTableWidget>
#include <QWidget>
#include <QStandardItemModel>
#include <QSqlTableModel>
typedef struct tableStruct
{
    QString sDbFilePath;
    QString sTabelName;
    QStringList slHeaders;
    int iRowCount;
    int iColCount;
    QStringList celldataList;
    QList<QBrush> cellColorList;
}table;

namespace Ui {
class compResultWidget;
}

class compResultWidget : public QWidget
{
    Q_OBJECT

public:
    QStandardItemModel treeModel;
    QSqlTableModel  *tableModel = nullptr;
    defaultSetting * setting = nullptr;
    QList<table> tableStructList;
    explicit compResultWidget(QWidget *parent = 0);
    void closeEvent(QCloseEvent *event) override;
    void compare(QList<QStringList> lTableNameList);
    ~compResultWidget();
signals:
    void compareInformation(QString ,QString);
    void finish();
private slots:
    void on_Treeview_Pressed(QModelIndex curIndex);
    void on_pushButton_clicked();

private:
    Ui::compResultWidget *ui;
};


#endif // COMPRESULTWIDGET_H
