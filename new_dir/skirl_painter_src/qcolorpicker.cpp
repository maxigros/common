#include "qcolorpicker.h"

QColorPicker::QColorPicker(QWidget *parent) :
    QPushButton(parent)
{
  setFlat(true);
  setAutoFillBackground(true);
  pickedColor = Qt::black;
  setText("");
  setMinimumSize(10, 10);
  QPalette p(this->palette());
  p.setColor(QPalette::Button, pickedColor);
  setPalette(p);
}

//------------------------------------------------------------------------------
void QColorPicker::updatePalette()
{
  QPalette p(this->palette());
  p.setColor(QPalette::Button, pickedColor);
  setPalette(p);
  repaint();
}
//------------------------------------------------------------------------------
QSize QColorPicker::sizeHint() const
{
  QSize s;
  s.setWidth(24);
  s.setHeight(24);
  return s;
}
//------------------------------------------------------------------------------
QColor QColorPicker::getColor()
{
  return pickedColor;
}
//------------------------------------------------------------------------------
void QColorPicker::setColor(QColor c)
{
  pickedColor = c;
  updatePalette();
}
//------------------------------------------------------------------------------
void QColorPicker::mouseReleaseEvent(QMouseEvent *e)
{
  QColor c = QColorDialog::getColor(pickedColor,
                                    0,
                                    QString::fromUtf8("Выберите цвет")
                                    );
  if(c.isValid())
    pickedColor = c;
  updatePalette();
  emit colorPicked(pickedColor);
}
//------------------------------------------------------------------------------
