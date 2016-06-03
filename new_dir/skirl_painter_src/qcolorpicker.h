/// @file qcolorpicker.h
/// @brief Заголовочный файл виджета выбора цвета.
/// @author Каранкевич Антон Александрович. НИИ РЭТ МГТУ им. Баумана.
///
/// Файл описывает виджет выбора цвета, реализованный в виде кнопки, окрашенной
/// в выбранный цвет.
/// @see MainWindow, ViewWindow, QFieldView, QRichTreeWidget, QColorPicker

#ifndef QCOLORPICKER_H
#define QCOLORPICKER_H

#include <QWidget>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>

/// @brief Класс кнопки выбора цвета.
///
/// Виджет выбора цвета представляет собой кнопку, окрашенную в цвет, выбранный
/// в данный момент. Кнопка унаследована от простого QPushButton. Изменен
/// sizeHint на размер 24x24, минимальный размер при размещении внутри раскладки
/// (layout) - 10x10. При нажатии на кнопку вызывается стандартное системное
/// меню выбора цвета.
class QColorPicker : public QPushButton
{
    Q_OBJECT
public:
    explicit QColorPicker(QWidget *parent = 0);

    void mouseReleaseEvent(QMouseEvent *);

    /// Возвращает выбранный в данный момент цвет.
    QColor getColor();
    /// Задает цвет кнопки.
    void setColor(QColor c);
    /// Возвращает предпочитаемый размер кнопки - 24х24. Для системных нужд.
    virtual QSize sizeHint() const;

signals:
    /// Сигнал, порождаемый при окончании выбора цвета (окно выбора цвета).
    /// Вместе с сигналом отправляется выбранный цвет.
    void colorPicked(QColor c);

public slots:

private:
    QColor  pickedColor;    ///< Выбранный цвет.
    void updatePalette();   ///< Обновление палитры в соответствии с выбранным
                            ///  цветом @ref pickedColor.

};

#endif // QCOLORPICKER_H
