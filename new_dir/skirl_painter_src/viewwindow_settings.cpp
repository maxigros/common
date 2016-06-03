#include "viewwindow.h"
#include "ui_viewwindow.h"

// View window names macros
#define VW_SECTION          "ViewWindow"
#define VW_VISIBLE          "isVisible"
#define VW_SIZE             "Size"
#define VW_POSITION         "Position"
#define VW_COMBOBOX_INDEX   "ComboBoxIndex"
#define VW_IS_FLOATING      "IsWidgetFloating"
#define VW_DOCKWIDGET_POS   "DockWidgetPosition"
#define VW_DOCKWIDGET_SIZE  "DockWidgetSize"
#define VW_STATE            "State"

void ViewWindow::writeSettings()
{
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings settings;

  //------------
  // VIEW WINDOW
  //------------

  settings.beginGroup(VW_SECTION);
  settings.setValue(VW_VISIBLE, isVisible());
  if(isVisible()) // Запоминаем положение окна только если оно видимо
  {
    settings.setValue(VW_SIZE, size());
    settings.setValue(VW_POSITION, pos());
  }

  settings.setValue(VW_COMBOBOX_INDEX, ui->mode_comboBox->currentIndex());

  settings.setValue(VW_IS_FLOATING, ui->dockWidget->isFloating());
  settings.setValue(VW_DOCKWIDGET_POS, ui->dockWidget->pos());
  settings.setValue(VW_DOCKWIDGET_SIZE, ui->dockWidget->size());
  settings.setValue(VW_STATE, saveState());
  settings.endGroup();
}
//------------------------------------------------------------------------------
void ViewWindow::readSettings()
{
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings settings;

  //------------
  // VIEW WINDOW
  //------------

  settings.beginGroup(VW_SECTION);

  setVisible( settings.value(VW_VISIBLE, false).toBool() );
  resize( settings.value( VW_SIZE, QSize(800, 600) ).toSize() );
  move( settings.value( VW_POSITION, QPoint(200,200) ).toPoint() );

  ui->mode_comboBox->setCurrentIndex(
                settings.value(VW_COMBOBOX_INDEX, 0).toInt()
                                    );

  ui->dockWidget->setFloating(
                settings.value(VW_IS_FLOATING, false).toBool()
                             );
  ui->dockWidget->move(
                settings.value(VW_DOCKWIDGET_POS, QPoint(300,300)).toPoint()
                      );
  ui->dockWidget->resize(
                settings.value(VW_DOCKWIDGET_SIZE, QSize(300,300)).toSize()
                        );

  restoreState(settings.value(VW_STATE).toByteArray());
  settings.endGroup();
}
