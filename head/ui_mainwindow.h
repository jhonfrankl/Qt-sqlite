/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *creataDatabaseMenu;
    QAction *createTableMenu;
    QAction *openMenu;
    QAction *saveMenu;
    QAction *importExcelMenu;
    QAction *queryMenu;
    QAction *addRowMenu;
    QAction *addColMenu;
    QAction *deleteRowMenu;
    QAction *deleteColMenu;
    QAction *saveAsMenu;
    QAction *deleteTableMenu;
    QAction *compareTableMenu;
    QAction *refrashAction;
    QAction *createDBAction;
    QAction *defaultSetAction;
    QAction *connectDeviceAction;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTreeView *treeView;
    QTableView *tableView;
    QMenuBar *menubar;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *menu_P;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(980, 647);
        creataDatabaseMenu = new QAction(MainWindow);
        creataDatabaseMenu->setObjectName(QStringLiteral("creataDatabaseMenu"));
        createTableMenu = new QAction(MainWindow);
        createTableMenu->setObjectName(QStringLiteral("createTableMenu"));
        openMenu = new QAction(MainWindow);
        openMenu->setObjectName(QStringLiteral("openMenu"));
        saveMenu = new QAction(MainWindow);
        saveMenu->setObjectName(QStringLiteral("saveMenu"));
        importExcelMenu = new QAction(MainWindow);
        importExcelMenu->setObjectName(QStringLiteral("importExcelMenu"));
        queryMenu = new QAction(MainWindow);
        queryMenu->setObjectName(QStringLiteral("queryMenu"));
        addRowMenu = new QAction(MainWindow);
        addRowMenu->setObjectName(QStringLiteral("addRowMenu"));
        addColMenu = new QAction(MainWindow);
        addColMenu->setObjectName(QStringLiteral("addColMenu"));
        deleteRowMenu = new QAction(MainWindow);
        deleteRowMenu->setObjectName(QStringLiteral("deleteRowMenu"));
        deleteColMenu = new QAction(MainWindow);
        deleteColMenu->setObjectName(QStringLiteral("deleteColMenu"));
        saveAsMenu = new QAction(MainWindow);
        saveAsMenu->setObjectName(QStringLiteral("saveAsMenu"));
        deleteTableMenu = new QAction(MainWindow);
        deleteTableMenu->setObjectName(QStringLiteral("deleteTableMenu"));
        compareTableMenu = new QAction(MainWindow);
        compareTableMenu->setObjectName(QStringLiteral("compareTableMenu"));
        refrashAction = new QAction(MainWindow);
        refrashAction->setObjectName(QStringLiteral("refrashAction"));
        createDBAction = new QAction(MainWindow);
        createDBAction->setObjectName(QStringLiteral("createDBAction"));
        defaultSetAction = new QAction(MainWindow);
        defaultSetAction->setObjectName(QStringLiteral("defaultSetAction"));
        connectDeviceAction = new QAction(MainWindow);
        connectDeviceAction->setObjectName(QStringLiteral("connectDeviceAction"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        treeView = new QTreeView(centralwidget);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setMinimumSize(QSize(100, 0));
        treeView->setMaximumSize(QSize(600, 16000000));

        gridLayout->addWidget(treeView, 0, 0, 1, 1);

        tableView = new QTableView(centralwidget);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setMinimumSize(QSize(700, 0));
        tableView->setMaximumSize(QSize(16777215, 16777215));

        gridLayout->addWidget(tableView, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 980, 26));
        fileMenu = new QMenu(menubar);
        fileMenu->setObjectName(QStringLiteral("fileMenu"));
        editMenu = new QMenu(menubar);
        editMenu->setObjectName(QStringLiteral("editMenu"));
        menu_P = new QMenu(menubar);
        menu_P->setObjectName(QStringLiteral("menu_P"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(fileMenu->menuAction());
        menubar->addAction(editMenu->menuAction());
        menubar->addAction(menu_P->menuAction());
        fileMenu->addAction(createDBAction);
        fileMenu->addAction(openMenu);
        fileMenu->addAction(queryMenu);
        fileMenu->addAction(refrashAction);
        editMenu->addAction(compareTableMenu);
        menu_P->addAction(defaultSetAction);
        menu_P->addAction(connectDeviceAction);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Onlly\344\277\235\346\212\244", 0));
        creataDatabaseMenu->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\345\272\223", 0));
        createTableMenu->setText(QApplication::translate("MainWindow", "\350\241\250\346\240\274", 0));
        openMenu->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
        saveMenu->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230", 0));
        importExcelMenu->setText(QApplication::translate("MainWindow", "Excel", 0));
        queryMenu->setText(QApplication::translate("MainWindow", "\346\237\245\350\257\242", 0));
        addRowMenu->setText(QApplication::translate("MainWindow", "\345\242\236\345\212\240\350\241\214", 0));
        addColMenu->setText(QApplication::translate("MainWindow", "\345\242\236\345\212\240\345\210\227", 0));
        deleteRowMenu->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\350\241\214", 0));
        deleteColMenu->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\345\210\227", 0));
        saveAsMenu->setText(QApplication::translate("MainWindow", "\345\217\246\345\255\230\344\270\272", 0));
        deleteTableMenu->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244", 0));
        compareTableMenu->setText(QApplication::translate("MainWindow", "\345\257\271\346\257\224", 0));
        refrashAction->setText(QApplication::translate("MainWindow", "\345\210\267\346\226\260", 0));
        createDBAction->setText(QApplication::translate("MainWindow", "\346\226\260\345\273\272", 0));
        defaultSetAction->setText(QApplication::translate("MainWindow", "\351\200\211\351\241\271(O)", 0));
        connectDeviceAction->setText(QApplication::translate("MainWindow", "connect(C)", 0));
        fileMenu->setTitle(QApplication::translate("MainWindow", "\346\226\207\344\273\266(F)", 0));
        editMenu->setTitle(QApplication::translate("MainWindow", "\347\274\226\350\276\221(E)", 0));
        menu_P->setTitle(QApplication::translate("MainWindow", "\345\267\245\345\205\267(T)", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
