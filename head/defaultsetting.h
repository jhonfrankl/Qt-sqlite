#ifndef DEFAULTSETTING_H
#define DEFAULTSETTING_H

#include <QWidget>
#include <QSettings>
namespace Ui {
class defaultSetting;
}

class defaultSetting : public QWidget
{
    Q_OBJECT

public:
    explicit defaultSetting(QWidget *parent = nullptr);
    QString defaultFP;
    ~defaultSetting();
    QSettings *settings;
    QString sendDBFP();
    QString sendIn();
    QString sendIe();
    QString sendimportMethord();
    QString sendValueName();
    QString sendMax();
    QString sendMin();
    QString sendNewDBFP();
    QString sendUnitsValueName();
private slots:
    void on_stdDbFilePathBtn_clicked();
    void on_newDBFPPB_clicked();

private:
    Ui::defaultSetting *ui;
};

#endif // DEFAULTSETTING_H
