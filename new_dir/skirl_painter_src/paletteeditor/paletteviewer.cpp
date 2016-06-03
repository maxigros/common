#include "paletteviewer.h"
#include "ui_ColorEditor.h"
#include "ui_colorinfo.h"

#include <QPainter>
#include <QMouseEvent>

PaletteViewer::PaletteViewer(QWidget *parent) :
    QWidget(parent)
{
    memset(&palette, 0, sizeof(palette));

    //
    //  Окно выбора цвета
    //
    editorWindow = new QWidget(0);
    cf_ui = new Ui::ColorEditorForm;
    cf_ui->setupUi(editorWindow);
    editorWindow->hide();

    connect(cf_ui->apply_button, SIGNAL(clicked()), SLOT(onApplyPressed()));
    connect(cf_ui->ok_button, SIGNAL(clicked()), SLOT(onOkPressed()));
    connect(cf_ui->isEndIndex_cb, SIGNAL(toggled(bool)),
                                  SLOT(onIndexCbClicked()));
    connect(cf_ui->isEndColor_cb, SIGNAL(toggled(bool)),
                                  SLOT(onColorCbClicked()));

    // Окно информации о цвете
    infoForm = new QWidget(0);
    ci_ui = new Ui::ColorInfoForm;
    ci_ui->setupUi(infoForm);
    infoForm->hide();
    infoForm->setWindowFlags(Qt::SplashScreen | Qt::X11BypassWindowManagerHint
                             | Qt::WindowStaysOnTopHint);
    infoForm->setFocusPolicy(Qt::NoFocus);
}
//------------------------------------------------------------------------------
PaletteViewer::~PaletteViewer()
{
    delete cf_ui;
    delete editorWindow;

    delete ci_ui;
    delete infoForm;
}
//------------------------------------------------------------------------------
void PaletteViewer::setRawPalette(Palette &p)
{
    memcpy( &palette.header, &p.header, sizeof(palette.header) );
    int psize = (palette.header.version / 100) * p.header.numColors;

    palette.data = new uint8_t[psize];
    memcpy(palette.data, p.data, psize);
    resizeEvent(0);
}
//------------------------------------------------------------------------------
Palette PaletteViewer::getRawPalette()
{
    Palette p;
    memcpy( &p.header, &palette.header, sizeof(palette.header) );
    int psize = (palette.header.version / 100) * p.header.numColors;

    p.data = new uint8_t[psize];
    memcpy(p.data, palette.data, psize);
    return p;
}
//------------------------------------------------------------------------------
void PaletteViewer::paintEvent(QPaintEvent *)
{
    int W = width();
    int H = height();

    QPainter p(this);
    p.setPen(Qt::NoPen);

    if(palette.data == 0)
        return;

    int num_w = W / PXSIZE;
    int num_h = H / PXSIZE;

    // Рисуем прямоугольники со сплошной заливкой и без каймы
    for(int y=0; y<num_h; y++)
    for(int x=0; x<num_w; x++)  // Цикл по элементам палитры
    {
        int index = y*num_w + x;
        if(index>palette.header.numColors-1)
            break;
        // Цвет заполнения
        p.setBrush(QBrush(getColorByIndex(index)));
        p.drawRect(x*PXSIZE, y*PXSIZE, PXSIZE, PXSIZE);
    }
}
//------------------------------------------------------------------------------
void PaletteViewer::resizeEvent(QResizeEvent *)
{
    int W = width();
    int px_w = W / PXSIZE;
    if(px_w == 0)
        return;
    setMinimumHeight((palette.header.numColors / px_w + 1)*PXSIZE);
}
//------------------------------------------------------------------------------
void PaletteViewer::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        showInfoWindow(e->pos());
    }
}
//------------------------------------------------------------------------------
void PaletteViewer::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        showInfoWindow(e->pos());
    }
}
//------------------------------------------------------------------------------
void PaletteViewer::mouseReleaseEvent(QMouseEvent *e)
{
    infoForm->hide();
    if(e->button() == Qt::LeftButton)
    {
        int x = e->pos().x() / PXSIZE;
        int y = e->pos().y() / PXSIZE;
        int px_w = width() / PXSIZE;
        int idx = y*px_w + x;
        if((idx < 0) || (idx >= palette.header.numColors))
            return;
        QColor c = getColorByIndex(idx);
        cf_ui->startColor_cp->setColor(c);
        cf_ui->endColor_cp->setColor(c);
        cf_ui->startIndex_sb->setValue(idx);
        cf_ui->startIndex_sb->setMaximum(palette.header.numColors-1);
        cf_ui->endIndex_sb->setValue(idx);
        cf_ui->endIndex_sb->setMaximum(palette.header.numColors-1);
        editorWindow->show();
        onIndexCbClicked();
        onColorCbClicked();
        editorWindow->raise();
    }
}
//------------------------------------------------------------------------------
QColor PaletteViewer::getColorByIndex(int idx, Palette *p)
{
    if(p == 0)
        p = &palette;
    int r=0, g=0, b=0, a=255;    // Компоненты цвета
    int px = 0; // Пиксель
    int pxsize = p->header.version / 100; // Размер пикселя
    if((pxsize < 0) || (pxsize > 4))
        return QColor(0,0,0);
    if((idx < 0) || (idx >= p->header.numColors))
        return QColor(0,0,0);
    memcpy(&px, p->data + idx*pxsize, pxsize);

    switch (p->header.version)
    {
        case PV_2RGB:
            r = (px >> 10) & 0x1F;
            r = r*0xFF/0x1F;    // Нормировка до 0xFF
            g = (px >> 5) & 0x1F;
            g = g*0xFF/0x1F;
            b = px & 0x1F;
            b = b*0xFF/0x1F;
            break;
        case PV_2RGGB:
            r = (px >> 11) & 0x1F;
            r = r*0xFF/0x1F;
            g = (px >> 5) & 0x3F;
            g = g*0xFF/0x3F;
            b = px & 0x1F;
            b = b*0xFF/0x1F;
            break;
        case PV_3RGB:
            r = (px >> 16) & 0xFF;
            g = (px >> 8) & 0xFF;
            b = px & 0xFF;
            break;
        case PV_4RGBA:
        case PV_4RGBF:
            a = (px >> 24) & 0xFF;
            r = (px >> 16) & 0xFF;
            g = (px >> 8) & 0xFF;
            b = px & 0xFF;
            break;
        case PV_4RGB0:
            r = (px >> 16) & 0xFF;
            g = (px >> 8) & 0xFF;
            b = px & 0xFF;
            break;
        default:
            r = 0;
            g = 0;
            b = 0;
            a = 0xFF;
    }

    return QColor(r, g, b, a);
}
//------------------------------------------------------------------------------
void PaletteViewer::setColorByIndex(int idx, QColor c, Palette *p)
{
    if(p == 0)
        p = &palette;

    int r = c.red();
    int g = c.green();
    int b = c.blue();
    int a = c.alpha();

    uint32_t px = 0; // Пиксель
    int pxsize = p->header.version / 100; // Размер пикселя
    if((pxsize < 0) || (pxsize > 4))
        return;
    if((idx < 0) || (idx >= p->header.numColors))
        return;

    switch(p->header.version)
    {
        case PV_2RGB:
            r = r * 0x1F/0xFF;
            px |= (r << 10)&0x7C00;
            g = g * 0x1F/0xFF;
            px |= (g << 5)&0x3E0;
            b = b * 0x1F/0xFF;
            px |= b & 0x1F;
            break;
        case PV_2RGGB:
            r = r * 0x1F/0xFF;
            px |= (r << 11)&0xF800;
            g = g * 0x3F/0xFF;
            px |= (g << 5)&0x7E0;
            b = b * 0x1F/0xFF;
            px |= b & 0x1F;
            break;
        case PV_3RGB:
            px |= (r<<16) & 0xFF0000;
            px |= (g<<8) & 0xFF00;
            px |= b & 0xFF;
            break;
        case PV_4RGBA:
            px |= (a << 24) & 0xFF000000;
            px |= (r<<16) & 0xFF0000;
            px |= (g<<8) & 0xFF00;
            px |= b & 0xFF;
        case PV_4RGBF:
            px |= (r<<16) & 0xFF0000;
            px |= (g<<8) & 0xFF00;
            px |= b & 0xFF;
            px |= 0xFF000000;
            break;
        case PV_4RGB0:
            px |= (r<<16) & 0xFF0000;
            px |= (g<<8) & 0xFF00;
            px |= b & 0xFF;
            px &= 0x00FFFFFF;
            break;
        default:
            px = 0;
    }

    memcpy(p->data + idx*pxsize, &px, pxsize);
}
//------------------------------------------------------------------------------
void PaletteViewer::setPaletteType(int pt)
{
    if(pt == palette.header.version)
        return;

    Palette p2;
    Palette* p2p = &p2;
    p2.header.ID[0] = 'P';
    p2.header.ID[1] = 'F';
    p2.header.numColors = palette.header.numColors;
    p2.header.version = pt;

    int pxsize = p2.header.version / 100;
    p2.data = new uint8_t[p2.header.numColors * pxsize];

    for(int a=0; (a<palette.header.numColors) && (a<p2.header.numColors); a++)
    {
        QColor c = getColorByIndex(a);
        setColorByIndex(a, c, p2p);
    }

    delete [] palette.data;
    memcpy(&palette, p2p, sizeof(Palette)); // Скопируется вместе с указателем
}
//------------------------------------------------------------------------------
void PaletteViewer::setPaletteSize(int s)
{
    if(s == palette.header.numColors)
        return;

    int pxsize = palette.header.version / 100;
    uint8_t* newdata = new uint8_t[pxsize * s];
    memset(newdata, 0, pxsize*s);
    int cpsize;
    if(s < palette.header.numColors)
        cpsize = s * pxsize;
    else
        cpsize = palette.header.numColors * pxsize;

    memcpy(newdata, palette.data, cpsize);
    delete [] palette.data;
    palette.data = newdata;
    palette.header.numColors = s;

    resizeEvent(0);
}
//------------------------------------------------------------------------------
void PaletteViewer::onApplyPressed()
{
    int start_idx = cf_ui->startIndex_sb->value();
    int end_idx;
    if(cf_ui->isEndIndex_cb->isChecked())
    {
        end_idx = cf_ui->endIndex_sb->value();
        if(end_idx < start_idx)
        {
            int t = start_idx;
            start_idx = end_idx;
            end_idx = t;
        }
    }
    else
        end_idx = start_idx;
    end_idx += 1; // Для цикла

    QColor start_color = cf_ui->startColor_cp->getColor();
    QColor end_color;
    if(cf_ui->isEndColor_cb->isChecked())
        end_color = cf_ui->endColor_cp->getColor();
    else
        end_color = start_color;

    // Для интерполяции цвета
    double step_r = (double)(end_color.red() - start_color.red())
                    / (end_idx-start_idx);
    double step_g = (double)(end_color.green() - start_color.green())
                    / (end_idx-start_idx);
    double step_b = (double)(end_color.blue() - start_color.blue())
                    / (end_idx-start_idx);

    for(int a=start_idx; a<end_idx; a++)
    {
        QColor c;
        c.setRed(start_color.red() + step_r*(a-start_idx) );
        c.setGreen(start_color.green() + step_g*(a-start_idx) );
        c.setBlue(start_color.blue() + step_b*(a-start_idx) );

        // Интерполируем и пишем по адресу
        setColorByIndex(a, c);
    }
    repaint();
}
//------------------------------------------------------------------------------
void PaletteViewer::onOkPressed()
{
    onApplyPressed();
    editorWindow->hide();
}
//------------------------------------------------------------------------------
void PaletteViewer::onIndexCbClicked()
{
    cf_ui->endIndex_sb->setEnabled(cf_ui->isEndIndex_cb->isChecked());
}
//------------------------------------------------------------------------------
void PaletteViewer::onColorCbClicked()
{
    cf_ui->endColor_cp->setVisible(cf_ui->isEndColor_cb->isChecked());
}
//------------------------------------------------------------------------------
void PaletteViewer::showInfoWindow(QPoint p)
{
    int x = p.x() / PXSIZE;
    int y = p.y() / PXSIZE;
    int px_w = width() / PXSIZE;
    int idx = y*px_w + x;
    if((idx < 0) || (idx >= palette.header.numColors))
        return;
    QColor c = getColorByIndex(idx);

    ci_ui->id_label->setText(QString::number(idx));
    ci_ui->color_label->setStyleSheet(
        "background-color: "
        + c.name()
        );
    ci_ui->color_label->setText("   ");
    ci_ui->rgb_label->setText(  QString::number(c.red()) + " "
                              + QString::number(c.green()) + " "
                              + QString::number(c.blue()) );
    infoForm->show();
    infoForm->move(mapToGlobal(p) + QPoint(10, 10));

    this->activateWindow();
}
