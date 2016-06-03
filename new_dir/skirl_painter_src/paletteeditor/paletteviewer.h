#ifndef PALETTEVIEWER_H
#define PALETTEVIEWER_H

#include <QWidget>
#include <colorpalette.h>

#define PXSIZE 5

namespace Ui
{
    class ColorEditorForm;
    class ColorInfoForm;
}

class PaletteViewer : public QWidget
{
    Q_OBJECT
public:
    explicit PaletteViewer(QWidget *parent = 0);
    ~PaletteViewer();

    // Запоминает переданную палитру, создавая ее копию у себя
    void setRawPalette(Palette& p);
    // Возвращает копию хранящейся в виджете палитры
    Palette getRawPalette();
    void setPaletteType (int pt);
    void setPaletteSize(int s);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

signals:

public slots:

private:
    Palette palette;
    QWidget* editorWindow;
    QWidget* infoForm;
    Ui::ColorEditorForm *cf_ui;
    Ui::ColorInfoForm *ci_ui;

    // Возвращает цвет по индексу в палитре. При p=0 используется palette.
    QColor getColorByIndex(int idx, Palette* p=0);
    // Задает цвет определенного индекса. При p=0 используется palette.
    void setColorByIndex(int idx, QColor c, Palette* p=0);
    // Показывает окно информации о цвете. В качестве параметров берет
    // координаты, в которые ткнул курсор.
    void showInfoWindow(QPoint p);

private slots:
    void onApplyPressed();
    void onOkPressed();
    void onIndexCbClicked();
    void onColorCbClicked();
};

#endif // PALETTEVIEWER_H
