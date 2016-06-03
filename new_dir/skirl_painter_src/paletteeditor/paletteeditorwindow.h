#ifndef PALETTEEDTORWINDOW_H
#define PALETTEEDTORWINDOW_H

#include <QMainWindow>
#include <qcolorpicker.h>

namespace Ui {
    class PaletteEdtorWindow;
}

class PaletteEdtorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PaletteEdtorWindow(QWidget *parent = 0);
    ~PaletteEdtorWindow();

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);

private slots:
    void on_open_action_triggered();
    void on_comboBox_currentIndexChanged(int index);
    void on_save_action_triggered();
    void on_spinBox_editingFinished();

private:
    Ui::PaletteEdtorWindow *ui;

    // Поиск индекса комбобокса по версии палитры
    int comboBoxIndex(int pv);
    // Поиск версии палитры по индексу комбобокса
    int paletteVersion(int cbIndex);
};

#endif // PALETTEEDTORWINDOW_H
