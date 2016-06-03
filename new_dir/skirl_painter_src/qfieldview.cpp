#define _USE_MATH_DEFINES // Студия блин!
#include <math.h>
#include <math.h>
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QtDebug>
//#include <qwt_math.h>

#include <stdio.h>

#include "qfieldview.h"

//------------------------------------------------------------------------------
QFieldView_PaintThread::QFieldView_PaintThread()
{
    elapsedTime = 0;
    memset(&t_scale, 0, sizeof(t_scale));

}
//------------------------------------------------------------------------------
QFieldView_PaintThread::~QFieldView_PaintThread()
{
}
//------------------------------------------------------------------------------
void QFieldView_PaintThread::run()
{
    QTime T1;
    QTime T2;

    T1 = QTime::currentTime();
    //if(t_scale == 0) return;
    int W = t_bound.width();
    int H = t_bound.height();

    // Кусок с пересозданием памяти под изображение нельзя прерывать, чтобы не
    // получить указатель без данных (delete не устанавливает указатель в 0).
    // Остальной код не опасно потерять при перезапуске потока.
    setTerminationEnabled(false); // Отключаем возможность терминации
    if( (t_img.width() != W) || (t_img.height() != H) )
        t_img = QImage(W, H, QImage::Format_RGB32);
    setTerminationEnabled(true);  // Включаем обратно.


    qDebug() << "Rebuilding image with threshold" << t_threshold;
    // Пошла генерация самого изображения
    if(t_bTimeMode)
    {
        makeSquareImage(W, H);
    }
    else
    {
        if(t_bIko)
            makeIKO(W, H);
        else
            makeSquareImage(W, H);
    }


    T2 = QTime::currentTime();
    elapsedTime = T1.msecsTo(T2);
}
//------------------------------------------------------------------------------
void QFieldView_PaintThread::makeSquareImage(int W, int H)
{
    //  unsigned int whiteColor = 0xFFFFFFFF;
    unsigned int offset = 0;
    unsigned char* RawData = (unsigned char*)t_img.constBits();

    //   if(frameset->isEmpty()) return; // проверка на пустые данные

    // Расчет шага при масштабировании
    // Шаг по сетке отсчетов - число отсчетов на 1 пиксель изображения.
    float rstep = 1.0*(t_scale.far - t_scale.close)/W;
    // Базовый отсчет
    int rbase = t_scale.close;
    float tstep = -1.0*(t_scale.newer - t_scale.older)/H;
    int tbase = t_scale.newer; //отрицательные, не забываем
    int lastframe = t_frameset->size() - 1; // Последний кадр в пачке

    if(t_amp_limit <= 0) t_amp_limit = 1; // были прецеденты

    // Вспомогательные переменные, общие для расчетов.
    float amp_out;
    SkirlQuad sq;
    SkirlFrame fr;
    RgbQuad q;

    // Далее следуют похожие куски кода, чтобы минимизировать число if-ов под
    // циклом.

    if(t_fdm == FDM_QUICK) // Быстрая отрисовка
    {
        for(int y=0; y<H; y++) // Цикл по Y
        {
            int t0 = tbase + floor(tstep*(y));
            int framenum = lastframe+t0;
            if((framenum < 0) || (framenum > lastframe))
            {
                // Вся строка пустая, заполняем нулями.
                for(int x=0; x<W; x++)
                {
                    q.rgbBlue = 0;
                    q.rgbGreen = 0;
                    q.rgbRed = 0;
                    q.rgbAlpha = 255;
                    memcpy(RawData+offset, &q, sizeof(q));
                    offset += sizeof(RgbQuad);
                }
                continue;
            }

            fr = t_frameset->at(framenum);
            for(int x=0; x<W; x++) // Цикл по X
            {
                int r0 = rbase + floor(rstep*(x));

                if( (r0>=0) && (r0 < fr.size) )
                {
                    sq = fr.quads[r0];
                    amp_out = sqrtf(sq.re*sq.re + sq.im*sq.im);
                    //         amp_max = sq.re;
                }
                else
                    amp_out = 0;

                q = getColorByAmp(amp_out);
                memcpy(RawData+offset, &q, sizeof(q));

                offset += sizeof(q);
            } // Цикл по X
        } // Цикл по Y
    } // Быстрая отрисовка


    if(t_fdm == FDM_GETMAX) // Поиск максимума
    {
        for(int y=0; y<H; y++) // Цикл по Y
        {
            int t0 = tbase + floor(tstep*(y+1));
            int t1 = tbase + floor(tstep*(y));

            for(int x=0; x<W; x++) // Цикл по X
            {
                int r0 = rbase + floor(rstep*(x));
                int r1 = rbase + floor(rstep*(x+1));

                amp_out = 0; // Ищем максимум из того, что "накрыли".
                for(int a=t0; a<=t1; a++) // Цикл по a - Y, время/угол
                {
                    int framenum = lastframe+a;
                    if( ( framenum<0 ) || (framenum > lastframe) )
                        continue;

                    fr = t_frameset->at(framenum);
                    int framesize = fr.size;

                    for(int b=r0; b<=r1; b++) // Цикл по b - X, дальность
                    {

                        if( b >= framesize )
                            break;

                        sq = fr.quads[b];
                        float amp = sqrtf(sq.re*sq.re + sq.im*sq.im);

                        if(amp>amp_out)amp_out = amp;

                    } // Цикл по b - X, дальность
                } // Цикл по a - Y, время/угол

                q = getColorByAmp(amp_out);
                memcpy(RawData+offset, &q, sizeof(q));

                offset += sizeof(q);
            } // Цикл по X
        } // Цикл по Y
    } // Поиск максимума

    if(t_fdm == FDM_AVERAGE) // Усреднение
    {
        for(int y=0; y<H; y++) // Цикл по Y
        {
            int t0 = tbase + floor(tstep*(y+1));
            int t1 = tbase + floor(tstep*(y));

            for(int x=0; x<W; x++) // Цикл по X
            {
                int r0 = rbase + floor(rstep*(x));
                int r1 = rbase + floor(rstep*(x+1));

                amp_out = 0; // Ищем максимум из того, что "накрыли".
                int num_avg = 0;
                for(int a=t0; a<=t1; a++) // Цикл по a - Y, время/угол
                {
                    int framenum = lastframe+a;
                    if( ( framenum<0 ) || (framenum > lastframe) )
                        continue;

                    fr = t_frameset->at(framenum);
                    int framesize = fr.size;

                    for(int b=r0; b<=r1; b++) // Цикл по b - X, дальность
                    {

                        if( b >= framesize )
                            break;

                        sq = fr.quads[b];
                        float amp = sqrtf(sq.re*sq.re + sq.im*sq.im);

                        amp_out += amp;
                        num_avg += 1;

                    } // Цикл по b - X, дальность
                } // Цикл по a - Y, время/угол

                if(num_avg > 0)
                    amp_out /= num_avg;

                q = getColorByAmp(amp_out);
                memcpy(RawData+offset, &q, sizeof(q));

                offset += sizeof(q);
            } // Цикл по X
        } // Цикл по Y
    } // Усреднение

}
//------------------------------------------------------------------------------
void QFieldView_PaintThread::makeIKO(int W, int H)
{
    unsigned int offset = 0;
    unsigned char* RawData = (unsigned char*)t_img.constBits();

    // Найдем "масштаб", сколько метров реального расстояния приходится на 1 пикс.
    float scale;
    if(W>H) // Масштаб ориентируется от высоты
        scale = t_ikoScale.size / H * 2;
    else // Масштаб ориентируется от ширины
        scale = t_ikoScale.size / W * 2;

    // Пригодится при расчетах - размер пикселя при объединении отсчетов.
    float pixsize = 1.41*scale;

    if(t_amp_limit <= 0) t_amp_limit = 1; // были прецеденты

    // Найдем минимальное расстояние, от которого стоит рисовать
    float minRange=1;
    if(t_ikoMetrics.minRange_LFM < t_ikoMetrics.minRange_SKI)
        minRange = t_ikoMetrics.minRange_LFM;
    else
        minRange = t_ikoMetrics.minRange_SKI;
    if(minRange < 1) minRange = 1; // Не ноль!

    // Много раз пригодится!
    int Amax = t_ikoMetrics.A_max;

    // Вспомогательные переменные, общие для расчетов.
    float amp_out;
    SkirlQuad sq;
    SkirlFrame fr;
    RgbQuad q;

    if(t_fdm == FDM_QUICK) // Быстрая отрисовка
    {
        // Заполнение изображения
        for(int y=0; y<H; y++)
            for(int x=0; x<W; x++)
            {
                float real_x = scale * (x-W/2) + t_ikoScale.x_mid;
                float real_y = scale * (H/2-y) + t_ikoScale.y_mid;
                float range = sqrtf(real_x*real_x + real_y * real_y);

                amp_out = 0;
                // Попадание в пределы расстояний
                if((range < t_maxRange) && (range>minRange))
                {
                    // Азимутальный угол в радианах
                    float angle = acosf(real_y / range);

                    if(real_x < 0)
                        angle *= -1;

                    if(t_ikoMetrics.A_inverse)
                        angle = 2.0f*M_PI-angle;

                    // Добавим поправку на уголовое смещение, переведя ее в рад.
                    angle -= M_PI/180.0*t_ikoMetrics.A_offset;

                    // Затолкаем угол в диапазон [0, 2*M_PI)
                    while(angle <  0     ) angle += M_PI*2;
                    while(angle >= M_PI*2) angle -= M_PI*2;

                    int angle_mark = floor(angle / (M_PI*2) * Amax);

                    // Особая указательная магия!
                    int* ind = t_indices + angle_mark*NUM_LAYERS;
                    int framenum = ind[0];

                    if(framenum != -1)  // Есть такая строка
                    {
                        fr = t_frameset->at(framenum);
                        int beamMode = fr.flags & 0xF;
                        if      (beamMode == SKI_FLAG)
                            range -= t_ikoMetrics.minRange_SKI;
                        else if (beamMode == LFM_FLAG)
                            range -= t_ikoMetrics.minRange_LFM;
                        else
                        {
                            // неизвестные тип сигнала - проигнорировать эти данные?
                        }
                        /*
          if((beamMode == SKI_TIME) || (beamMode == SKI_ANGLE))
            range -= t_ikoMetrics.minRange_SKI;
          if((beamMode == LFM_TIME) || (beamMode == LFM_ANGLE))
            range -= t_ikoMetrics.minRange_LFM;
          */
                        int r_idx = round(range / t_ikoMetrics.rangeStep);
                        if((r_idx < fr.size) && (r_idx > 0))
                        {
                            sq = fr.quads[r_idx];
                            amp_out = sqrtf(sq.re*sq.re + sq.im*sq.im);
                        }
                    } // Есть такая строка

                } // Попадание в пределы расстояний

                q = getColorByAmp(amp_out);
                memcpy(RawData+offset, &q, sizeof(q));
                offset += sizeof(q);
            } // Заполнение изображения
    } // Быстрая отрисовка.


    // FDM_GETMAX и FDM_AVERAGE почти одинаковы, но разнесены в разные
    // дублирующиеся ветви для скорострельности (if - долго)
    if(t_fdm == FDM_GETMAX) // Поиск максимума
    {
        // Заполнение изображения
        for(int y=0; y<H; y++)
            for(int x=0; x<W; x++)
            {
                float real_x = scale * (x-W/2) + t_ikoScale.x_mid;
                float real_y = scale * (H/2-y) + t_ikoScale.y_mid;
                float range = sqrtf(real_x*real_x + real_y * real_y);

                amp_out = 0;
                // Попадание в пределы расстояний
                if((range < t_maxRange) && (range>minRange))
                {
                    // Азимутальный угол в радианах
                    float angle = acosf(real_y / range);

                    if(real_x < 0)
                        angle *= -1;

                    if(t_ikoMetrics.A_inverse)
                        angle = 2.0f*M_PI-angle;

                    // Добавим поправку на уголовое смещение, переведя ее в радианы
                    angle -= M_PI/180.0*t_ikoMetrics.A_offset;

                    // Затолкаем угол в диапазон [0, 2*M_PI)
                    while(angle <  0     ) angle += M_PI*2;
                    while(angle >= M_PI*2) angle -= M_PI*2;

                    int angle_mark = floor(angle / (M_PI*2) * Amax);
                    int angle_size = floor(pixsize/(4.0*M_PI*range)*t_ikoMetrics.A_max);
                    int angle_min = angle_mark - angle_size;
                    int angle_max = angle_mark + angle_size;
                    // Хотя бы один угловой отсчет должен поместиться!
                    if(angle_min == angle_max) angle_max += 1;

                    for(int a=angle_min; a<angle_max; a++) // Цикл по a - углы
                    {
                        int aa = a; // Вкинем a в пределы углов
                        while(aa <  0   ) aa += Amax;
                        while(aa >= Amax) aa -= Amax;

                        int* ind = t_indices + angle_mark*NUM_LAYERS;

                        for(int b=0; NUM_LAYERS; b++) // Цикл по b - строки данных
                        {
                            int framenum = ind[b];
                            if(framenum == -1)
                                break;
                            fr = t_frameset->at(framenum);
                            float fr_range; // расстояние в кадре, это не range пикселя!
                            int beamMode = fr.flags & 0xF;
                            if      (beamMode == SKI_FLAG)
                                fr_range = range - t_ikoMetrics.minRange_SKI;
                            else if (beamMode == LFM_FLAG)
                                fr_range = range - t_ikoMetrics.minRange_LFM;
                            else
                            {
                                // неизвестные тип сигнала - проигнорировать эти данные?
                            }
                            /*
          if((beamMode == SKI_TIME) || (beamMode == SKI_ANGLE))
            fr_range = range - t_ikoMetrics.minRange_SKI;
          if((beamMode == LFM_TIME) || (beamMode == LFM_ANGLE))
            fr_range = range - t_ikoMetrics.minRange_LFM;
          */
                            // Пределы обхода по дальности (отсч.)
                            int r_low  = round( (fr_range-pixsize) / t_ikoMetrics.rangeStep);
                            int r_high = round( (fr_range+pixsize) / t_ikoMetrics.rangeStep);

                            // Впишем в размер строки
                            int framesize = fr.size;
                            if(r_low < 0) r_low = 0;
                            if(r_high < 0) r_high = 0;
                            if(r_low == r_high) // Минимум одна дальность должна влезть!
                                r_high += 1;
                            if(r_low > framesize) r_low = framesize;
                            if(r_high > framesize) r_high = framesize;

                            for(int c=r_low; c<r_high; c++) // Цикл по c - отсчеты дальности
                            {
                                sq = fr.quads[c];
                                float amp = sqrtf(sq.re*sq.re + sq.im*sq.im);
                                if(amp > amp_out) amp_out = amp;

                            } // Цикл по c - отсчеты дальности
                        } // Цикл по b - строки данных

                    } // Цикл по a - углы
                } // Попадание в пределы расстояний

                q = getColorByAmp(amp_out);
                memcpy(RawData+offset, &q, sizeof(q));
                offset += sizeof(q);
            } // Заполнение изображения
    } // Поиск максимума.


    // FDM_GETMAX и FDM_AVERAGE почти одинаковы, но разнесены в разные
    // дублирующиеся ветви для скорострельности (if - долго)
    if(t_fdm == FDM_AVERAGE) // Усреднение
    {
        // Заполнение изображения
        for(int y=0; y<H; y++)
            for(int x=0; x<W; x++)
            {
                float real_x = scale * (x-W/2) + t_ikoScale.x_mid;
                float real_y = scale * (H/2-y) + t_ikoScale.y_mid;
                float range = sqrtf(real_x*real_x + real_y * real_y);

                amp_out = 0;
                int num_avg = 0;
                // Попадание в пределы расстояний
                if((range < t_maxRange) && (range>minRange))
                {
                    // Азимутальный угол в радианах
                    float angle = acosf(real_y / range);

                    if(real_x < 0)
                        angle *= -1;

                    if(t_ikoMetrics.A_inverse)
                        angle = 2.0f*M_PI-angle;

                    // Добавим поправку на уголовое смещение, переведя ее в радианы
                    angle -= M_PI/180.0*t_ikoMetrics.A_offset;

                    // Затолкаем угол в диапазон [0, 2*M_PI)
                    while(angle <  0     ) angle += M_PI*2;
                    while(angle >= M_PI*2) angle -= M_PI*2;

                    int angle_mark = floor(angle / (M_PI*2) * Amax);
                    int angle_size = floor(pixsize/(4.0*M_PI*range)*t_ikoMetrics.A_max);
                    int angle_min = angle_mark - angle_size;
                    int angle_max = angle_mark + angle_size;
                    // Хотя бы один угловой отсчет должен поместиться!
                    if(angle_min == angle_max) angle_max += 1;

                    for(int a=angle_min; a<angle_max; a++) // Цикл по a - углы
                    {
                        int aa = a; // Вкинем a в пределы углов
                        while(aa <  0   ) aa += Amax;
                        while(aa >= Amax) aa -= Amax;

                        int* ind = t_indices + angle_mark*NUM_LAYERS;

                        for(int b=0; b<NUM_LAYERS; b++) // Цикл по b - строки данных
                        {
                            int framenum = ind[b];
                            if(framenum == -1)
                                break;
                            fr = t_frameset->at(framenum);
                            float fr_range; // расстояние в кадре, это не range пикселя!
                            int beamMode = fr.flags & 0xF;
                            if      (beamMode == SKI_FLAG)
                                fr_range = range - t_ikoMetrics.minRange_SKI;
                            else if (beamMode == LFM_FLAG)
                                fr_range = range - t_ikoMetrics.minRange_LFM;
                            else
                            {
                                // неизвестные тип сигнала - проигнорировать эти данные?
                            }
                            /*
          if((beamMode == SKI_TIME) || (beamMode == SKI_ANGLE))
            fr_range = range - t_ikoMetrics.minRange_SKI;
          if((beamMode == LFM_TIME) || (beamMode == LFM_ANGLE))
            fr_range = range - t_ikoMetrics.minRange_LFM;
          */
                            // Пределы обхода по дальности (отсч.)
                            int r_low  = round( (fr_range-pixsize) / t_ikoMetrics.rangeStep);
                            int r_high = round( (fr_range+pixsize) / t_ikoMetrics.rangeStep);

                            // Впишем в размер строки
                            int framesize = fr.size;
                            if(r_low < 0) r_low = 0;
                            if(r_high < 0) r_high = 0;
                            if(r_low == r_high) // Минимум одна дальность должна влезть!
                                r_high += 1;
                            if(r_low > framesize) r_low = framesize;
                            if(r_high > framesize) r_high = framesize;

                            for(int c=r_low; c<r_high; c++) // Цикл по c - отсчеты дальности
                            {
                                sq = fr.quads[c];
                                float amp = sqrtf(sq.re*sq.re + sq.im*sq.im);
                                amp_out += amp;
                                num_avg += 1;

                            } // Цикл по c - отсчеты дальности
                        } // Цикл по b - строки данных

                    } // Цикл по a - углы
                } // Попадание в пределы расстояний

                if(num_avg > 0)
                    amp_out /= num_avg;

                q = getColorByAmp(amp_out);
                memcpy(RawData+offset, &q, sizeof(q));
                offset += sizeof(q);
            } // Заполнение изображения
    } // Усреднение.

}
//------------------------------------------------------------------------------
RgbQuad QFieldView_PaintThread::getColorByAmp(float amp)
{
    RgbQuad q;
    int idx = floor(amp*t_palette->header.numColors/t_amp_limit);
    if(idx > t_palette->header.numColors - 1)
        idx = t_palette->header.numColors - 1;
    if(amp<t_threshold)
        idx = 0;
//    switch(t_palette->header.version)
//    {
//    case PV_4RGBA:
//    case PV_4RGBF:
//    case PV_4RGB0:
//        // 4 байта ровно!
        memcpy(&q, t_palette->data + idx*4, 4);
        q.rgbAlpha = 255;
//        break;
//    }
    return q;
}
//------------------------------------------------------------------------------
void QFieldView_PaintThread::fillImage(QImage& img)
{
    img = t_img.copy();
}
//------------------------------------------------------------------------------
void QFieldView_PaintThread::setDrawData(DrawDataPack ddp)
{
    t_bIko = *(ddp.bIko);
    t_bTimeMode = *(ddp.bTimeMode);
    t_fdm = *(ddp.fdm);
    t_amp_limit = *(ddp.amp_limit);
    t_maxframesize = *(ddp.maxFrameSize);
    t_maxRange = *(ddp.maxRange);
    t_frameset = ddp.frameset;
    t_fullViewIndex = *(ddp.fullViewIndex);
    t_indices = ddp.indices;
    t_threshold = ddp.threshold;
    if(ddp.scale != 0)
        memcpy(&t_scale, ddp.scale, sizeof(t_scale));
    if(ddp.ikoScale != 0)
        memcpy(&t_ikoScale, ddp.ikoScale, sizeof(t_ikoScale));
    if(ddp.ikoMetrics != 0)
        memcpy(&t_ikoMetrics, ddp.ikoMetrics, sizeof(t_ikoMetrics));
    if(ddp.palette != 0)
    {
        t_palette = ddp.palette;
    }
}
//------------------------------------------------------------------------------
//****************************************************************************//
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
QFieldView::QFieldView(QWidget *parent) :
    QWidget(parent)
{
    bDebugMode = false;
    border = BORDER;
    minimap = MINIMAP;
    piesize = PIESIZE;
    tool = VT_TRACE;
    setFocusPolicy(Qt::ClickFocus);
    bIko = false;
    bTimeMode = true;
    bCheckResize = true;
    maxframesize = 0;
    maxRange = 0;
    dragState = DS_NONE;
    amp_limit = AMP_LIMIT;
    fdm = FDM_QUICK;
    bRedrawFlag = false;
    scaleStack.clear();
    ikoScaleStack.clear();
    scale = 0;
    ikoScale = 0;
    ikoMetrics = 0;
    frameset = 0;
    fullViewIndex = 0;
    indices = 0;
    frameLimits = 0;
    fieldImage.loadFromData(0);
    memset(&imageScale, 0, sizeof(imageScale));
    dragStart = QPoint(0,0);
    dragEnd = QPoint(0,0);
    gridColor = QColor( (QRgb) GRID_COLOR );
    gridColor.setAlpha(GRID_ALPHA);
    threshold = 0;

    memset(&palette, 0, sizeof(palette));
    setDefaultPalette();

    pack.bIko = &bIko;
    pack.bTimeMode = &bTimeMode;
    pack.amp_limit = &amp_limit;
    pack.fdm = &fdm;
    pack.maxFrameSize = &maxframesize;
    pack.fullViewIndex = &fullViewIndex;
    pack.maxRange = &maxRange;
    pack.scale = 0;
    pack.ikoScale = 0;
    pack.ikoMetrics = 0;
    pack.frameset = 0;
    pack.palette = &palette;
    pack.threshold = threshold;
    pt.setDrawData(pack); // В принципе достаточно одного раза - поток работает
    // по указателям

    // Слот обработки потока
    connect(&pt, SIGNAL(finished()), SLOT(onThreadFinished()));

    r_channel = 0;
    t_channel = 0;
    traceAngle = 0;
    traceRange = 0;
}
//------------------------------------------------------------------------------
QFieldView::~QFieldView()
{
    while( !pt.wait(1000) ) // Ждем пока поток закончится.
    {
    }
    delete [] indices;
    delete [] palette.data;
}
//------------------------------------------------------------------------------
void QFieldView::setDefaultPalette()
{
    // Создание "дефолтной" палитры цветов
    palette.header.ID[0] = 'P';
    palette.header.ID[1] = 'F';
    palette.header.numColors = 256;
    palette.header.version = PV_4RGB0;
    delete [] palette.data;
    palette.data = new uint8_t[4*256];
    memset(palette.data, 0, 4*256);
    for(int a=0; a<256; a++)
    {
        palette.data[4*a+1] = a;
    }
}
//------------------------------------------------------------------------------
void QFieldView::paintEvent(QPaintEvent *e)
{
    // Всякие разные цвета
    QColor border_color(255,255,255); // Цвет полей по краям
    QColor mm_color(96,96,96);        // Цвет мини-карты
    QColor mmf_color(192,0,0);        // Цвет рамки мини-карты
    QColor mmg_color(0, 0, 0);        // Цвет сетки мини-карты
    QColor trace_color(255,255,255);  // Цвет линий трассировки
    QColor zoom_color(0, 128, 255);   // Цвет окна зума
    QColor zoom_fill_color(0,128,255,64); // Цвет заполнения окна зума.
    // Кисти для рисования полей
    QPen   border_pen(border_color);
    QBrush border_brush(border_color, Qt::SolidPattern);
    // Кисти для рисования миникарты
    QPen   mm_pen(Qt::NoPen);
    QBrush mm_brush(mm_color, Qt::SolidPattern);
    // Кисти для рисования рамки
    QPen   mmf_pen(mmf_color);
    QBrush mmf_brush(Qt::NoBrush);
    // Кисти для рисования сетки мини-карты
    QPen   mmg_pen(mmg_color);
    QBrush mmg_brush(Qt::NoBrush);
    // Кисть для рисования трассировки
    QPen trace_pen(trace_color);
    // Кисть для рисования зума
    QPen zoom_pen(zoom_color);
    QBrush zoom_brush(zoom_fill_color, Qt::SolidPattern);
    // Кисть для рисования сетки ИКО
    QPen grid_pen(gridColor);

    // Инициализацияы
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setRenderHint(QPainter::HighQualityAntialiasing, false);
    p.setRenderHint(QPainter::SmoothPixmapTransform, false);
    QRect bound = p.viewport();
    int W = bound.width();
    int H = bound.height();
    QTime T1 = QTime::currentTime();
    QTime T2;


    QSize fieldSize;
    fieldSize.setWidth(width()-2*border-minimap);
    fieldSize.setHeight(height()-2*border);

    // Отправим изображение на повторную перерисовку, если оно не подходит по
    // размеру (изменили размер виджета)
    if(fieldSize != fieldImage.size())
        invalidateImage();

    if(bTimeMode) // Отрисовка прямоугольника по времени
    {
        // Аналогично отправим если масштаб изображения не соответствует текущему.
        if( (imageScale.close != scale->close ) ||
                (imageScale.far   != scale->far   ) ||
                (imageScale.newer != scale->newer ) ||
                (imageScale.older != scale->older )
                )
            invalidateImage();

        QRect imageRect = getImageBound(imageScale);
        /*    imageRect.setBottomLeft(
                getPixelByDataPoint( imageScale.r_base - imageScale.r_scale,
                                     imageScale.t_base - imageScale.t_scale)
                        );
    imageRect.setTopRight(
                getPixelByDataPoint( imageScale.r_base + imageScale.r_scale - 1,
                                     imageScale.t_base + imageScale.t_scale - 1)
                         );*/

        p.setClipRect( border,                   border,
                       width()-2*border-minimap, height() - 2*border);
        p.drawImage(border, border, fieldImage);
        p.setClipping(false);
//        p.drawImage(imageRect,
//                    fieldImage
//                    ); // Выплевываем изображение на форму, домасштабируем если надо.

        // Отрисовка полей
        p.setPen(border_pen);
        p.setBrush(border_brush);
        p.drawRect(0, 0, W, border-1); // Верхнее поле
        p.drawRect(0, border, border, H-2*border); // левое поле
        p.drawRect(W-border, border, border, H-2*border); //правое поле
        p.drawRect(0, H-border, W, border); // Нижнее поле

        // Отрисовка миникарты
        p.setPen(mm_pen);
        p.setBrush(mm_brush);
        int map_h = H-2*border; // пригодится
        p.drawRect(W-border-minimap, border, minimap, map_h);

        // Отрисовка рамки миникарты
        if(frameset->size()>0)
        {
            p.setPen(mmf_pen);
            p.setBrush(mmf_brush);
            float r_scale = 1.0 * minimap/maxframesize;
            float t_scale = 1.0 * map_h/frameset->size();
            int frame_x = W-border-minimap + floor( r_scale*(scale->close));
            int frame_y = border - floor( t_scale*(scale->newer-1));
            int frame_w = floor( r_scale*(scale->far - scale->close) ) - 1;
            int frame_h = floor( t_scale*(scale->newer - scale->older) ) - 1;
            // -1 стоит из-за особенностей рисования QT, см. мануал.
            p.drawRect(frame_x, frame_y, frame_w, frame_h);

            if(tool == VT_TRACE)
            {
                // Отрисовка трассирующих линий
                p.setPen(trace_pen);
                QPoint trace_pt = getPixelByDataPoint(r_channel, t_channel);
                int trace_x = trace_pt.x();
                int trace_y = trace_pt.y();
                // Горизонтальная линия
                p.drawLine(border, trace_y, W-border-minimap, trace_y);
                // Вертикальная линия
                p.drawLine(trace_x, border, trace_x, H-border);
            }

            // Отрисовка окна зума
            if( (tool == VT_ZOOM) && (dragState == DS_ZOOM_FRAME) )
            {
                p.setPen(zoom_pen);
                p.setBrush(zoom_brush);
                p.drawRect(QRect(dragStart, dragEnd));
                p.setBrush(Qt::NoBrush);
            }
        }

        T2 = QTime::currentTime();
    } // Отрисовка прямоугольника по времени
    else
    {
        if(bIko) // Отрисовка ИКО
        {
            if( (imageIkoScale.size  != ikoScale->size)  ||
                    (imageIkoScale.x_mid != ikoScale->x_mid) ||
                    (imageIkoScale.y_mid != ikoScale->y_mid)
                    )
                invalidateImage();

            QRect imageRect = getImageBound(imageIkoScale);
            p.setClipRect( border,                   border,
                           width()-2*border-minimap, height() - 2*border);
            p.drawImage(border, border, fieldImage);
            p.setClipping(false);

//            p.drawImage(imageRect,
//                        fieldImage
//                        ); // Выплевываем изображение на форму, домасштабируем если надо.

            // Отрисовка полей
            p.setPen(border_pen);
            p.setBrush(border_brush);
            p.drawRect(0, 0, W, border-1); // Верхнее поле
            p.drawRect(0, border, border, H-2*border); // левое поле
            p.drawRect(W-border, border, border, H-2*border); //правое поле
            p.drawRect(0, H-border, W, border); // Нижнее поле

            // Отрисовка миникарты
            p.setPen(mm_pen);
            p.setBrush(mm_brush);
            int map_h = H-2*border; // пригодится
            p.drawRect(W-border-minimap, border, minimap, map_h);

            // Отрисовка маленькой мини-карты, отображающей резиновую рамку
            p.setPen(mmg_pen);
            p.setBrush(mmg_brush);
            int mmg_x, mmg_y, mmg_w, mmg_h;
            mmg_x = W-border-minimap;
            mmg_y = H-border-minimap;
            mmg_w = minimap;
            mmg_h = minimap;
            // Вертикальная линия
            p.drawLine(mmg_x + mmg_w/2, mmg_y, mmg_x + mmg_w/2, mmg_y + mmg_h-1);
            // Горизонтальная линия
            p.drawLine(mmg_x, mmg_y + mmg_h/2, mmg_x + mmg_w-1, mmg_y + mmg_h/2);
            // Отрисуем пачку кругов
            for(int a=0; a<3; a++)
            {
                p.drawEllipse(mmg_x, mmg_y, mmg_w-1, mmg_h-1);
                mmg_x += minimap/6;
                mmg_y += minimap/6;
                mmg_w -= minimap/3;
                mmg_h -= minimap/3;
            }

            float sa=0, ea=0; // Начальный и конечный угол текущего кадра.

            // Отрисовка "пирогов"
            if(frameLimits != 0)
            {
                for(int a=0; a<NUM_CIRCLES; a++)
                {
                    if(a == fullViewIndex)
                    {
                        p.setPen(mmf_pen);
                        p.setBrush(mmf_brush);
                    }
                    else
                    {
                        p.setPen(mmg_pen);
                        p.setBrush(mmg_brush);
                    }
                    int pie_y = height() - border - minimap - (border+piesize)*(a+1);
                    if(pie_y < border) break;

                    if(frameLimits[2*a] != -1)
                    {
                        float a_start = frameset->at(frameLimits[2*a + 1]).angle;
                        float a_end = frameset->at(frameLimits[2*a]).angle;
                        // Не забываем, что углы лежат "наоборот".
                        float start_angle =    90
                                - a_start * 360 / ikoMetrics->A_max
                                - ikoMetrics->A_offset;
                        float end_angle =    90
                                - a_end * 360 / ikoMetrics->A_max
                                - ikoMetrics->A_offset;

                        if(bInverseFrames)
                        {
                            // Кадры идут на уменьшение угла, end_angle > start_angle
                            if(end_angle < start_angle)
                                end_angle += 360;
                        }
                        else
                        {
                            // Кадры идут на увеличение угла, end_angle < start_angle
                            if(start_angle < end_angle)
                                start_angle += 360;
                        }
                        p.drawPie(width() - border - minimap,
                                  pie_y,
                                  minimap-1,
                                  piesize-1,
                                  floor(16*start_angle),
                                  floor(16*(end_angle - start_angle)));

                        if(a == fullViewIndex)
                        {
                            sa = start_angle;
                            ea = end_angle;
                        }
                    }
                }
            }

            // Отрисовка рамки миникарты
            if((ikoScale != 0) && (maxRange != 0))
            {
                p.setPen(mmf_pen);
                p.setBrush(mmf_brush);
                QPointF topleft = getCoordsByPixel(border, border);
                QPointF bottomright = getCoordsByPixel(W-border-minimap, H-border);
                int x1 = round( (topleft.x() + maxRange) / (2*maxRange) * minimap)
                        + W - border - minimap;
                if(x1 < W-border-minimap) x1 = W-border-minimap;
                int x2 = round( (bottomright.x() + maxRange) / (2*maxRange) * minimap)
                        + W - border - minimap - 1;
                if(x2 > W-border-1) x2 = W-border-1;
                int y1 = H-border
                        - round( (topleft.y() + maxRange) / (2*maxRange) * minimap );
                if(y1 < H-border-minimap) y1 = H-border-minimap;
                int y2 = H-border - 1
                        - round( (bottomright.y() + maxRange) / (2*maxRange) * minimap );
                if(y2 >H -border-1) y2 = H-border-1;
                p.drawRect(x1, y1, x2-x1, y2-y1);

            }

            p.setClipRect( border,                   border,
                          width()-2*border-minimap, height() - 2*border);

            // Отрисовка сетки ИКО
            if(gridColor.alpha() > 0)
            {
                p.setPen(grid_pen);
                // Отрисовка радиальных линий
                if((gridStep_a > 0) && (360.0 / gridStep_a < 10000)) // Простые проверки
                {
                    for(double a=0; a<360; a+=gridStep_a)
                    {
                        double angle =   (double)ikoMetrics->A_max * a / 360
                                - (double)ikoMetrics->A_offset/360*ikoMetrics->A_max;
                        QPointF pt2 = getCoordsByGeo(maxRange, angle);
                        QPoint px1 = getPixelByCoords(0, 0);
                        QPoint px2 = getPixelByCoords(pt2);
                        p.drawLine(px1, px2);
                    }
                }

                // Отрисовка кругов
                // Простые проверки
                if((gridStep_r > 0) && (maxRange / gridStep_r < 10000))
                {
                    for(double a=gridStep_r; a<maxRange; a+=gridStep_r)
                    {
                        QPoint px1 = getPixelByCoords(0, 0);
                        QPoint px2 = getPixelByCoords(a, 0);
                        int rx = px2.x() - px1.x();
                        p.drawEllipse(px1, rx, rx);
                    }
                }
            }// Отрисовка сетки ИКО

            if(tool == VT_TRACE)
            {
                // Отрисовка трассирующих линий
                p.setPen(trace_pen);
                p.setBrush(Qt::NoBrush);
                QPoint pt_center = getPixelByCoords(0, 0);
                // +0.5, чтобы при большом зуме попадать в центр квадрата, а не абы куда
                QPoint pt_end = getPixelByCoords(
                            getCoordsByGeo(maxRange, traceAngle+0.5)
                            );
                QPoint pt_w = getPixelByCoords(traceRange, 0);
                int arc_w = pt_w.x() - pt_center.x();
                // Линия A=const
                p.drawLine(pt_center, pt_end);
                // Дуга R=const
                p.drawArc( pt_center.x() - arc_w,
                          pt_center.y() - arc_w,
                          2*arc_w - 1,
                          2*arc_w - 1,
                          round(16*sa),
                          round(16*(ea-sa))
                          );
            }

            // Отрисовка окна зума
            if( (tool == VT_ZOOM) && (dragState == DS_ZOOM_FRAME) )
            {
                p.setPen(zoom_pen);
                p.setBrush(zoom_brush);
                p.drawRect(QRect(dragStart, dragEnd));
                p.setBrush(Qt::NoBrush);
            }

            p.setClipRect(0, 0, width(), height());

            T2 = QTime::currentTime();
        } // Отрисовка ИКО
        else // Отрисовка прямоугольника по углу
        {
            //makeSquareImage(bound);
            QRect imageRect = getImageBound(imageScale);
            p.drawImage(imageRect, fieldImage);
            T2 = QTime::currentTime();
        }
    }

    QTime T3 = QTime::currentTime();

    if(bDebugMode)
    {
        p.setPen(QColor(0,0,0));
        QString msg;
        msg.sprintf("%d ms elapsed, %d ms for last thread. %dx%d points.",
                    T1.msecsTo(T3),
                    pt.getElapsedTime(),
                    (W-2*border),
                    (H-2*border)
                    );
        p.drawText(10, H - fontMetrics().descent(), msg);
    }
    p.end();
}

