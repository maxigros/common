/********************************************************************************
** Form generated from reading UI file 'viewwindow_help.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWWINDOW_HELP_H
#define UI_VIEWWINDOW_HELP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewWindow_Help
{
public:
    QGridLayout *gridLayout;
    QPushButton *hide_button;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_19;
    QLabel *label_20;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_15;
    QLabel *label_16;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ViewWindow_Help)
    {
        if (ViewWindow_Help->objectName().isEmpty())
            ViewWindow_Help->setObjectName(QStringLiteral("ViewWindow_Help"));
        ViewWindow_Help->resize(500, 450);
        ViewWindow_Help->setMinimumSize(QSize(500, 450));
        gridLayout = new QGridLayout(ViewWindow_Help);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        hide_button = new QPushButton(ViewWindow_Help);
        hide_button->setObjectName(QStringLiteral("hide_button"));

        gridLayout->addWidget(hide_button, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        scrollArea = new QScrollArea(ViewWindow_Help);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 464, 569));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setContentsMargins(3, 3, 3, 3);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setHorizontalSpacing(3);
        formLayout->setVerticalSpacing(3);
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(24, 24));
        label_3->setPixmap(QPixmap(QString::fromUtf8(":/buttons/images/trace.gif")));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setWordWrap(true);

        formLayout->setWidget(1, QFormLayout::FieldRole, label_2);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setWordWrap(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, label_4);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setPixmap(QPixmap(QString::fromUtf8(":/buttons/images/zoom.gif")));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setPixmap(QPixmap(QString::fromUtf8(":/buttons/images/pan.gif")));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_6);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setPixmap(QPixmap(QString::fromUtf8(":/buttons/images/zoom_in.gif")));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_7);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setPixmap(QPixmap(QString::fromUtf8(":/buttons/images/zoom_out.gif")));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_8);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setWordWrap(true);

        formLayout->setWidget(3, QFormLayout::FieldRole, label_9);

        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout->setWidget(4, QFormLayout::FieldRole, label_10);

        label_11 = new QLabel(groupBox);
        label_11->setObjectName(QStringLiteral("label_11"));

        formLayout->setWidget(5, QFormLayout::FieldRole, label_11);

        label_19 = new QLabel(groupBox);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setPixmap(QPixmap(QString::fromUtf8(":/buttons/images/clear.png")));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_19);

        label_20 = new QLabel(groupBox);
        label_20->setObjectName(QStringLiteral("label_20"));

        formLayout->setWidget(6, QFormLayout::FieldRole, label_20);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout = new QVBoxLayout(groupBox_2);
        verticalLayout->setSpacing(3);
        verticalLayout->setContentsMargins(3, 3, 3, 3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setWordWrap(true);

        verticalLayout->addWidget(label_12);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setWordWrap(true);

        verticalLayout->addWidget(label_13);

        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setWordWrap(true);

        verticalLayout->addWidget(label_14);


        verticalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_2 = new QVBoxLayout(groupBox_3);
        verticalLayout_2->setSpacing(3);
        verticalLayout_2->setContentsMargins(3, 3, 3, 3);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setWordWrap(true);

        verticalLayout_2->addWidget(label_17);

        label_18 = new QLabel(groupBox_3);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setWordWrap(true);

        verticalLayout_2->addWidget(label_18);

        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setWordWrap(true);

        verticalLayout_2->addWidget(label_15);

        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setWordWrap(true);

        verticalLayout_2->addWidget(label_16);


        verticalLayout_3->addWidget(groupBox_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 3);


        retranslateUi(ViewWindow_Help);
        QObject::connect(hide_button, SIGNAL(clicked()), ViewWindow_Help, SLOT(hide()));

        QMetaObject::connectSlotsByName(ViewWindow_Help);
    } // setupUi

    void retranslateUi(QWidget *ViewWindow_Help)
    {
        ViewWindow_Help->setWindowTitle(QApplication::translate("ViewWindow_Help", "\320\237\320\276\320\274\320\276\321\211\321\214 (\320\237\320\232\320\234 - \321\201\320\272\321\200\321\213\321\202\321\214)", 0));
        hide_button->setText(QApplication::translate("ViewWindow_Help", "\320\241\320\272\321\200\321\213\321\202\321\214", 0));
        groupBox->setTitle(QApplication::translate("ViewWindow_Help", "\320\232\320\275\320\276\320\277\320\272\320\270", 0));
        label_3->setText(QString());
        label_2->setText(QApplication::translate("ViewWindow_Help", "\320\242\321\200\320\260\321\201\321\201\320\270\321\200\320\276\320\262\320\272\320\260 \320\263\321\200\320\260\321\204\320\270\320\272\320\276\320\262 (\320\262\321\213\320\261\320\276\321\200 \320\272\320\260\320\275\320\260\320\273\320\276\320\262 \320\264\320\260\320\273\321\214\320\275\320\276\321\201\321\202\320\270-\321\201\320\272\320\276\321\200\320\276\321\201\321\202\320\270) \320\273\320\265\320\262\320\276\320\271 \320\272\320\275\320\276\320\277\320\272\320\276\320\271 \320\274\321\213\321\210\320\270", 0));
        label_4->setText(QApplication::translate("ViewWindow_Help", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265 \"\321\200\320\265\320\267\320\270\320\275\320\276\320\262\320\276\320\271 \321\200\320\260\320\274\320\272\320\276\320\271\" \320\273\320\265\320\262\320\276\320\271 \320\272\320\275\320\276\320\277\320\272\320\276\320\271 \320\274\321\213\321\210\320\270, \320\262\320\276\320\267\320\262\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\272 \320\277\321\200\320\265\320\264\321\213\320\264\321\203\321\211\320\265\320\274\321\203 \320\274\320\260\321\201\321\210\321\202\320\260\320\261\321\203 - \320\277\321\200\320\260\320\262\320\276\320\271.", 0));
        label_5->setText(QString());
        label_6->setText(QString());
        label_7->setText(QString());
        label_8->setText(QString());
        label_9->setText(QApplication::translate("ViewWindow_Help", "\320\237\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265 \320\263\321\200\320\260\321\204\320\270\320\272\320\260 \320\273\320\265\320\262\320\276\320\271 \320\272\320\275\320\276\320\277\320\272\320\276\320\271 \320\274\321\213\321\210\320\270.", 0));
        label_10->setText(QApplication::translate("ViewWindow_Help", "\320\243\320\262\320\265\320\273\320\270\321\207\320\265\320\275\320\270\320\265 \320\274\320\260\321\201\321\210\321\202\320\260\320\261\320\260 \320\262\320\264\320\262\320\276\320\265.", 0));
        label_11->setText(QApplication::translate("ViewWindow_Help", "\320\243\320\274\320\265\320\275\321\214\321\210\320\265\320\275\320\270\320\265 \320\274\320\260\321\201\321\210\321\202\320\260\320\261\320\260 \320\262\320\264\320\262\320\276\320\265.", 0));
        label_19->setText(QString());
        label_20->setText(QApplication::translate("ViewWindow_Help", "\320\236\321\207\320\270\321\201\321\202\320\272\320\260 \320\275\320\260\320\272\320\276\320\277\320\273\320\265\320\275\320\275\321\213\321\205 \320\264\320\260\320\275\320\275\321\213\321\205 \320\276\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\321\217", 0));
        groupBox_2->setTitle(QApplication::translate("ViewWindow_Help", "\320\232\320\273\320\260\320\262\320\270\320\260\321\202\321\203\321\200\320\260", 0));
        label->setText(QApplication::translate("ViewWindow_Help", "\320\241\321\202\321\200\320\265\320\273\320\272\320\270 - \320\277\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265 \320\272\321\203\321\200\321\201\320\276\321\200\320\260 \321\202\321\200\320\260\321\201\321\201\320\270\321\200\320\276\320\262\320\272\320\270 (\320\262\321\213\320\261\320\276\321\200 \320\272\320\260\320\275\320\260\320\273\320\276\320\262) \321\201 \321\210\320\260\320\263\320\276\320\274 1 \320\272\320\260\320\275\320\260\320\273", 0));
        label_12->setText(QApplication::translate("ViewWindow_Help", "Ctrl+\321\201\321\202\321\200\320\265\320\273\320\272\320\270 - \320\277\320\265\321\200\320\265\320\274\320\265\321\210\320\265\320\275\320\270\320\265 \320\272\321\203\321\200\321\201\320\276\321\200\320\260 \321\202\321\200\320\260\321\201\321\201\320\270\321\200\320\276\320\262\320\272\320\270 \321\201 \321\210\320\260\320\263\320\276\320\274 10 \320\272\320\260\320\275\320\260\320\273\320\276\320\262", 0));
        label_13->setText(QApplication::translate("ViewWindow_Help", "PgUp, PgDown - \320\277\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 \320\262\321\200\320\265\320\274\320\265\320\275\320\270 \320\275\320\260 1 \320\274\320\260\321\201\321\210\321\202\320\260\320\261 (\320\262\320\262\320\265\321\200\321\205-\320\262\320\275\320\270\320\267).", 0));
        label_14->setText(QApplication::translate("ViewWindow_Help", "Ctrl+PgUp, PgDn - \320\277\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 \320\264\320\260\320\273\321\214\320\275\320\276\321\201\321\202\320\270 \320\275\320\260 \320\276\320\264\320\270\320\275 \320\274\320\260\321\201\321\210\321\202\320\260\320\261 (\320\262\320\273\320\265\320\262\320\276-\320\262\320\277\321\200\320\260\320\262\320\276)", 0));
        groupBox_3->setTitle(QApplication::translate("ViewWindow_Help", "\320\234\321\213\321\210\321\214", 0));
        label_17->setText(QApplication::translate("ViewWindow_Help", "\320\233\320\265\320\262\320\260\321\217 \320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270 - \320\262 \320\267\320\260\320\262\320\270\321\201\320\270\320\274\320\276\321\201\321\202\320\270 \320\276\321\202 \320\270\320\275\321\201\321\202\321\200\321\203\320\274\320\265\320\275\321\202\320\260", 0));
        label_18->setText(QApplication::translate("ViewWindow_Help", "\320\237\321\200\320\260\320\262\320\260\321\217 \320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270 - \320\276\321\202\320\272\320\260\321\202 \320\275\320\260 \320\277\321\200\320\265\320\264\321\213\320\264\321\203\321\211\320\270\320\271 \320\274\320\260\321\201\321\210\321\202\320\260\320\261 (\320\262 \321\200\320\265\320\266\320\270\320\274\320\265 \320\274\320\260\321\201\321\210\321\202\320\260\320\261\320\270\321\200\320\276\320\262\320\260\320\275\320\270\321\217)", 0));
        label_15->setText(QApplication::translate("ViewWindow_Help", "\320\237\321\200\320\276\320\272\321\200\321\203\321\202\320\272\320\260 \320\272\320\276\320\273\320\265\321\201\320\260 \320\274\321\213\321\210\320\270 - \320\270\320\267\320\274\320\265\320\275\320\265\320\275\320\270\320\265 \320\274\320\260\321\201\321\210\321\202\320\260\320\261\320\260", 0));
        label_16->setText(QApplication::translate("ViewWindow_Help", "\320\237\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265 \321\201 \320\267\320\260\320\266\320\260\321\202\321\213\320\274 \"\320\272\320\276\320\273\320\265\321\201\320\276\320\274\" - \320\277\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265 \320\276\320\261\320\267\320\276\321\200\320\260", 0));
    } // retranslateUi

};

namespace Ui {
    class ViewWindow_Help: public Ui_ViewWindow_Help {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWWINDOW_HELP_H
