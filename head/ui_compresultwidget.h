/********************************************************************************
** Form generated from reading UI file 'compresultwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPRESULTWIDGET_H
#define UI_COMPRESULTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_compResultWidget
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QTreeView *treeView;
    QTableWidget *tableWidget;

    void setupUi(QWidget *compResultWidget)
    {
        if (compResultWidget->objectName().isEmpty())
            compResultWidget->setObjectName(QStringLiteral("compResultWidget"));
        compResultWidget->resize(828, 593);
        gridLayout = new QGridLayout(compResultWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pushButton = new QPushButton(compResultWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMinimumSize(QSize(100, 20));
        pushButton->setMaximumSize(QSize(100, 40));

        gridLayout->addWidget(pushButton, 0, 0, 1, 1);

        treeView = new QTreeView(compResultWidget);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setMaximumSize(QSize(400, 16777215));

        gridLayout->addWidget(treeView, 1, 0, 1, 1);

        tableWidget = new QTableWidget(compResultWidget);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));

        gridLayout->addWidget(tableWidget, 1, 1, 1, 1);


        retranslateUi(compResultWidget);

        QMetaObject::connectSlotsByName(compResultWidget);
    } // setupUi

    void retranslateUi(QWidget *compResultWidget)
    {
        compResultWidget->setWindowTitle(QApplication::translate("compResultWidget", "Form", 0));
        pushButton->setText(QApplication::translate("compResultWidget", "\344\277\235\345\255\230", 0));
    } // retranslateUi

};

namespace Ui {
    class compResultWidget: public Ui_compResultWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPRESULTWIDGET_H
