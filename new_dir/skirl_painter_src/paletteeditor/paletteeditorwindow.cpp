#include "paletteeditorwindow.h"
#include "ui_paletteeditorwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

PaletteEdtorWindow::PaletteEdtorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PaletteEdtorWindow)
{
    ui->setupUi(this);

    // Иконки на action'ы
    ui->open_action->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->save_action->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->exit_action->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    ui->new_action->setIcon(style()->standardIcon(QStyle::SP_FileIcon));

    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
    on_spinBox_editingFinished();
}
//------------------------------------------------------------------------------
PaletteEdtorWindow::~PaletteEdtorWindow()
{
    delete ui;
}
//------------------------------------------------------------------------------
void PaletteEdtorWindow::resizeEvent(QResizeEvent *)
{
//    ui->scrollArea->widget()->resize(ui->scrollArea->size());
}
//------------------------------------------------------------------------------
void PaletteEdtorWindow::closeEvent(QCloseEvent *)
{
    exit(0);
}
//------------------------------------------------------------------------------
void PaletteEdtorWindow::on_open_action_triggered()
{
    QString path = QFileDialog::getOpenFileName(
        0,
        QString::fromUtf8("Выберите файл палитры"),
        ".",
        QString::fromUtf8("Файлы палитры (*.pf *.pal);;Все файлы(*)")
        );

    if(path.isEmpty())
        return;

    QFile F(path);
    if(!F.open(QIODevice::ReadOnly))
        return;
    Palette p;
    F.read((char*)&p.header, sizeof(p.header));
    int pxsize = p.header.version / 100;
    if((pxsize < 0) || (pxsize > 4))
    {
        F.close();
        QMessageBox::critical(
            0,
            QString::fromUtf8("Ошибка открытия файла"),
            QString::fromUtf8("Файл палитры поврежден или неверен")
            );
        return;
    }
    p.data = new uint8_t[p.header.numColors*pxsize];
    F.read((char*)p.data, p.header.numColors*pxsize);
    F.close();

    int index = comboBoxIndex(p.header.version);
    ui->comboBox->setCurrentIndex(index);
    ui->spinBox->setValue(p.header.numColors);

    ui->paletteWidget->setRawPalette(p);
    ui->paletteWidget->repaint();
    delete [] p.data;
}
//------------------------------------------------------------------------------
void PaletteEdtorWindow::on_save_action_triggered()
{
    QString path = QFileDialog::getSaveFileName(
        0,
        QString::fromUtf8("Выберите файл палитры"),
        ".",
        QString::fromUtf8("Файлы палитры (*.pf *.pal);;Все файлы(*)")
        );

    if(path.isEmpty())
        return;

    QFile F(path);
    if(!F.open(QIODevice::WriteOnly))
        return;

    Palette p = ui->paletteWidget->getRawPalette();
    int pxsize = p.header.version / 100;
    if((pxsize < 0) || (pxsize > 4))
    {
        F.close();
        QMessageBox::critical(
            0,
            QString::fromUtf8("Ошибка открытия файла"),
            QString::fromUtf8("Файл палитры поврежден или неверен")
            );
        return;
    }

    F.write((char*)&p.header, sizeof(p.header));
    F.write((char*)p.data, p.header.numColors*pxsize);

    F.close();
    delete [] p.data;
}
//------------------------------------------------------------------------------
void PaletteEdtorWindow::on_comboBox_currentIndexChanged(int index)
{
    int pv = paletteVersion(index);
    ui->paletteWidget->setPaletteType(pv);
    ui->paletteWidget->repaint();
}
//------------------------------------------------------------------------------
int PaletteEdtorWindow::comboBoxIndex(int pv)
{
    switch(pv)
    {
        case PV_2RGB:
            return 0;
        case PV_2RGGB:
            return 1;
        case PV_3RGB:
            return 2;
        case PV_4RGB0:
            return 3;
        case PV_4RGBF:
            return 4;
        case PV_4RGBA:
            return 5;
        default:
            return 3;
    }
}
//------------------------------------------------------------------------------
int PaletteEdtorWindow::paletteVersion(int cbIndex)
{
    switch(cbIndex)
    {
        case 0:
            return PV_2RGB;
        case 1:
            return PV_2RGGB;
        case 2:
            return PV_3RGB;
        case 3:
            return PV_4RGB0;
        case 4:
            return PV_4RGBF;
        case 5:
            return PV_4RGBA;
        default:
            return PV_4RGB0;
    }
}
//------------------------------------------------------------------------------
void PaletteEdtorWindow::on_spinBox_editingFinished()
{
    ui->paletteWidget->setPaletteSize(ui->spinBox->value());
    ui->paletteWidget->repaint();
}
