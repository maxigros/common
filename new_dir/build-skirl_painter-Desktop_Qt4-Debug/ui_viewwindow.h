/********************************************************************************
** Form generated from reading UI file 'viewwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWWINDOW_H
#define UI_VIEWWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include "qfieldview.h"
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_ViewWindow
{
public:
    QAction *openFile_action;
    QAction *clear_action;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QFieldView *field_widget;
    QComboBox *mode_comboBox;
    QLabel *range_label;
    QLabel *time_label;
    QSpinBox *range_spinBox;
    QSpinBox *time_spinBox;
    QLineEdit *realTime_edit;
    QLabel *label_3;
    QLineEdit *realRange_edit;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *realAngle_edit;
    QWidget *toolWidget;
    QHBoxLayout *horizontalLayout;
    QToolButton *trace_toolButton;
    QToolButton *zoom_toolButton;
    QToolButton *pan_toolButton;
    QToolButton *halfScale_button;
    QToolButton *doubleScale_button;
    QLabel *label_5;
    QSlider *thr_slider;
    QLabel *thr_label;
    QSpacerItem *horizontalSpacer;
    QToolButton *clear_toolButton;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *help_button;
    QLabel *label_4;
    QLineEdit *flags_edit;
    QMenuBar *menubar;
    QMenu *menu;
    QStatusBar *statusbar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QwtPlot *range_plot;
    QwtPlot *time_plot;
    QPushButton *resetScale_button;
    QSpacerItem *horizontalSpacer_3;
    QToolButton *gTrace_toolButton;
    QToolButton *gZoom_toolButton;
    QLineEdit *trackerX_edit;
    QLineEdit *trackerY_edit;

    void setupUi(QMainWindow *ViewWindow)
    {
        if (ViewWindow->objectName().isEmpty())
            ViewWindow->setObjectName(QString::fromUtf8("ViewWindow"));
        ViewWindow->resize(890, 676);
        openFile_action = new QAction(ViewWindow);
        openFile_action->setObjectName(QString::fromUtf8("openFile_action"));
        clear_action = new QAction(ViewWindow);
        clear_action->setObjectName(QString::fromUtf8("clear_action"));
        centralwidget = new QWidget(ViewWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setSpacing(3);
        gridLayout_2->setContentsMargins(3, 3, 3, 3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        field_widget = new QFieldView(centralwidget);
        field_widget->setObjectName(QString::fromUtf8("field_widget"));
        field_widget->setMinimumSize(QSize(100, 100));

        gridLayout_2->addWidget(field_widget, 3, 0, 1, 9);

        mode_comboBox = new QComboBox(centralwidget);
        mode_comboBox->setObjectName(QString::fromUtf8("mode_comboBox"));

        gridLayout_2->addWidget(mode_comboBox, 0, 0, 1, 4);

        range_label = new QLabel(centralwidget);
        range_label->setObjectName(QString::fromUtf8("range_label"));

        gridLayout_2->addWidget(range_label, 0, 4, 1, 1);

        time_label = new QLabel(centralwidget);
        time_label->setObjectName(QString::fromUtf8("time_label"));

        gridLayout_2->addWidget(time_label, 0, 6, 1, 1);

        range_spinBox = new QSpinBox(centralwidget);
        range_spinBox->setObjectName(QString::fromUtf8("range_spinBox"));
        range_spinBox->setReadOnly(true);

        gridLayout_2->addWidget(range_spinBox, 0, 5, 1, 1);

        time_spinBox = new QSpinBox(centralwidget);
        time_spinBox->setObjectName(QString::fromUtf8("time_spinBox"));
        time_spinBox->setReadOnly(true);

        gridLayout_2->addWidget(time_spinBox, 0, 7, 1, 1);

        realTime_edit = new QLineEdit(centralwidget);
        realTime_edit->setObjectName(QString::fromUtf8("realTime_edit"));
        realTime_edit->setReadOnly(true);

        gridLayout_2->addWidget(realTime_edit, 1, 7, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 1, 6, 1, 1);

        realRange_edit = new QLineEdit(centralwidget);
        realRange_edit->setObjectName(QString::fromUtf8("realRange_edit"));
        realRange_edit->setReadOnly(true);

        gridLayout_2->addWidget(realRange_edit, 1, 5, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 4, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        realAngle_edit = new QLineEdit(centralwidget);
        realAngle_edit->setObjectName(QString::fromUtf8("realAngle_edit"));
        realAngle_edit->setReadOnly(true);

        gridLayout_2->addWidget(realAngle_edit, 1, 3, 1, 1);

        toolWidget = new QWidget(centralwidget);
        toolWidget->setObjectName(QString::fromUtf8("toolWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolWidget->sizePolicy().hasHeightForWidth());
        toolWidget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(toolWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(1, 1, 1, 1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        trace_toolButton = new QToolButton(toolWidget);
        trace_toolButton->setObjectName(QString::fromUtf8("trace_toolButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/buttons/images/trace.gif"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/buttons/images/trace_active.gif"), QSize(), QIcon::Normal, QIcon::On);
        trace_toolButton->setIcon(icon);
        trace_toolButton->setIconSize(QSize(24, 24));
        trace_toolButton->setCheckable(true);
        trace_toolButton->setAutoRaise(true);

        horizontalLayout->addWidget(trace_toolButton);

        zoom_toolButton = new QToolButton(toolWidget);
        zoom_toolButton->setObjectName(QString::fromUtf8("zoom_toolButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/buttons/images/zoom.gif"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/buttons/images/zoom_active.gif"), QSize(), QIcon::Normal, QIcon::On);
        zoom_toolButton->setIcon(icon1);
        zoom_toolButton->setIconSize(QSize(24, 24));
        zoom_toolButton->setCheckable(true);
        zoom_toolButton->setAutoRaise(true);

        horizontalLayout->addWidget(zoom_toolButton);

        pan_toolButton = new QToolButton(toolWidget);
        pan_toolButton->setObjectName(QString::fromUtf8("pan_toolButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/buttons/images/pan.gif"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/buttons/images/pan_active.gif"), QSize(), QIcon::Normal, QIcon::On);
        pan_toolButton->setIcon(icon2);
        pan_toolButton->setIconSize(QSize(24, 24));
        pan_toolButton->setCheckable(true);
        pan_toolButton->setAutoRaise(true);

        horizontalLayout->addWidget(pan_toolButton);

        halfScale_button = new QToolButton(toolWidget);
        halfScale_button->setObjectName(QString::fromUtf8("halfScale_button"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/buttons/images/zoom_out.gif"), QSize(), QIcon::Normal, QIcon::Off);
        halfScale_button->setIcon(icon3);
        halfScale_button->setIconSize(QSize(24, 24));
        halfScale_button->setAutoRaise(true);

        horizontalLayout->addWidget(halfScale_button);

        doubleScale_button = new QToolButton(toolWidget);
        doubleScale_button->setObjectName(QString::fromUtf8("doubleScale_button"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/buttons/images/zoom_in.gif"), QSize(), QIcon::Normal, QIcon::Off);
        doubleScale_button->setIcon(icon4);
        doubleScale_button->setIconSize(QSize(24, 24));
        doubleScale_button->setAutoRaise(true);

        horizontalLayout->addWidget(doubleScale_button);

        label_5 = new QLabel(toolWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout->addWidget(label_5);

        thr_slider = new QSlider(toolWidget);
        thr_slider->setObjectName(QString::fromUtf8("thr_slider"));
        thr_slider->setMaximum(200);
        thr_slider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(thr_slider);

        thr_label = new QLabel(toolWidget);
        thr_label->setObjectName(QString::fromUtf8("thr_label"));

        horizontalLayout->addWidget(thr_label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        clear_toolButton = new QToolButton(toolWidget);
        clear_toolButton->setObjectName(QString::fromUtf8("clear_toolButton"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/buttons/images/clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        clear_toolButton->setIcon(icon5);
        clear_toolButton->setIconSize(QSize(24, 24));
        clear_toolButton->setAutoRaise(true);

        horizontalLayout->addWidget(clear_toolButton);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        help_button = new QToolButton(toolWidget);
        help_button->setObjectName(QString::fromUtf8("help_button"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/buttons/images/question.gif"), QSize(), QIcon::Normal, QIcon::Off);
        help_button->setIcon(icon6);
        help_button->setIconSize(QSize(24, 24));
        help_button->setAutoRaise(true);

        horizontalLayout->addWidget(help_button);


        gridLayout_2->addWidget(toolWidget, 2, 0, 1, 8);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 1, 0, 1, 1);

        flags_edit = new QLineEdit(centralwidget);
        flags_edit->setObjectName(QString::fromUtf8("flags_edit"));

        gridLayout_2->addWidget(flags_edit, 1, 1, 1, 1);

        ViewWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ViewWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 890, 20));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        ViewWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ViewWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ViewWindow->setStatusBar(statusbar);
        dockWidget = new QDockWidget(ViewWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        range_plot = new QwtPlot(dockWidgetContents);
        range_plot->setObjectName(QString::fromUtf8("range_plot"));
        range_plot->setMinimumSize(QSize(200, 100));

        gridLayout->addWidget(range_plot, 2, 0, 1, 7);

        time_plot = new QwtPlot(dockWidgetContents);
        time_plot->setObjectName(QString::fromUtf8("time_plot"));
        time_plot->setMinimumSize(QSize(200, 100));

        gridLayout->addWidget(time_plot, 3, 0, 1, 7);

        resetScale_button = new QPushButton(dockWidgetContents);
        resetScale_button->setObjectName(QString::fromUtf8("resetScale_button"));

        gridLayout->addWidget(resetScale_button, 0, 5, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 4, 1, 1);

        gTrace_toolButton = new QToolButton(dockWidgetContents);
        gTrace_toolButton->setObjectName(QString::fromUtf8("gTrace_toolButton"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/buttons/images/cross.gif"), QSize(), QIcon::Normal, QIcon::Off);
        icon7.addFile(QString::fromUtf8(":/buttons/images/cross_active.gif"), QSize(), QIcon::Normal, QIcon::On);
        gTrace_toolButton->setIcon(icon7);
        gTrace_toolButton->setIconSize(QSize(24, 24));
        gTrace_toolButton->setCheckable(true);
        gTrace_toolButton->setChecked(true);
        gTrace_toolButton->setAutoExclusive(true);
        gTrace_toolButton->setAutoRaise(true);

        gridLayout->addWidget(gTrace_toolButton, 0, 0, 1, 1);

        gZoom_toolButton = new QToolButton(dockWidgetContents);
        gZoom_toolButton->setObjectName(QString::fromUtf8("gZoom_toolButton"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/buttons/images/magnifier.gif"), QSize(), QIcon::Normal, QIcon::Off);
        icon8.addFile(QString::fromUtf8(":/buttons/images/magnifier_active.gif"), QSize(), QIcon::Normal, QIcon::On);
        gZoom_toolButton->setIcon(icon8);
        gZoom_toolButton->setIconSize(QSize(24, 24));
        gZoom_toolButton->setCheckable(true);
        gZoom_toolButton->setAutoExclusive(true);
        gZoom_toolButton->setAutoRaise(true);

        gridLayout->addWidget(gZoom_toolButton, 0, 1, 1, 1);

        trackerX_edit = new QLineEdit(dockWidgetContents);
        trackerX_edit->setObjectName(QString::fromUtf8("trackerX_edit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(trackerX_edit->sizePolicy().hasHeightForWidth());
        trackerX_edit->setSizePolicy(sizePolicy1);
        trackerX_edit->setReadOnly(true);

        gridLayout->addWidget(trackerX_edit, 0, 2, 1, 1);

        trackerY_edit = new QLineEdit(dockWidgetContents);
        trackerY_edit->setObjectName(QString::fromUtf8("trackerY_edit"));
        sizePolicy1.setHeightForWidth(trackerY_edit->sizePolicy().hasHeightForWidth());
        trackerY_edit->setSizePolicy(sizePolicy1);
        trackerY_edit->setReadOnly(true);

        gridLayout->addWidget(trackerY_edit, 0, 3, 1, 1);

        dockWidget->setWidget(dockWidgetContents);
        ViewWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(8), dockWidget);

        menubar->addAction(menu->menuAction());
        menu->addAction(openFile_action);
        menu->addSeparator();
        menu->addAction(clear_action);

        retranslateUi(ViewWindow);

        mode_comboBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(ViewWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ViewWindow)
    {
        ViewWindow->setWindowTitle(QApplication::translate("ViewWindow", "\320\236\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        openFile_action->setText(QApplication::translate("ViewWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214 \321\204\320\260\320\271\320\273 \320\264\320\260\320\275\320\275\321\213\321\205", 0, QApplication::UnicodeUTF8));
        clear_action->setText(QApplication::translate("ViewWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 \320\264\320\260\320\275\320\275\321\213\320\265", 0, QApplication::UnicodeUTF8));
        mode_comboBox->clear();
        mode_comboBox->insertItems(0, QStringList()
         << QApplication::translate("ViewWindow", "\320\237\321\200\321\217\320\274\320\276\321\203\320\263\320\276\320\273\321\214\320\275\320\260\321\217 \321\200\320\260\320\267\320\262\320\265\321\200\321\202\320\272\320\260", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ViewWindow", "\320\232\321\200\321\203\320\263\320\276\320\262\320\260 \321\200\320\260\320\267\320\262\320\265\321\200\321\202\320\272\320\260, \320\230\320\232\320\236", 0, QApplication::UnicodeUTF8)
        );
        range_label->setText(QApplication::translate("ViewWindow", "\320\232\320\260\320\275\320\260\320\273 \320\264\320\260\320\273\321\214\320\275\320\276\321\201\321\202\320\270", 0, QApplication::UnicodeUTF8));
        time_label->setText(QApplication::translate("ViewWindow", "\320\232\320\260\320\275\320\260\320\273 \320\262\321\200\320\265\320\274\320\265\320\275\320\270", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ViewWindow", "\320\240\320\265\320\260\320\273\321\214\320\275\320\276\320\265 \320\262\321\200\320\265\320\274\321\217", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ViewWindow", "\320\240\320\265\320\260\320\273\321\214\320\275\320\260\321\217 \320\264\320\260\320\273\321\214\320\275\320\276\321\201\321\202\321\214", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ViewWindow", "\320\243\320\263\320\276\320\273", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        trace_toolButton->setToolTip(QApplication::translate("ViewWindow", "\320\242\321\200\320\260\321\201\321\201\320\270\321\200\320\276\320\262\320\272\320\260 \321\202\320\276\321\207\320\265\320\272", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        trace_toolButton->setText(QApplication::translate("ViewWindow", "\320\242\321\200\320\260\321\201\321\201\320\270\321\200\320\276\320\262\320\272\320\260", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        zoom_toolButton->setToolTip(QApplication::translate("ViewWindow", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        zoom_toolButton->setText(QApplication::translate("ViewWindow", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pan_toolButton->setToolTip(QApplication::translate("ViewWindow", "\320\237\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pan_toolButton->setText(QApplication::translate("ViewWindow", "\320\237\320\265\321\200\320\265\320\274\320\265\321\211\320\265\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        halfScale_button->setToolTip(QApplication::translate("ViewWindow", "\320\243\320\274\320\265\320\275\321\214\321\210\320\265\320\275\320\270\320\265 \320\274\320\260\321\201\321\210\321\202\320\260\320\261\320\260", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        halfScale_button->setText(QApplication::translate("ViewWindow", "/2", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        doubleScale_button->setToolTip(QApplication::translate("ViewWindow", "\320\243\320\262\320\265\320\273\320\270\321\207\320\265\320\275\320\270\320\265 \320\274\320\260\321\201\321\210\321\202\320\260\320\261\320\260", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        doubleScale_button->setText(QApplication::translate("ViewWindow", "*2", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ViewWindow", "\320\237\320\276\321\200\320\276\320\263 \320\276\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\321\217: ", 0, QApplication::UnicodeUTF8));
        thr_label->setText(QApplication::translate("ViewWindow", "0", 0, QApplication::UnicodeUTF8));
        clear_toolButton->setText(QApplication::translate("ViewWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        help_button->setText(QApplication::translate("ViewWindow", "?", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ViewWindow", "\320\244\320\273\320\260\320\263\320\270", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("ViewWindow", "\320\244\320\260\320\271\320\273", 0, QApplication::UnicodeUTF8));
        dockWidget->setWindowTitle(QApplication::translate("ViewWindow", "\320\223\321\200\320\260\321\204\320\270\320\272\320\270", 0, QApplication::UnicodeUTF8));
        resetScale_button->setText(QApplication::translate("ViewWindow", "\320\241\320\261\321\200\320\276\321\201\320\270\321\202\321\214 \320\274\320\260\321\201\321\210\321\202\320\260\320\261", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        gTrace_toolButton->setToolTip(QApplication::translate("ViewWindow", "\320\242\321\200\320\260\321\201\321\201\320\270\321\200\320\276\320\262\320\272\320\260 \320\263\321\200\320\260\321\204\320\270\320\272\320\276\320\262", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        gTrace_toolButton->setText(QApplication::translate("ViewWindow", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        gZoom_toolButton->setToolTip(QApplication::translate("ViewWindow", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        gZoom_toolButton->setText(QApplication::translate("ViewWindow", "...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ViewWindow: public Ui_ViewWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWWINDOW_H