//------------------------------------------------------------------------------
bool QFieldView::bHitField(int x, int y)
{
    if( (x>=border) && (x<width()-border-minimap) &&
            (y>=border) && (y<height()-border) )
        return true;

    return false;
}
//------------------------------------------------------------------------------
bool QFieldView::bHitMap(int x, int y)
{
    if(bTimeMode)
    {
        if( (x>=width()-border-minimap) && (x<width()-border) &&
                (y>=border) && (y<height()-border) )
            return true;
    }
    else
    {
        if(bIko)
        {
            if( (x>=width()-border-minimap) && (x<width()-border) &&
                    (y>=height()-border-minimap) && (y < height() - border)  )
                return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------
bool QFieldView::bHitPie(int x, int y)
{
    if((!bTimeMode) && (bIko))
    {
        if( (x>=width()-border-minimap) && (x<width()-border) &&
                (y>=border) && (y < height() - 2*border - minimap)  )
            return true;
    }
    return false;
}
//------------------------------------------------------------------------------
void QFieldView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        if( bHitField(e->x(), e->y()) ) // Попадание в поле
        {
            if((tool == VT_TRACE) && (dragState == DS_NONE) )
                tracePoint(e->x(), e->y());
            if(tool == VT_ZOOM)
                startZoomDrag(e->x(), e->y());
            if(tool == VT_PAN)
                startPanDrag(e->x(), e->y());
        }

        if( bHitMap(e->x(), e->y()) ) // Попадание в миникарту
        {
            dragState = DS_MINIMAP;
            moveScale(e->x(), e->y());
        }

        if(bHitPie(e->x(), e->y()))
        {
            changePieIndex( e->x(), e->y() );
        }
    }// Левая кнопка

    if(e->button() == Qt::RightButton)
    {
        if(tool == VT_ZOOM)
        {
            if(bTimeMode)
            {
                // Достаем из стека предыдущее значение (если оно есть, конечно).
                if( !(scaleStack.isEmpty()) )
                {
                    ScaleParams s = scaleStack.pop();
                    memcpy(scale, &s, sizeof(s));
                    invalidateImage();
                    repaint();
                    checkChannelBounds();
                    emit scaleChanged();
                    setTracePoint(r_channel, t_channel);
                }
            }
            else
            {
                if(bIko)
                {
                    if( !(ikoScaleStack.isEmpty()) )
                    {
                        IKO_ScaleParams s = ikoScaleStack.pop();
                        memcpy(ikoScale, &s, sizeof(s));
                        invalidateImage();
                        repaint();
                        checkChannelBounds();
                        emit scaleChanged();
                        setTracePoint(traceRange, traceAngle);
                    }
                }
            }
        }
    }// Правая кнопка

    if(e->button() == Qt::MiddleButton) // Средняя кнопка
    {
        if( bHitField(e->x(), e->y()) ) // Попадание в поле
        {
            startPanDrag(e->x(), e->y());
        }
    } // Средняя кнопка
}
//------------------------------------------------------------------------------
void QFieldView::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton) //Тащим левой кнопкой
    {
        int X = e->x();
        int Y = e->y();
        if((tool == VT_TRACE) && (dragState == DS_NONE) )
        {
            if(bHitField(X, Y))
                tracePoint(X, Y);
        }

        if(dragState == DS_ZOOM_FRAME) // Уже тащим рамку
        {
            // Вкидываем координаты в пограничные значения для поля.
            if(X<border) X = border;
            if(X>=width() - border - minimap) X = width() - border - minimap - 1;
            if(Y<border) Y = border;
            if(Y>=height() - border) Y = height() - border - 1;
            keepZoomDrag(X, Y);
        }

        if(dragState == DS_MINIMAP)
        {
            // Вкидываем координаты в пограничные значения для миникарты.
            if(X<width() - border - minimap) X = width() - border - minimap;
            if(X>=width() - border) X = width() - border - 1;
            if(bTimeMode)
            {
                if(Y<border) Y = border;
                if(Y>=height() - border) Y = height() - border - 1;
            }
            else
            {
                if(bIko)
                {
                    if(Y<height() - border - minimap) Y = height() - border - minimap;
                    if(Y>=height() - border) Y = height() - border - 1;
                }
            }
            moveScale(X, Y);
        }

        if((dragState == DS_PAN) && (tool == VT_PAN)) // Тащим поле
        {
            /*      if(X<border) X = border;
      if(X>=width() - border - minimap) X = width() - border - minimap - 1;
      if(Y<border) Y = border;
      if(Y>=height() - border) Y = height() - border - 1;*/
            keepPanDrag(X, Y);
        }
    }// Левая кнопка

    if(e->buttons() == Qt::MiddleButton)
    {
        keepPanDrag(e->x(), e->y());
    }// Средняя кнопка
}
//------------------------------------------------------------------------------
void QFieldView::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton ) // Отпустили левую кнопку
    {
        // Можем закончить тащить где угодно
        if( (tool == VT_ZOOM) && (dragState == DS_ZOOM_FRAME) )
            endZoomDrag();
    }// Левая кнопка

    // Отпускаем миникарту если мы ее тащили
    if(dragState == DS_MINIMAP)
        dragState = DS_NONE;

    // Отпускаем перетаскивание
    if(dragState == DS_PAN)
        dragState = DS_NONE;
}
//------------------------------------------------------------------------------
void QFieldView::keyReleaseEvent(QKeyEvent *e)
{
    int step = 1;
    if( e->modifiers() & Qt::ControlModifier )
        step = 10;
    bool bChannelMoved = false;
    bool bScaleChanged = false;
    if(bTimeMode)
    {
        int scale_h = scale->newer - scale->older;
        int scale_w = scale->far - scale->close;

        switch(e->key())
        {
        case Qt::Key_Up:
            t_channel += step;
            bChannelMoved = true;
            break;
        case Qt::Key_Down:
            t_channel -= step;
            bChannelMoved = true;
            break;
        case Qt::Key_Left:
            r_channel -= step;
            bChannelMoved = true;
            break;
        case Qt::Key_Right:
            r_channel += step;
            bChannelMoved = true;
            break;
        case Qt::Key_PageUp:
            if(e->modifiers() & Qt::ControlModifier)
            {
                scale->close += scale_w;
                scale->far   += scale_w;
            }
            else
            {
                scale->newer += scale_h;
                scale->older += scale_h;
            }
            bScaleChanged = true;
            break;
        case Qt::Key_PageDown:
            if(e->modifiers() & Qt::ControlModifier)
            {
                scale->close -= scale_w;
                scale->far   -= scale_w;
            }
            else
            {
                scale->newer -= scale_h;
                scale->older -= scale_h;
            }
            bScaleChanged = true;
            break;
        }

        if(bChannelMoved)
        {
            checkChannelBounds();
            setTracePoint(r_channel, t_channel);
        }

        if(bScaleChanged)
        {
            checkScaleBounds();
            setTracePoint(r_channel, t_channel);
            emit scaleChanged();
        }
    } // if(bTimeMode)
    else
    {
        if(bIko)
        {
            double scale_size = ikoScale->size * 2;

            switch(e->key())
            {
            case Qt::Key_Up:
                traceRange += ikoMetrics->rangeStep * step;
                bChannelMoved = true;
                break;
            case Qt::Key_Down:
                traceRange -= ikoMetrics->rangeStep * step;
                bChannelMoved = true;
                break;
            case Qt::Key_Left:
                traceAngle -= step;
                bChannelMoved = true;
                break;
            case Qt::Key_Right:
                traceAngle += step;
                bChannelMoved = true;
                break;
            case Qt::Key_PageUp:
                if(e->modifiers() & Qt::ControlModifier)
                {
                    ikoScale->x_mid += scale_size;
                }
                else
                {
                    ikoScale->y_mid += scale_size;
                }
                bScaleChanged = true;
                break;
            case Qt::Key_PageDown:
                if(e->modifiers() & Qt::ControlModifier)
                {
                    ikoScale->x_mid -= scale_size;
                }
                else
                {
                    ikoScale->y_mid -= scale_size;
                }
                bScaleChanged = true;
                break;
            }

            if(bChannelMoved)
            {
                checkChannelBounds();
                setTracePoint(traceRange, traceAngle);
            }

            if(bScaleChanged)
            {
                checkScaleBounds();
                setTracePoint(traceRange, traceAngle);
                emit scaleChanged();
            }
        }
    }
}
//------------------------------------------------------------------------------
void QFieldView::resizeEvent(QResizeEvent *e)
{
    if(bCheckResize)
        if(pt.isFinished())
            invalidateImage();
}
//------------------------------------------------------------------------------
void QFieldView::wheelEvent(QWheelEvent *e)
{
    if(bTimeMode) // Отрисовка прямоугольника
    {
        int scale_w = scale->far   - scale->close;
        int scale_h = scale->newer - scale->older;

        // Найдем исходные координаты курсора, до изменения масштаба
        QPoint mc1 = getDataPointByPixel(e->pos());

        if(e->delta() > 0)
        {
            scale_w /= 2;
            scale_h /= 2;

            if(scale_w == 0) scale_w = 1;
            if(scale_h == 0) scale_h = 1;

            scale->close += scale_w / 4;
            scale->far   -= scale_w / 4;
            scale->newer -= scale_h / 4;
            scale->older += scale_h / 4;
        }
        else
        {
            // Бывает и такое
            if(scale_w < 2) scale_w = 2;
            if(scale_h < 2) scale_h = 2;

            scale->close -= scale_w / 2;
            scale->far   += scale_w / 2;
            scale->newer += scale_h / 2;
            scale->older -= scale_h / 2;
        }

        // Координаты курсора после изменения масштаба
        QPoint mc2 = getDataPointByPixel(e->pos());
        scale->close += mc1.x() - mc2.x();
        scale->far   += mc1.x() - mc2.x();
        scale->newer += mc1.y() - mc2.y();
        scale->older += mc1.y() - mc2.y();
    }
    else
    {
        if(bIko) // Отрисовка ИКО
        {
            // Положение мышки до изменения масштаба
            QPointF mc1 = getCoordsByPixel(e->pos());
            // Мышка после масштабирования должна стоять на тех же координатах
            if(e->delta() > 0)
                ikoScale->size /= 2;
            else
                ikoScale->size *= 2;
            // И после. Нам надо вернуть масштаб "на исходную"
            QPointF mc2 = getCoordsByPixel(e->pos());
            // Задвигаем
            ikoScale->x_mid -= mc2.x() - mc1.x();
            ikoScale->y_mid -= mc2.y() - mc1.y();
        }
    }

    forceRepaint();
    clearZoomStack();
}
//------------------------------------------------------------------------------
void QFieldView::tracePoint(int x, int y)
{
//  QPoint pt0(x, y);
//  QPointF pt1 = getCoordsByPixel(pt0);
//  QPointF pt2 = getGeoByCoords(pt1);
//  QPointF pt3 = getCoordsByGeo(pt2);
//  if(pt3 != pt1)
//    printf("pt1(%g, %g), pt2(%g, %g)", pt1.x(), pt1.y(), pt3.x(), pt3.y());

    if(bTimeMode)
    {
        QPoint data_pt = getDataPointByPixel(x, y);
        setTracePoint(data_pt.x(), data_pt.y());
    }
    else
    {
        if(bIko)
        {
            QPointF coords_pt = getCoordsByPixel(x, y);
            QPointF geo_pt = getGeoByCoords(coords_pt);
            if(geo_pt.x() > maxRange) geo_pt.setX(maxRange);
            setTracePoint(geo_pt.x(), (int)geo_pt.y());
        }
    }
}
//------------------------------------------------------------------------------
QPoint QFieldView::getDataPointByPixel(QPoint pixel, bool bMap)
{
    return getDataPointByPixel(pixel.x(), pixel.y(), bMap);
}
//------------------------------------------------------------------------------
QPoint QFieldView::getDataPointByPixel(int x, int y, bool bMap)
{
    // Трассируем карту.
    if(bMap)
        //  if(bHitMap(x, y))
    {
        int r_ch = floor(
                    1.0 * (x-(width()-border-minimap))*maxframesize / minimap
                    );
        int t_ch = -1 * floor(
                    1.0 * (y-border)*frameset->size() / (height() - 2*border)
                    );
        return QPoint(r_ch, t_ch);
    }

    // Трассируем поле
    /*  if(bHitField(x, y))
  {*/
    int r_ch = floor(
                1.0 * (x-border) * (scale->far - scale->close)
                / (width() - 2*border - minimap)
                ) + scale->close;
    int t_ch = scale->newer - 1 -
            floor(
                1.0 * (y-border) * (scale->newer - scale->older)
                / (height() - 2*border)
                );
    return QPoint(r_ch, t_ch);
    //  }

}
//------------------------------------------------------------------------------
QPoint QFieldView::getPixelByDataPoint(QPoint dataPoint)
{
    return getPixelByDataPoint(dataPoint.x(), dataPoint.y());
}
//------------------------------------------------------------------------------
QPoint QFieldView::getPixelByDataPoint(int r_ch, int t_ch)
{

    double psize_x, psize_y;
    int ssize = (scale->far - scale->close);
    if(ssize != 0)
        psize_x = 1.0 * (width() - 2*border - minimap)
                / ssize;
    else
        psize_x = 0;

    ssize = (scale->newer - scale->older);
    if(ssize != 0)
        psize_y = 1.0 * (height() - 2*border)
                / ssize;
    else
        psize_y = 0;

    int x = round (
                psize_x * (r_ch - (scale->close))
                + psize_x / 2
                )
            + border;
    int y = height() - border -
            round(
                psize_y*(t_ch - (scale->older))
                + psize_y / 2
                );

    /*  int x = floor(
                1.0 * (r_ch - (scale->r_base - scale->r_scale)) /
                (2 * scale->r_scale) *
                (width() - 2*border - minimap)
               ) + border;
  int y = floor(
                1.0 * (scale->t_base + scale->t_scale - 1 - t_ch) /
                (2 * scale->t_scale) *
                (height() - 2*border)
               ) + border;*/
    return QPoint(x, y);
}
//------------------------------------------------------------------------------
QPointF QFieldView::getCoordsByPixel(QPoint p, bool bMap)
{
    return getCoordsByPixel((double)p.x(), (double)p.y(), bMap);
}
//------------------------------------------------------------------------------
QPointF QFieldView::getCoordsByPixel(int x, int y, bool bMap)
{
    return getCoordsByPixel((double)x, (double)y, bMap);
}
//------------------------------------------------------------------------------
QPointF QFieldView::getCoordsByPixel(QPointF p, bool bMap)
{
    return getCoordsByPixel(p.x(), p.y(), bMap);
}
//------------------------------------------------------------------------------
QPointF QFieldView::getCoordsByPixel(double x, double y, bool bMap)
{
    if(bMap)
    {
        double real_x = (float)(x - width() + border + minimap) / minimap
                * 2 * maxRange
                - maxRange;
        double real_y = (float)(height() - border - y) /  minimap * 2 * maxRange
                - maxRange;
        return QPointF(real_x, real_y);
    }
    else
    {
        double scale;  // Сколько метров в одном пикселе
        int W = width() - 2*border - minimap;
        int H = height() - 2*border;
        if(width()>height()) // Масштаб ориентируется от высоты
            scale = ikoScale->size / H * 2;
        else // Масштаб ориентируется от ширины
            scale = ikoScale->size / W * 2;

        double real_x = scale * (x - border - W/2) + ikoScale->x_mid;
        double real_y = scale * (H/2 - (y-border)) + ikoScale->y_mid;
        return QPointF(real_x, real_y);
    }
}
//------------------------------------------------------------------------------
QPoint QFieldView::getPixelByCoords(QPointF p)
{
    return getPixelByCoords(p.x(), p.y());
}
//------------------------------------------------------------------------------
QPoint QFieldView::getPixelByCoords(double x, double y)
{
    if(ikoScale == 0) return QPoint(0,0);
    float scale; // Сколько пикселей приходится на метр
    int W = width() - 2*border - minimap;
    int H = height() - 2*border;
    if(ikoScale->size == 0)
    {
        scale = 0;
    }
    else
    {
        if(W>H)
            scale = H / ikoScale->size / 2;
        else
            scale = W / ikoScale->size / 2;
    }

    return QPoint(       round(scale * (x - ikoScale->x_mid)) + W/2 + border,
                  H/2 - round(scale * (y - ikoScale->y_mid)) + border
                  );
}
//------------------------------------------------------------------------------
QPointF QFieldView::getFloatPixelByCoords(QPointF p)
{
    return getFloatPixelByCoords(p.x(), p.y());
}
//------------------------------------------------------------------------------
QPointF QFieldView::getFloatPixelByCoords(double x, double y)
{
    float scale; // Сколько пикселей приходится на метр
    int W = width() - 2*border - minimap;
    int H = height() - 2*border;

    if(ikoScale->size > 0)
    {
        if(W>H)
            scale = H / ikoScale->size / 2;
        else
            scale = W / ikoScale->size / 2;
    }
    else
        scale = 0;

    return QPointF(      scale * (x - ikoScale->x_mid) + W/2 + border,
                   H/2 - scale * (y - ikoScale->y_mid) + border
                   );
}
//------------------------------------------------------------------------------
QPointF QFieldView::getCoordsByGeo(QPointF pt)
{
    return getCoordsByGeo(pt.x(), pt.y());
}
//------------------------------------------------------------------------------
QPointF QFieldView::getCoordsByGeo(double R, double A)
{
    if(ikoMetrics == 0)
        return QPointF(0,0);
    if(ikoMetrics->A_max == 0)
        return QPointF(0,0);
    double A_rad = M_PI * 2.0 * A / ikoMetrics->A_max;
    A_rad += M_PI / 180.0 * ikoMetrics->A_offset;
    double X = R * sin(A_rad);
    double Y = R * cos(A_rad);
    return QPointF(X, Y);
}
//------------------------------------------------------------------------------
QPointF QFieldView::getGeoByCoords(QPointF pt)
{
    return getGeoByCoords(pt.x(), pt.y());
}
//------------------------------------------------------------------------------
QPointF QFieldView::getGeoByCoords(double X, double Y)
{
    double range = sqrt(X*X + Y*Y);
    if(range == 0)
        return QPointF(0,0);
    double angle = 180.0/M_PI*acos(Y / range);

    if(X<0)
    {
        angle *= -1;
        angle += 360;
    }

    if(ikoMetrics->A_inverse)
        angle = 360-angle;

    // Добавим поправку на уголовое смещение
    angle -= ikoMetrics->A_offset;

    // Затолкаем угол в диапазон [0, 360)
    while(angle < 0   ) angle += 360;
    while(angle >= 360) angle -= 360;

    int angle_mark = round(angle / 360.0 * ikoMetrics->A_max);

    return QPointF(range, angle_mark);
}
//------------------------------------------------------------------------------
QRect QFieldView::getImageBound(ScaleParams imageScale)
{
    QRect res;
    double frac;

    // Какую часть составляет левая сторона картинки от всей ширины поля

    int ssize = scale->far - scale->close;
    if(ssize!=0)
        frac = 1.0 *
                (
                    imageScale.close - scale->close
                    )
                / ssize;
    else
        frac = 0;

    res.setLeft(
                border
                + round( frac * (width() - 2*border - minimap) )
                );

    // То же самое для правой стороны
    if(ssize != 0)
        frac = 1.0 *
                (
                    imageScale.far - scale->close
                    )
                / ssize;
    else
        frac = 0;

    res.setRight( border
                 + round( frac * (width() - 2*border - minimap) )
                 );

    // Повторяем все для времени: "свежий" канал
    ssize = scale->newer - scale->older;
    if(ssize != 0)
        frac = 1.0 *
                (
                    imageScale.older - scale->older
                    )
                / ssize;
    else
        frac = 0;

    res.setBottom( height() - border
                  - round(frac * ( height() - 2*border ) )
                  );

    if(ssize != 0)
        frac = 1.0 *
                (
                    imageScale.newer - scale->older
                    )
                / ssize;
    else
        frac = 0;
    res.setTop( height() - border
               - round(frac * ( height() - 2*border ) )
               );

    return res;
}
//------------------------------------------------------------------------------
QRect QFieldView::getImageBound(IKO_ScaleParams imageScale)
{
    QRect res;
    if((fieldImage.width() == 0) || (fieldImage.height() == 0))
    {
        // нечего рисовать
        res.setRect(0, 0, 0, 0);
        return res;
    }
    // Найдем центр изображения
    QPoint img_mid = getPixelByCoords(imageScale.x_mid, imageScale.y_mid);
    int imgW, imgH;
    int w = fieldImage.width();
    int h = fieldImage.height();
    // Отношение высоты к ширине. Пригодится.
    float ratio = (float)fieldImage.height() / (float)fieldImage.width();
    // Во сколько раз картинка отличается от текущего масштаба
    float rescale;
    if(ikoScale->size == 0)
    {
        if(imageScale.size == 0)
            rescale = 1; // Нет данных, нулевые масштабы
        else
            rescale = 0; // Какая-то ошибка. Нечего рисовать.
    }
    else
        rescale = imageScale.size / ikoScale->size;

    if(fieldImage.width() > fieldImage.height()) // Масштаб по высоте картинки
    {
        imgH = floor(rescale * ( height() - 2*border));
        imgW = floor( (float)imgH / ratio );
    }
    else // Масштаб по ширине картинки
    {
        imgW = floor(rescale * (width() - 2*border - minimap) );
        imgH = floor( (float)imgW * ratio );
    }
    res.setLeft  ( img_mid.x() - imgW/2 );
    res.setRight ( img_mid.x() + imgW/2 );
    res.setTop   ( img_mid.y() - imgH/2 );
    res.setBottom( img_mid.y() + imgH/2 );
    return res;
}
//------------------------------------------------------------------------------
void QFieldView::setTracePoint(int rangeChannel, int timeChannel)
{
    r_channel = rangeChannel;
    t_channel = timeChannel;
    checkChannelBounds();
    // Задание спинбоксов будет повторным при тыкании "извне". Здесь оно нужно
    // для тыкания "изнутри" через tracePoint

    emit pointPicked(r_channel, t_channel);
    repaint();
}
//------------------------------------------------------------------------------
void QFieldView::setTracePoint(double range, int angle)
{
    if(frameLimits == 0) return;


    int fr_min=-1, fr_max=-1; // Пределы кадра в номерах строк
    // Нижний предел
    int i = frameLimits[2*fullViewIndex];
    if(i == -1)
    {
        printf("Error: Index for minimal angle is -1\n");
        return;
    }
    else
        fr_min = i;

    // Верхний предел
    i = frameLimits[2*fullViewIndex+1];
    if(i == -1)
    {
        printf("Error: Index for maximal angle is -1\n");
        return;
    }
    else
        fr_max = i;

    // В случае инверсного порядка следования кадров (хотя маловероятно)
    if(fr_min > fr_max)
    {
        int t = fr_max;
        fr_max = fr_min;
        fr_min = t;
    }
    traceRange = range;
    traceAngle = angle;
    checkChannelBounds();
    i = indices [fullViewIndex * ikoMetrics->A_max * NUM_LAYERS
                     + angle * NUM_LAYERS ];

    if(i != -1)
    {
        int trace_fr = i;
        emit pointPicked(range, trace_fr, fr_min, fr_max);
    }
    else
    {
        printf("Error: index for tracing angle is -1\n");
    }
    repaint();

}
//------------------------------------------------------------------------------
void QFieldView::checkChannelBounds()
{
    if(bTimeMode)
    {
        int var_min = scale->close;     // Минимальное значение
        int var_max = scale->far - 1; // Максимальное значение

        if(r_channel < var_min)
            r_channel = var_min;
        if(r_channel > var_max)
            r_channel = var_max;

        var_min = scale->older;     // Минимальное значение
        var_max = scale->newer - 1; // Максимальное значение

        if(t_channel < var_min)
            t_channel = var_min;
        if(t_channel > var_max)
            t_channel = var_max;
    }
    else
    {
        if(bIko)
        {
            double var_min, var_max;
            QPointF pt = getCoordsByGeo(traceRange, traceAngle);
            QPointF px = getFloatPixelByCoords(pt);
            var_min = border;
            var_max = width() - border - minimap;

            if(px.x() < var_min) px.setX(var_min);
            if(px.x() > var_max) px.setX(var_max);

            var_min = border;
            var_max = height() - border;

            if(px.y() < var_min) px.setY(var_min);
            if(px.y() > var_max) px.setY(var_max);

            QPointF px2 = getCoordsByPixel(px);
            QPointF pt2 = getGeoByCoords(px2);
            traceRange = pt2.x();
            traceAngle = pt2.y();
        }
    }
}
//------------------------------------------------------------------------------
void QFieldView::checkScaleBounds()
{
    if(bTimeMode)
    {
        // Проверим, влезаем ли мы вообще в границы.
        if(scale->far - scale->close > maxframesize)
        {
            // Если вдруг мы хотим показать больше, чем имеем
            // (что в принципе не вызовет ошибку в отрисовке поля)
            scale->close = 0;
            scale->far = maxframesize;
            //    scale->r_scale += scale->r_scale % 2; // Округляем вверх до четного.
        }

        // Аналогично для времени
        if(scale->newer - scale->older > frameset->size())
        {
            // Если вдруг мы хотим показать больше, чем имеем
            // (что в принципе не вызовет ошибку в отрисовке поля)
            scale->newer = 1;
            scale->older = 1 - frameset->size();
            //    scale->t_scale += scale->t_scale % 2; // Округляем вверх до четного.
        }

        // Проверка на переворачивание границ слева-справа
        int tmp;
        if(scale->close > scale->far)
        {
            tmp=scale->far;
            scale->far = scale->close;
            scale->close = tmp;
        }
        // Проверка на переворачивание верх-низ
        if(scale->older > scale->newer)
        {
            tmp=scale->older;
            scale->older = scale->newer;
            scale->newer = tmp;
        }

        if(scale->close == scale->far) // Совпал масштаб
            scale->far += 1;
        if(scale->newer == scale->older)
            scale->newer += 1;

        // Проверка левой границы
        int diff=scale->close;
        if(diff < 0)
        {
            scale->close = 0;
            scale->far -= diff;
        }
        // Проверка правой границы
        diff = scale->far - maxframesize;
        if(diff > 0)
        {
            scale->far = maxframesize;
            scale->close -= diff;
        }

        //Проверка верхней границы
        diff = scale->newer - 1;
        if(diff > 1)
        {
            scale->newer = 1;
            scale->older -= diff;
        }
        //Проверка нижней границы
        diff = 1 - frameset->size() - scale->older;
        if(diff > 0)
        {
            scale->newer += diff;
            scale->older = 1 - frameset->size();
        }
    }
    else
    {
        if(bIko)
        {
            // Проверка на размеры
            if(ikoScale->size > maxRange)
            {
                ikoScale->size = maxRange;
                ikoScale->x_mid = 0;
                ikoScale->y_mid = 0;
                return;
            }

            // Правая граница
            float diff = ikoScale->x_mid + ikoScale->size - maxRange;
            if(diff > 0)
                ikoScale->x_mid -= diff;

            // Леваяграница
            diff = ikoScale->x_mid - ikoScale->size + maxRange;
            if(diff < 0)
                ikoScale->x_mid -= diff;

            // Верхняя граница
            diff = ikoScale->y_mid + ikoScale->size - maxRange;
            if(diff > 0)
                ikoScale->y_mid -= diff;

            // Нижняя граница
            diff = ikoScale->y_mid - ikoScale->size + maxRange;
            if(diff < 0)
                ikoScale->y_mid -= diff;
        }
    }
}
//------------------------------------------------------------------------------
void QFieldView::invalidateImage()
{
    // Отключаем бегущий поток
    if(pt.isRunning())
    {
        bRedrawFlag = true;
    }
    else
    {
        bRedrawFlag = false;
        // Отправляем потоку новые данные.
        QRect r(border, border, width()-2*border-minimap, height()-2*border);
        pt.setBound(r);
        pack.indices = indices + ikoMetrics->A_max * fullViewIndex * NUM_LAYERS;
        pt.setDrawData(pack);
        pt.start();
    }
}
//------------------------------------------------------------------------------
void QFieldView::onThreadFinished()
{
    pt.wait(1000);
    pt.fillImage(fieldImage);
    // Запоминаем масштаб картинки, достав из потока тот масштаб, который
    // мы ему отправили ранее.
    memcpy(&imageScale, pt.getScale(), sizeof(imageScale));
    memcpy(&imageIkoScale, pt.getIkoScale(), sizeof(imageIkoScale));

    emit paintFinished();

    repaint();

    if(bRedrawFlag)
        invalidateImage();
}
//------------------------------------------------------------------------------
void QFieldView::startZoomDrag(int x, int y)
{
    dragState = DS_ZOOM_FRAME;
    dragStart.setX(x);
    dragStart.setY(y);
    dragEnd = dragStart;
    repaint();
}
//------------------------------------------------------------------------------
void QFieldView::keepZoomDrag(int x, int y)
{
    if(bTimeMode)
    {
        dragEnd.setX(x);
        dragEnd.setY(y);
    }
    else
    {
        if(bIko)
        {
            // Обрежем прямоугольник выделения до квадрата
            int x0 = dragStart.x();
            int y0 = dragStart.y();
            int w = abs(x - x0);
            int h = abs(y - y0);

            if(w>h) w=h;
            else    h=w;

            if(x>x0)
                dragEnd.setX(x0+w);
            else
                dragEnd.setX(x0-w);

            if(y>y0)
                dragEnd.setY(y0+h);
            else
                dragEnd.setY(y0-h);
        }
    }
    repaint();
}
//------------------------------------------------------------------------------
void QFieldView::endZoomDrag()
{
    dragState = DS_NONE;
    if(bTimeMode)
    {
        QPoint start = getDataPointByPixel(dragStart);
        QPoint end   = getDataPointByPixel(dragEnd);

        scaleStack.push(*scale); // Сохраняем предыдущий масштаб

        if(start.x() < end.x())
        {
            scale->close = start.x();
            scale->far = end.x() + 1;
        }
        else
        {
            scale->close = end.x();
            scale->far = start.x() + 1;
        }

        if(start.y() < end.y())
        {
            scale->older = start.y();
            scale->newer = end.y() + 1;
        }
        else
        {
            scale->older = end.y();
            scale->newer = start.y() + 1;
        }

    }
    else
    {
        if(bIko)
        {
            QPointF start = getCoordsByPixel(dragStart);
            QPointF end = getCoordsByPixel(dragEnd);
            ikoScaleStack.push(*ikoScale);

            ikoScale->x_mid = 0.5 * (start.x() + end.x());
            ikoScale->y_mid = 0.5 * (start.y() + end.y());
            ikoScale->size = 0.5 * fabs(end.x() - start.x()); // Радиус!
        }
    }
    checkScaleBounds();
    checkChannelBounds();
    // Проверять на границы области отображения тут не надо

    invalidateImage();
    repaint();
    // Впихиваем трассировку в полученное окно и вызываем перерисовку графиков
    if(bTimeMode)
    {
        setTracePoint(r_channel, t_channel);
    }
    else
    {
        if(bIko)
        {
            setTracePoint(traceRange, traceAngle);
        }
    }
}
//------------------------------------------------------------------------------
void QFieldView::startPanDrag(int x, int y)
{
    dragState = DS_PAN;
    panCenter.setX(x);
    panCenter.setY(y);
}
//------------------------------------------------------------------------------
void QFieldView::keepPanDrag(int x, int y)
{
    if(bTimeMode)
    {
        QPoint pt1 = getDataPointByPixel(panCenter);
        QPoint pt2 = getDataPointByPixel(x, y);
        // Переносим panCenter в эту точку.
        scale->close -= pt2.x() - pt1.x();
        scale->far -= pt2.x() - pt1.x();
        scale->newer -= pt2.y() - pt1.y();
        scale->older -= pt2.y() - pt1.y();
        // Обновляем, чтобы не "ползло".
        startPanDrag(x, y);
    }
    else
    {
        if(bIko)
        {
            QPointF pt1 = getCoordsByPixel(panCenter);
            QPointF pt2 = getCoordsByPixel(x, y);
            // Переносим
            ikoScale->x_mid -= pt2.x() - pt1.x();
            ikoScale->y_mid -= pt2.y() - pt1.y();
            startPanDrag(x, y);
        }
    }
    checkScaleBounds();
    checkChannelBounds();
    invalidateImage();
    repaint();
}
//------------------------------------------------------------------------------
void QFieldView::moveScale(int x, int y)
{
    if(bTimeMode)
    {
        // Запросим "центр", в который надо передвинуть окно отображения.
        // В случае, если координаты x и y принадлежат миникарте, та же функция
        // вернет адекватные значения.
        QPoint scale_pt = getDataPointByPixel(x, y, true);
        int middle = (scale->far + scale->close) / 2;
        scale->close += scale_pt.x() - middle;
        scale->far   += scale_pt.x() - middle;
        middle = (scale->newer + scale->older) / 2;
        scale->older += scale_pt.y() - middle;
        scale->newer += scale_pt.y() - middle;
        checkScaleBounds();
        checkChannelBounds();
        invalidateImage();
        repaint();
        emit scaleChanged();
        setTracePoint(r_channel, t_channel);
    }
    else
    {
        if(bIko)
        {
            QPointF scale_pt = getCoordsByPixel (x, y, true);
            ikoScale->x_mid = scale_pt.x();
            ikoScale->y_mid = scale_pt.y();
            checkScaleBounds();
            invalidateImage();
            repaint();
            emit scaleChanged();
            setTracePoint(traceRange, traceAngle);
        }
    }
}
//------------------------------------------------------------------------------
void QFieldView::changePieIndex(int x, int y)
{
    int idx = (height() - border - minimap - y) / (piesize + border);
    if(idx < NUM_CIRCLES)
    {
        if((frameLimits[2*idx] != -1) || (frameLimits[2*idx+1] != -1))
            fullViewIndex = idx;
    }
    invalidateImage();
    setTracePoint(traceRange, traceAngle);
    repaint();
}
//------------------------------------------------------------------------------
void QFieldView::forceRepaint()
{
    checkScaleBounds();
    checkChannelBounds();
    invalidateImage();
//    repaint();
    //  emit scaleChanged(r_channel, t_channel);
    if(bTimeMode)
    {
        emit pointPicked(r_channel, t_channel);
    }
    else
    {
        if(bIko)
        {
            setTracePoint(traceRange, traceAngle); // Там и сигнал
        }
    }
}
//------------------------------------------------------------------------------
void QFieldView::clearZoomStack()
{
    scaleStack.clear();
    ikoScaleStack.clear();
}
//------------------------------------------------------------------------------
bool QFieldView::isFramesetLocked()
{
    return pt.isRunning();
}
//------------------------------------------------------------------------------
bool QFieldView::waitForPaintThread(int msec)
{
    return pt.wait(msec);
}
//------------------------------------------------------------------------------
bool QFieldView::setPaletteFile(QString f)
{
    if(f.isEmpty())
    {
        setDefaultPalette();
        return true; // Без ошибок, просто выходим
    }
    while(pt.wait(10) == false)
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // не переписываем палитру при работающем потоке

    QFile F(f);
    if(!F.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(
            0,
            QString::fromUtf8("Ошибка файла палитры"),
            QString::fromUtf8("Не найден указанный файл палитры")
            );
        setDefaultPalette();
        return false;
    }
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
        setDefaultPalette();
        return false;
    }
    if( (p.header.version != PV_4RGBF) &&
        (p.header.version != PV_4RGBA) &&
        (p.header.version != PV_4RGB0)
      )
    {
        F.close();
        QMessageBox::critical(
            0,
            QString::fromUtf8("Ошибка открытия файла"),
            QString::fromUtf8("Файл палитры должен быть в 4-байтном формате")
            );
        setDefaultPalette();
        return false;
    }
    delete [] palette.data;
    memcpy(&palette.header, &p.header, sizeof(p.header));
    palette.data = new uint8_t[palette.header.numColors*pxsize];
    F.read((char*)palette.data, palette.header.numColors*pxsize);
    F.close();

    invalidateImage();

    return true;
}
//------------------------------------------------------------------------------
void QFieldView::rebuildIndices()
{
    while(pt.wait(10) == false)
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    int Amax = ikoMetrics->A_max; // Много раз понадобится
    delete [] indices;
    int arsize = Amax * NUM_CIRCLES * NUM_LAYERS; // Пригодится
    indices = new int[arsize];
    pack.indices = indices;

    delete [] frameLimits;
    frameLimits = new int[2*NUM_CIRCLES];
    memset(frameLimits, 0xFF, 2*NUM_CIRCLES*sizeof(int));

    refillIndices();
}
//------------------------------------------------------------------------------
void QFieldView::refillIndices()
{
    QTime t;
    t.start();

    while(pt.wait(10) == false)
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    int Amax = ikoMetrics->A_max; // Много раз понадобится

    if((indices == 0) || (frameLimits == 0))
        return;

    // Очистка углов
    memset((char*)indices, 0xFF, Amax * NUM_CIRCLES * NUM_LAYERS*sizeof(int));

    // Очистка пределов кадров
    memset((char*)frameLimits, 0xFF, 2*NUM_CIRCLES*sizeof(int));

    if(frameset == 0)       return;
    if(frameset->isEmpty()) return;

    int lastframe = frameset->size() - 1;

    // Найдем, в какую сторону растут угловые отсчеты
    int num_positive = 0;
    int num_negative = 0;
    for(int a=0; a<frameset->size()-1; a++)
    {
        // Пропуск левых строк
        if((frameset->at(a).angle > Amax) || frameset->at(a).angle == 0x7FFF)
            continue;
        if(frameset->at(a).angle < frameset->at(a+1).angle)
            num_positive++;
        if(frameset->at(a).angle > frameset->at(a+1).angle)
            num_negative++;
    }
    bInverseFrames = (num_negative > num_positive);

    // Индекс элемента в массиве строк.
    int fr = lastframe;
    // Индекс "круга", т.е. номера обзора.
    int circle_index = 0;
    // Опорный угол, на котором закончился последний кадр.
    int A0 = frameset->at(lastframe).angle;
    frameLimits[0] = lastframe;

    if(bInverseFrames)
        A0 --;
    else
        A0 ++;

    int idx1, idx2;

    // Пошли заполнять таблицу
    while(fr>0)
    {
        int a_this = frameset->at(fr).angle;
        if((a_this > Amax)||(a_this == 0x7FFF))  // Пропуск левых строк.
        {
            fr --;
            continue;
        }
        if(a_this < A0) a_this += Amax;
        int a_prev = frameset->at(fr-1).angle;
        if(a_prev < A0) a_prev += Amax;

        // Найдем пределы углов, заполняемых данной линией.
        // Тут углы могут вылезать за края
        idx1 =   frameset->at(fr).angle
                - ikoMetrics->angleSKO;
        idx2 =   frameset->at(fr).angle
                + ikoMetrics->angleSKO + 1;

        // Заполним нужные ячейки. Перекрытие обзоров нам не сильно важно.
        for(int a=idx1; a<idx2; a++)
        {
            // Номер элемента в строке
            int idx = a;
            // Вгоняем его в пределы [0..Amax)
            while(idx<0) idx += Amax;
            while(idx>=Amax) idx -= Amax;
            // Вычисляем индекс с учетом многострочности массива
            idx *= NUM_LAYERS;
            idx += circle_index * Amax * NUM_LAYERS;
            int b;
            for(b=0; b<NUM_LAYERS; b++)
                if(indices[idx+b]==-1)
                {
                    indices[idx+b] = fr;
                    break;
                }
        }

        if(bInverseFrames)
        {
            if(a_this > a_prev)
            {
                frameLimits[2*circle_index+1] = fr;
                circle_index ++; // Нашли границу кадра
                if(circle_index == NUM_CIRCLES) break;
                frameLimits[2*circle_index] = fr-1;
            }
        }
        else
        {
            if(a_this < a_prev)
            {
                frameLimits[2*circle_index+1] = fr;
                circle_index ++; // Нашли границу кадра
                if(circle_index == NUM_CIRCLES) break;
                frameLimits[2*circle_index] = fr-1;
            }
        }

        fr --;

    }

    if(circle_index < NUM_CIRCLES)
    {
        // Закинем последний предел кадра
        frameLimits[2*circle_index+1] = 0;

        // Забьем в массив последнюю строку
        idx1 =   frameset->at(fr).angle
                - ikoMetrics->angleSKO;
        idx2 =   frameset->at(fr).angle
                + ikoMetrics->angleSKO + 1;

        // Заполним нужные ячейки. Перекрытие обзоров нам не сильно важно.
        for(int a=idx1; a<idx2; a++)
        {
            // Номер элемента в строке
            int idx = a;
            // Вгоняем его в пределы [0..Amax)
            while(idx<0) idx += Amax;
            while(idx>=Amax) idx -= Amax;
            // Вычисляем индекс с учетом многострочности массива
            idx *= NUM_LAYERS;
            idx += circle_index * Amax * NUM_LAYERS;
            int b;
            for(b=0; b<NUM_LAYERS; b++)
                if(indices[idx]==-1)
                {
                    indices[idx] = fr;
                    break;
                }
        }
    }

    printf("%d ms for refilling indices\n", t.elapsed());
}
//------------------------------------------------------------------------------
void QFieldView::setThreshold(float thr)
{
    threshold = thr;
    pack.threshold = thr;
    forceRepaint();
//    invalidateImage();
}
