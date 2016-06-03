#include "viewwindow.h"
#include "ui_viewwindow.h"
#include "ui_viewwindow_help.h"

#include <stdio.h>

#include <QFileInfo>
#include <QFileDialog>
#include <QtDebug>

ViewWindow::ViewWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewWindow)
{
    r_channel = 0;
    t_channel = 0;
    traceRange = 0;
    trace_frame = 0;
    trace_minFrame = 0;
    trace_maxFrame = 0;

    memset(&ikoMetrics, 0, sizeof(ikoMetrics));
    ikoMetrics.A_max = 10000;
    ikoMetrics.rangeStep = 1;
    ikoMetrics.minRange_LFM = 100;
    ikoMetrics.minRange_SKI = 100;

    memset(&scale, 0, sizeof(scale));

    memset(&ikoScale, 0, sizeof(ikoScale));
    ikoScale.size = 1000;

    //--------------
    // Настройка UI
    //--------------
    ui->setupUi(this);
    ui->field_widget->timeMode();
    ui->range_plot->setAxisTitle(QwtPlot::xBottom,
                                 QString::fromUtf8("Расстояние"));
    ui->range_plot->setAxisTitle(QwtPlot::yLeft,
                                 QString::fromUtf8("A/RE/IM"));
    ui->time_plot->setAxisTitle(QwtPlot::xBottom,
                                QString::fromUtf8("Время"));
    ui->time_plot->setAxisTitle(QwtPlot::yLeft,
                                QString::fromUtf8("A/RE/IM"));
    ui->field_widget->installEventFilter(this);

    //----------------------------
    // Настройка окна справки
    //----------------------------
    helpWindow = new QWidget(0);
    help_ui = new Ui::ViewWindow_Help;
    help_ui->setupUi(helpWindow);
    helpWindow->installEventFilter(this);
    helpWindow->hide();

    //----------------------------
    // Настройка внутренней кухни
    //----------------------------

    maxFrames = 100000;
    bFreeSpinBoxes = true;
    maxframesize = 0;
    bTimeKeep = false;
    memset(&scale, 0, sizeof(scale));
    memset(&ikoScale, 0, sizeof(ikoScale));
    memset(&ikoMetrics, 0, sizeof(ikoMetrics));

    // Создадим кривые для графиков
    for(int a=0; a<MAX_CURVES; a++)
    {
        curves[a] = new QwtPlotCurve();
        bCurves[a] = true;
    }
    // Задаем цвета из значений по-умолчанию.
    QPen re_pen ( QColor((QRgb)GRAPH_RE_COLOR) );
    QPen im_pen ( QColor((QRgb)GRAPH_IM_COLOR) );
    QPen abs_pen ( QColor((QRgb)GRAPH_ABS_COLOR) );
    // Кривые первого графика
    curves[0]->setPen(re_pen);
    curves[1]->setPen(im_pen);
    curves[2]->setPen(abs_pen);
    // Кривые второго графика
    curves[3]->setPen(re_pen);
    curves[4]->setPen(im_pen);
    curves[5]->setPen(abs_pen);

    // Создадим сетки на графиках
    QPen gridPen( QColor(192,192,192) );  // Сетка - серая.
    rangeGrid = new QwtPlotGrid;
    rangeGrid->setPen(gridPen);
    rangeGrid->attach(ui->range_plot);
    timeGrid = new QwtPlotGrid;
    timeGrid->setPen(gridPen);
    timeGrid->attach(ui->time_plot);

    // Зумер "по дальности"
    rangeZoomer = new QwtPlotZoomer(ui->range_plot->canvas());
    //rangeZoomer->setMaxStackDepth(1);
    //Полный анзум делается по Ctrl+RMB
    rangeZoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton,
                                 Qt::ControlModifier);
    // Однократный анзум делается по правой кнопке.
    rangeZoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    // Зумер "по времени"
    timeZoomer = new QwtPlotZoomer(ui->time_plot->canvas());
    //timeZoomer->setMaxStackDepth(1);
    //Полный анзум делается по Ctrl+RMB
    timeZoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton,
                                Qt::ControlModifier);
    // Однократный анзум делается по правой кнопке.
    timeZoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    // паннер "по дальности"
    rangePanner = new QwtPlotPanner(ui->range_plot->canvas());
    rangePanner->setMouseButton(Qt::MiddleButton);

    // паннер "по времени"
    timePanner = new QwtPlotPanner(ui->time_plot->canvas());
    timePanner->setMouseButton(Qt::MiddleButton);

    // Маркеры
    QPen markerPen( QColor(255,255,255) );
    rangeMarker = new QwtPlotMarker;
    rangeMarker->setLinePen(markerPen);
    rangeMarker->setLineStyle(QwtPlotMarker::VLine);
    rangeMarker->setXValue(0);
    rangeMarker->attach(ui->range_plot);
    timeMarker = new QwtPlotMarker;
    timeMarker->setLinePen(markerPen);
    timeMarker->setLineStyle(QwtPlotMarker::VLine);
    timeMarker->setXValue(0);
    timeMarker->attach(ui->time_plot);

    // Тыкатели
    QPen pickerPen(QColor(255,0,0));
    rangePicker = new QwtPlotPicker(ui->range_plot->canvas());
    rangePicker->setRubberBand(QwtPlotPicker::NoRubberBand);
    rangePicker->setTrackerMode(QwtPlotPicker::AlwaysOff);
#if (QWT_VERSION >> 16) < 6
    rangePicker->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::DragSelection);
#else
    rangePicker->setStateMachine(new QwtPickerDragPointMachine);
#endif
    //  rangePicker->setMousePattern();

    timePicker = new QwtPlotPicker(ui->time_plot->canvas());
    timePicker->setRubberBand(QwtPlotPicker::NoRubberBand);
    timePicker->setTrackerMode(QwtPlotPicker::AlwaysOff);
#if (QWT_VERSION >> 16) < 6
    timePicker->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::DragSelection);
#else
    timePicker->setStateMachine(new QwtPickerDragPointMachine);
#endif

    // Маркеры для отображения тыкателей
    rangePickMarker = new QwtPlotMarker;
    rangePickMarker->setLinePen(pickerPen);
    rangePickMarker->setLineStyle(QwtPlotMarker::Cross);
    rangePickMarker->attach(ui->range_plot);
    rangePickMarker->setVisible(false);

    timePickMarker = new QwtPlotMarker;
    timePickMarker->setLinePen(pickerPen);
    timePickMarker->setLineStyle(QwtPlotMarker::Cross);
    timePickMarker->attach(ui->time_plot);
    timePickMarker->setVisible(false);

    ui->field_widget->setFrameset(&frameset);
    ui->field_widget->setScaleParams(&scale);
    ui->field_widget->setIkoScale(&ikoScale);
    ui->field_widget->setIkoMetrics(&ikoMetrics);

    updateButtons();
    flipMode();

    //----------
    // Коннекты
    //----------

    // Ну их нафиг пока, кнопки есть
    //  connect( ui->range_spinBox, SIGNAL(valueChanged(int)),
    //                              SLOT(onSpinBoxesUpdated()) );
    //  connect( ui->time_spinBox, SIGNAL(valueChanged(int)),
    //                             SLOT(onSpinBoxesUpdated()) );
    connect(ui->field_widget, SIGNAL(pointPicked(int,int)),
            SLOT(onPointPicked(int,int)));
    connect(ui->field_widget, SIGNAL(scaleChanged()),
            SLOT(onScaleChanged()));
    connect(ui->field_widget, SIGNAL(pointPicked(double,int,int,int)),
            SLOT(onIkoPointPicked(double,int,int,int)));
    connect(rangePicker, SIGNAL(moved(QPoint)),
            SLOT(onRangeGraphPointPicked(QPoint)) );
    connect(rangePicker, SIGNAL(appended(QPoint)),
            SLOT(onRangeGraphPointPicked(QPoint)) );
    connect(timePicker, SIGNAL(moved(QPoint)),
            SLOT(onTimeGraphPointPicked(QPoint)) );
    connect(timePicker, SIGNAL(appended(QPoint)),
            SLOT(onTimeGraphPointPicked(QPoint)) );

}

//------------------------------------------------------------------------------

ViewWindow::~ViewWindow()
{
    waitForPaint();
    int a;
    for(a=0; a<MAX_CURVES; a++)
        delete curves[a];
    delete rangeZoomer;
    delete rangePanner;
    delete rangeGrid;
    delete rangeMarker;
    delete rangePicker;
    delete rangePickMarker;
    delete timeZoomer;
    delete timePanner;
    delete timeGrid;
    delete timeMarker;
    delete timePicker;
    delete timePickMarker;
    delete ui;

    for(a=0; a<frameset.size(); a++)
        delete frameset[a].quads;

    delete help_ui;
    delete helpWindow;
}
//------------------------------------------------------------------------------
bool ViewWindow::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == helpWindow)
    {
        if(e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* me = (QMouseEvent*)e;
            if(me->button() == Qt::RightButton)
                helpWindow->hide();
        }
    }

    //  if(obj == ui->field_widget)
    //  {
    //    if(e->type() == QEvent::Wheel)
    //    {
    //      QWheelEvent* we = (QWheelEvent*)e;
    //      if(we->delta() > 0)
    //        on_doubleScale_button_clicked();
    //      if(we->delta() < 0)
    //        on_halfScale_button_clicked();
    //    }
    //  }
    return false;
}
//------------------------------------------------------------------------------
QFieldView* ViewWindow::fieldWidget()
{
    return ui->field_widget;  // КОСТЫЛЬ!
}
//------------------------------------------------------------------------------
void ViewWindow::onHelpWindowClosed()
{
    helpWindow->close();
}
//------------------------------------------------------------------------------
void ViewWindow::closeEvent(QCloseEvent *)
{
    helpWindow->close();
    waitForPaint(false);
    writeSettings();
}
//------------------------------------------------------------------------------

void ViewWindow::on_mode_comboBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->field_widget->timeMode();
        ui->field_widget->forceRepaint();
        addDockWidget(Qt::BottomDockWidgetArea, ui->dockWidget);
        break;
//    case 1:
//        ui->field_widget->scanMode();
//        ui->field_widget->square();
//        ui->field_widget->forceRepaint();
//        addDockWidget(Qt::BottomDockWidgetArea, ui->dockWidget);
//        break;
    case 1:
        ui->field_widget->scanMode();
        ui->field_widget->iko();
        ui->field_widget->forceRepaint();
        addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget);
        break;
    }
    clearGraphs();
    on_resetScale_button_clicked();
    repaint();
}
//------------------------------------------------------------------------------

void ViewWindow::addFrames(QList<SkirlFrame> frames)
{
    QTime t;
    t.start();

    if(frames.empty()) return;

    waitForPaint();

    bool bSetScale = false;
    if(frameset.empty())
    {
        bSetScale = true; // Первичное задание квадратур
        /*    r_channel = 0;
              t_channel = 0;*/
    }

    frameset.append(frames);

    truncateFrameset();

    // Найдем максимальную длину кадра  и максимальную дальность в новой пачке.
    double maxrange = getSizeLimits();
    ui->field_widget->setMaxFramesize(maxframesize); // Отправили предел размера
                                                     // в виджет
    ui->field_widget->setMaxRange(maxrange);         // Туда же - максимальное
                                                     // расстояние (м)

    if(bSetScale)
    {
        scale.newer = 1;
        scale.older = 1-frameset.size();
        scale.close = 0;
        scale.far   = maxframesize;

        ikoScale.x_mid = 0;
        ikoScale.y_mid = 0;
        ikoScale.size = maxrange;
    }

    qDebug() << "New frames: ikoScale.size ="<<ikoScale.size
             << ", max frame size =" << maxframesize;

    setSpinBoxRanges();
    // Трассировка сделана только в пределах текущего масштаба


//    ui->field_widget->refillIndices();
    ui->field_widget->rebuildIndices();

    // Обнуляем fieldImage, ибо нам его надо обновить.
    ui->field_widget->forceRepaint();
//    repaint();
    printf("%d ms for addFrames\n", t.elapsed());
    fflush(stdout);
}
//------------------------------------------------------------------------------

void ViewWindow::dropFrames()
{
    waitForPaint();
    for(int a=0; a<frameset.size(); a++)
        delete [] frameset[a].quads;
    frameset.clear();
    trace_frame = 0;
    traceRange = 0;
    t_channel = 0;
    r_channel = 0;
    ui->field_widget->refillIndices(); // Сбрасываем индексы
    ui->field_widget->forceRepaint();
    clearGraphs();
}

//------------------------------------------------------------------------------

void ViewWindow::truncateFrameset()
{
    if(bTimeKeep)
    {
        unsigned int limitTime = frameset.last().time - framesettime * 1e6;

        while(frameset.first().time < limitTime)
        {
            delete [] frameset.first().quads;
            frameset.removeFirst();
        }
    } // if(bTimeKeep)
    else
    {
        int numRemoved = frameset.size() - maxFrames;
        for(int a=0; a<numRemoved; a++)
        {
            delete [] frameset.first().quads;
            frameset.removeFirst();
        }
    }

}
//------------------------------------------------------------------------------

void ViewWindow::setSpinBoxRanges()
{
    bFreeSpinBoxes = false;
    ui->range_spinBox->setMinimum(scale.close);
    ui->range_spinBox->setMaximum(scale.far-1);
//    ui->time_spinBox->setMinimum(scale.older);
//    ui->time_spinBox->setMaximum(scale.newer - 1);
    ui->time_spinBox->setMaximum(1-scale.older);
    ui->time_spinBox->setMinimum(1-scale.newer);
    bFreeSpinBoxes = true;
}

//------------------------------------------------------------------------------

void ViewWindow::updateButtons()
{
    ViewTool t = ui->field_widget->getTool();
    ui->trace_toolButton->setChecked( (t == VT_TRACE) );
    ui->zoom_toolButton->setChecked( (t == VT_ZOOM) );
    ui->pan_toolButton->setChecked( (t == VT_PAN) );
}

//------------------------------------------------------------------------------

void ViewWindow::clearGraphs()
{
    int a;
    for(a=0; a<MAX_CURVES; a++)
        curves[a]->detach();
    ui->range_plot->replot();
    ui->time_plot->replot();
}

//------------------------------------------------------------------------------

void ViewWindow::makeRectGraphs()
{
    // Для начала запомним, как у нас стояли зумеры
    QStack<QRectF> range_stack = rangeZoomer->zoomStack();
    int rangeZoomIndex = rangeZoomer->zoomRectIndex();
    QStack<QRectF> time_stack = timeZoomer->zoomStack();
    int timeZoomIndex = timeZoomer->zoomRectIndex();

    clearGraphs();
    // Запомним, какие каналы мы смотрим
    int minchannel = 1 - frameset.size(); // Минимально возможный канал времени
    // для трассировки
    if((t_channel>0) ||(t_channel < minchannel )) return;

    // Формируем данные для графика "от дальности"
    int plotsize = scale.far - scale.close;
    double* X = new double [plotsize];     // Горизонтальная ось
    double* Yre  = new double [plotsize];  // Действительная часть
    double* Yim  = new double [plotsize];  // Мнимая часть
    double* Yabs = new double [plotsize];  // Модуль

    int lastframe = frameset.size() - 1;

    int a;
    for(a=0; a<plotsize; a++)
    {
        int range = scale.close + a;
        X[a] = range;
        SkirlQuad q;
        if(range >= frameset[lastframe + t_channel].size)
        {
            q.re = 0;
            q.im = 0;
        }
        else
            q = frameset[lastframe + t_channel].quads[range];
        Yre[a] = q.re;
        Yim[a] = q.im;
        Yabs[a] = sqrtf( 1.0*q.re*q.re + 1.0*q.im*q.im );
    }

    // Задаем данные для первого графика
#if (QWT_VERSION >> 16) < 6
    // До версии 6.0 данные кидались через setData
    curves[0]->setData(X, Yre,  plotsize);
    curves[1]->setData(X, Yim,  plotsize);
    curves[2]->setData(X, Yabs, plotsize);
#else
    // Начиная с версии 6.0.0 данные пошли тем же путем через setSamples
    curves[0]->setSamples(X, Yre,  plotsize);
    curves[1]->setSamples(X, Yim,  plotsize);
    curves[2]->setSamples(X, Yabs, plotsize);
#endif

    delete [] X;
    delete [] Yre;
    delete [] Yim;
    delete [] Yabs;

    // Формируем данные для графика "от угла/времени"
    plotsize = scale.newer - scale.older;
    X = new double [plotsize];     // Горизонтальная ось
    Yre  = new double [plotsize];  // Действительная часть
    Yim  = new double [plotsize];  // Мнимая часть
    Yabs = new double [plotsize];  // Модуль

    for(a=0; a<plotsize; a++)
    {
        int fr = scale.older + a;
        X[a] = fr;
        SkirlQuad q;
        q.re = 0;
        q.im = 0;

        if(lastframe + fr < frameset.size())
        {
            if(r_channel < frameset[lastframe + fr].size)
            {
                q = frameset[lastframe+fr].quads[r_channel];
            }
        }

        Yre[a] = q.re;
        Yim[a] = q.im;
        Yabs[a] = sqrtf( 1.0*q.re*q.re + 1.0*q.im*q.im );
    }

    // Задаем данные для второго графика
#if (QWT_VERSION >> 16) < 6
    // До версии 6.0 данные кидались через setData
    curves[3]->setData(X, Yre,  plotsize);
    curves[4]->setData(X, Yim,  plotsize);
    curves[5]->setData(X, Yabs, plotsize);
#else
    // Начиная с версии 6.0.0 данные пошли тем же путем через setSamples
    curves[3]->setSamples(X, Yre,  plotsize);
    curves[4]->setSamples(X, Yim,  plotsize);
    curves[5]->setSamples(X, Yabs, plotsize);
#endif

    delete [] X;
    delete [] Yre;
    delete [] Yim;
    delete [] Yabs;

    // Приклеивание кривых
    if(bCurves[0])
        curves[0]->attach(ui->range_plot);
    if(bCurves[1])
        curves[1]->attach(ui->range_plot);
    if(bCurves[2])
        curves[2]->attach(ui->range_plot);

    if(bCurves[3])
        curves[3]->attach(ui->time_plot);
    if(bCurves[4])
        curves[4]->attach(ui->time_plot);
    if(bCurves[5])
        curves[5]->attach(ui->time_plot);

    // Отрисовка маркеров.
    rangeMarker->setXValue(r_channel);
    timeMarker->setXValue(t_channel);

    // Уберем маркеры трассировки (не сильно актуальны при смене графиков)
    rangePickMarker->setVisible(false);
    timePickMarker->setVisible(false);

    // Уберем цифры в текстовых полях
    ui->trackerX_edit->setText("");
    ui->trackerY_edit->setText("");

    rangeZoomer->setZoomStack(range_stack, rangeZoomIndex);
    timeZoomer->setZoomStack(time_stack, timeZoomIndex);

    ui->range_plot->replot();
    ui->time_plot->replot();

}
//------------------------------------------------------------------------------
void ViewWindow::makeIkoGraphs()
{
    int frssize = frameset.size();
    if(frssize == 0) return;
    if(trace_frame > frssize)
        return;
    // Для начала запомним, как у нас стояли зумеры
    QStack<QRectF> range_stack = rangeZoomer->zoomStack();
    int rangeZoomIndex = rangeZoomer->zoomRectIndex();
    QStack<QRectF> time_stack = timeZoomer->zoomStack();
    int timeZoomIndex = timeZoomer->zoomRectIndex();

    clearGraphs();

//    printf("Plotting iko graphs for frame %d (frames from %d to %d)\n",
//           trace_frame, trace_minFrame, trace_maxFrame);

    // График "от дальности".
    // Кадр, который строим
    SkirlFrame fr = frameset.at(trace_frame);

    int maxPlotSize = fr.size;      // Максимальный размер графика.
    int plotsize = 0;               // Реальный размер графика (с учетом
                                    // попадания точек в окно масштаба).

    double* X = new double [maxPlotSize];     // Горизонтальная ось
    double* Yre  = new double [maxPlotSize];  // Действительная часть
    double* Yim  = new double [maxPlotSize];  // Мнимая часть
    double* Yabs = new double [maxPlotSize];  // Модуль

    // Найдем стартовое расстояние
    double baserange = 0;
    int beamMode = fr.flags & 0xF;
    if      (beamMode == SKI_FLAG)
        baserange = ikoMetrics.minRange_SKI;
    else if (beamMode == LFM_FLAG)
        baserange = ikoMetrics.minRange_LFM;
    else
    {
        // неизвестные тип сигнала - проигнорировать эти данные?
    }
    /*
  if((beamMode == SKI_TIME) || (beamMode == SKI_ANGLE))
    baserange = ikoMetrics.minRange_SKI;
  if((beamMode == LFM_TIME) || (beamMode == LFM_ANGLE))
    baserange = ikoMetrics.minRange_LFM;
  */
    // Забиваем данные
    int a;
    for(a=0; a<maxPlotSize; a++)
    {
        double range = baserange + a*ikoMetrics.rangeStep;
        SkirlQuad q = fr.quads[a];
        QPointF pt = ui->field_widget->getCoordsByGeo(range, fr.angle);
        if( (pt.x() > ikoScale.x_mid - ikoScale.size) &&
                (pt.x() < ikoScale.x_mid + ikoScale.size) &&
                (pt.y() > ikoScale.y_mid - ikoScale.size) &&
                (pt.y() < ikoScale.y_mid + ikoScale.size)    )
        {
            X[plotsize] = range;
            Yre[plotsize] = q.re;
            Yim[plotsize] = q.im;
            Yabs[plotsize] = sqrt(1.0*q.re*q.re + 1.0*q.im*q.im);
            plotsize ++;
        }
    }

    // Задаем данные для первого графика
#if (QWT_VERSION >> 16) < 6
    // До версии 6.0 данные кидались через setData
    curves[0]->setData(X, Yre,  plotsize);
    curves[1]->setData(X, Yim,  plotsize);
    curves[2]->setData(X, Yabs, plotsize);
#else
    // Начиная с версии 6.0.0 данные пошли тем же путем через setSamples
    curves[0]->setSamples(X, Yre,  plotsize);
    curves[1]->setSamples(X, Yim,  plotsize);
    curves[2]->setSamples(X, Yabs, plotsize);
#endif

    delete [] X;
    delete [] Yre;
    delete [] Yim;
    delete [] Yabs;

    // Формируем данные для графика от угла. Тут все хитро...
    maxPlotSize = trace_maxFrame - trace_minFrame;
    plotsize = 0;

    X = new double [maxPlotSize];     // Горизонтальная ось
    Yre  = new double [maxPlotSize];  // Действительная часть
    Yim  = new double [maxPlotSize];  // Мнимая часть
    Yabs = new double [maxPlotSize];  // Модуль

    for(int a=trace_minFrame; a<trace_maxFrame; a++)
    {
        fr = frameset.at(a);
        if(fr.angle == 0x7FFF)
            continue;                 // Пропуск кадров без угла
        // Базовое расстояние считать каждый раз...
        baserange = 0;
        int beamMode = fr.flags & 0xF;
        if      (beamMode == SKI_FLAG)
            baserange = ikoMetrics.minRange_SKI;
        else if (beamMode == LFM_FLAG)
            baserange = ikoMetrics.minRange_LFM;
        else
        {
            // неизвестные тип сигнала - проигнорировать эти данные?
        }
        /*
    if((beamMode == SKI_TIME) || (beamMode == SKI_ANGLE))
      baserange = ikoMetrics.minRange_SKI;
    if((beamMode == LFM_TIME) || (beamMode == LFM_ANGLE))
      baserange = ikoMetrics.minRange_LFM;
    */
        int r = floor((traceRange - baserange) / ikoMetrics.rangeStep );
        if((r>=0) && (r<fr.size))
        {
            // Считаем точку, вкидывается ли она в масштаб
            SkirlQuad q = fr.quads[r];
            QPointF pt = ui->field_widget->getCoordsByGeo(traceRange, fr.angle);
            // Проверка на попадание в окно масштаба
            if( (pt.x() > ikoScale.x_mid - ikoScale.size) &&
                    (pt.x() < ikoScale.x_mid + ikoScale.size) &&
                    (pt.y() > ikoScale.y_mid - ikoScale.size) &&
                    (pt.y() < ikoScale.y_mid + ikoScale.size)    )
            {
                X[plotsize] = fr.angle;
                Yre[plotsize] = q.re;
                Yim[plotsize] = q.im;
                Yabs[plotsize] = sqrt(1.0*q.re*q.re + 1.0*q.im*q.im);
                plotsize ++;
            }
        }
    }

    // Задаем данные для второго графика
#if (QWT_VERSION >> 16) < 6
    // До версии 6.0 данные кидались через setData
    curves[3]->setData(X, Yre,  plotsize);
    curves[4]->setData(X, Yim,  plotsize);
    curves[5]->setData(X, Yabs, plotsize);
#else
    // Начиная с версии 6.0.0 данные пошли тем же путем через setSamples
    curves[3]->setSamples(X, Yre,  plotsize);
    curves[4]->setSamples(X, Yim,  plotsize);
    curves[5]->setSamples(X, Yabs, plotsize);
#endif

    delete [] X;
    delete [] Yre;
    delete [] Yim;
    delete [] Yabs;

    // Приклеивание кривых
    if(bCurves[0])
        curves[0]->attach(ui->range_plot);
    if(bCurves[1])
        curves[1]->attach(ui->range_plot);
    if(bCurves[2])
        curves[2]->attach(ui->range_plot);

    if(bCurves[3])
        curves[3]->attach(ui->time_plot);
    if(bCurves[4])
        curves[4]->attach(ui->time_plot);
    if(bCurves[5])
        curves[5]->attach(ui->time_plot);

    // Отрисовка маркеров.
    rangeMarker->setXValue(traceRange);
    timeMarker->setXValue(frameset.at(trace_frame).angle);

    // Уберем маркеры трассировки (не сильно актуальны при смене графиков)
    rangePickMarker->setVisible(false);
    timePickMarker->setVisible(false);

    // Уберем цифры в текстовых полях
    ui->trackerX_edit->setText("");
    ui->trackerY_edit->setText("");

    rangeZoomer->setZoomStack(range_stack, rangeZoomIndex);
    timeZoomer->setZoomStack(time_stack, timeZoomIndex);

    ui->range_plot->replot();
    ui->time_plot->replot();
}
//------------------------------------------------------------------------------
void ViewWindow::updateGraphs()
{
    if(ui->field_widget->isTimeMode())
    {
        makeRectGraphs();
    }
    else
    {
        if(ui->field_widget->isIko())
            makeIkoGraphs();
    }
}
//------------------------------------------------------------------------------
void ViewWindow::setMaxFrames(int framenum)
{
    waitForPaint();
    maxFrames = framenum;
    truncateFrameset(); // Проверим, а не срезали ли нам число хранимых кадров
    ui->field_widget->refillIndices();
    ui->field_widget->forceRepaint();
}
//------------------------------------------------------------------------------
void ViewWindow::setCurvesVisibility(int cv)
{
    for(int a=0; a<MAX_CURVES; a++)
    {
        bCurves[a] = (bool) (cv & 1);
        cv = cv >> 1;
    }
}
//------------------------------------------------------------------------------
int ViewWindow::getCurvesVisibility()
{
    int ret;

    for(int a=MAX_CURVES-1; a>=0; a--)
    {
        ret |= (int)bCurves[a];
        ret = ret << 1;
    }
    return ret;
}
//------------------------------------------------------------------------------
int ViewWindow::getAmpLimit()
{
    return ui->field_widget->getAmpLimit();
}
//------------------------------------------------------------------------------
FieldDrawMethod ViewWindow::getGenMode()
{
    return ui->field_widget->getGenMode();
}
//------------------------------------------------------------------------------
void ViewWindow::setKeepMode(double value, bool bTime)
{
    bTimeKeep = bTime;
    if(bTime)
        framesettime = value;
    else
        maxFrames = value;
}
//------------------------------------------------------------------------------
void ViewWindow::setDebugMode(bool bDebug)
{
    ui->field_widget->setDebugMode(bDebug);
}
//------------------------------------------------------------------------------
void ViewWindow::setAmpLimit(int amp_limit)
{
    ui->field_widget->setAmpLimit(amp_limit);
}
//------------------------------------------------------------------------------
bool ViewWindow::setPaletteFile(QString F)
{
    return ui->field_widget->setPaletteFile(F);
}
//------------------------------------------------------------------------------
void ViewWindow::setGenMode(FieldDrawMethod i)
{
    bool bRefresh = false;
    if(ui->field_widget->getGenMode() != i)
        bRefresh = true;
    ui->field_widget->setGenMode(i);
    if(bRefresh)
        ui->field_widget->forceRepaint();
}
//------------------------------------------------------------------------------
void ViewWindow::setIkoMetrics(IKO_Metrics im)
{
    bool bRebuild = (im.A_max != ikoMetrics.A_max);
    bool bRerange = (im.rangeStep != ikoMetrics.rangeStep);
    bRerange |= (im.minRange_LFM != ikoMetrics.minRange_LFM);
    bRerange |= (im.minRange_SKI != ikoMetrics.minRange_SKI);
    ikoMetrics = im;
    if(bRebuild)
        ui->field_widget->rebuildIndices();
    else
        ui->field_widget->refillIndices();
    if(bRerange)
    {
        ui->field_widget->setMaxRange(getSizeLimits());
        ui->field_widget->forceRepaint();
    }
}
//------------------------------------------------------------------------------
void ViewWindow::setGrid(double gridR, double gridA)
{
    ui->field_widget->setGrid(gridR, gridA);
}
//------------------------------------------------------------------------------
void ViewWindow::setGridColor(QColor c)
{
    ui->field_widget->setGridColor(c);
}
//------------------------------------------------------------------------------
void ViewWindow::onSpinBoxesUpdated()
{
    if(bFreeSpinBoxes)
    {
        ui->field_widget->setTracePoint(ui->range_spinBox->value(),
                                        ui->time_spinBox->value());
        ui->field_widget->repaint();
    }
}

//------------------------------------------------------------------------------

void ViewWindow::on_resetScale_button_clicked()
{
    // Уравниваем графики обратно к содержимому.
    ui->range_plot->setAxisAutoScale(QwtPlot::xBottom);
    ui->range_plot->setAxisAutoScale(QwtPlot::yLeft);

    ui->time_plot->setAxisAutoScale(QwtPlot::xBottom);
    ui->time_plot->setAxisAutoScale(QwtPlot::yLeft);

    // Строим
    ui->range_plot->replot();
    ui->time_plot->replot();

    // Сбрасываем зумеры
    rangeZoomer->setZoomBase();
    timeZoomer->setZoomBase();
}

//------------------------------------------------------------------------------
void ViewWindow::printFrameInfo()
{
    QString n_a_text = QString::fromUtf8("н/д");
    QString tmp;
    if(frameset.isEmpty())
    {
        ui->range_spinBox->setValue(0);
        ui->time_spinBox->setValue(0);
        ui->realAngle_edit->setText(n_a_text);
        ui->realTime_edit->setText(n_a_text);
        ui->realRange_edit->setText(n_a_text);
        return;
    }

    SkirlFrame fr;
    bFreeSpinBoxes = false; // Запрещаем обработку изменения значений спинбоксов.

    if(ui->field_widget->isTimeMode()) // Прямоугольник
    {
        int lastframe = frameset.size() - 1;
        fr = frameset[lastframe + t_channel];

        // Базовое расстояние (пригодится)
        double baserange = 0;
        int beamMode = fr.flags & 0xF;
        if (beamMode == SKI_FLAG)
            baserange = ikoMetrics.minRange_SKI;
        else if (beamMode == LFM_FLAG)
            baserange = ikoMetrics.minRange_LFM;
        else
        {
            // неизвестный тип сигнала - проигнорировать эти данные?
        }

        ui->range_spinBox->setValue(r_channel);
//        ui->time_spinBox->setValue(t_channel);
        ui->time_spinBox->setValue(frameset.size() + t_channel);
//        qDebug() << "t_channel is " << t_channel;

        double range = baserange + r_channel*ikoMetrics.rangeStep;
        ui->realRange_edit->setText(QString::number(range));
    }
    else
    {
        if(ui->field_widget->isIko()) // ИКО
        {
            int lastframe = frameset.size() - 1;
            fr = frameset.at(trace_frame);

            // Базовое расстояние (пригодится)
            double baserange = 0;
            int beamMode = fr.flags & 0xF;
            if (beamMode == SKI_FLAG)
                baserange = ikoMetrics.minRange_SKI;
            else if (beamMode == LFM_FLAG)
                baserange = ikoMetrics.minRange_LFM;
            else
            {
                // неизвестный тип сигнала - проигнорировать эти данные?
            }

            int rangechannel = floor((traceRange-baserange) / ikoMetrics.rangeStep );
            ui->range_spinBox->setValue(rangechannel);
//            ui->time_spinBox->setValue(trace_frame - lastframe);
            ui->time_spinBox->setValue(trace_frame);

            ui->realRange_edit->setText(QString::number(traceRange));
        }
    }

    bFreeSpinBoxes = true;  // Разрешаем обратно.

    // Флаги в шестнадцатеричном виде
    ui->flags_edit->setText("0x"+QString::number(fr.flags,16));

    // Угол
    ui->realAngle_edit->setText(QString::number(fr.angle));

    // Время
    ui->realTime_edit->setText(
        QString::number(fr.time * 1e-5) + QString::fromUtf8(" мс (")
        + QString::number(fr.time) + ")"
                              );
}
//------------------------------------------------------------------------------

void ViewWindow::onPointPicked(int r_ch, int t_ch)
{
    qDebug() << "Point picked: " << r_ch << "x" << t_ch;
    r_channel = r_ch;
    t_channel = t_ch;
    if(t_channel > 0)
        t_channel = 0;
    printFrameInfo();
    makeRectGraphs();
}
//------------------------------------------------------------------------------
void ViewWindow::onIkoPointPicked(double range, int fr_trace,
                                  int fr_min, int fr_max)
{
    traceRange = range;
    trace_frame = fr_trace;
    trace_minFrame = fr_min;
    trace_maxFrame = fr_max;

    printFrameInfo();
    makeIkoGraphs();
}
//------------------------------------------------------------------------------
void ViewWindow::onScaleChanged()
{
    // Обновляем пределы спинбоксов
    setSpinBoxRanges();
    // Сбрасываем масштаб
    on_resetScale_button_clicked();
}
//------------------------------------------------------------------------------
void ViewWindow::on_zoom_toolButton_clicked()
{
    ui->field_widget->setTool(VT_ZOOM);
    updateButtons();
}
//------------------------------------------------------------------------------
void ViewWindow::on_trace_toolButton_clicked()
{
    ui->field_widget->setTool(VT_TRACE);
    updateButtons();
}
//------------------------------------------------------------------------------
void ViewWindow::on_pan_toolButton_clicked()
{
    ui->field_widget->setTool(VT_PAN);
    updateButtons();

}
//------------------------------------------------------------------------------
void ViewWindow::on_halfScale_button_clicked()
{
    if(ui->field_widget->isTimeMode())
    {
        int scale_w = scale.far   - scale.close;
        int scale_h = scale.newer - scale.older;

        // Бывает и такое
        if(scale_w < 2) scale_w = 2;
        if(scale_h < 2) scale_h = 2;

        scale.close -= scale_w / 2;
        scale.far   += scale_w / 2;
        scale.newer += scale_h / 2;
        scale.older -= scale_h / 2;
    }
    else
    {
        if(ui->field_widget->isIko())
        {
            ikoScale.size *= 2;
        }
    }
    ui->field_widget->forceRepaint(); // Тут проверяются масштабы
    ui->field_widget->clearZoomStack();
    // Очищаем зум-стек при изменении масштаба вручную.

    setSpinBoxRanges();
}
//------------------------------------------------------------------------------
void ViewWindow::on_doubleScale_button_clicked()
{
    if(ui->field_widget->isTimeMode())
    {
        int scale_w = scale.far   - scale.close;
        int scale_h = scale.newer - scale.older;

        scale_w /= 2;
        scale_h /= 2;

        if(scale_w == 0) scale_w = 1;
        if(scale_h == 0) scale_h = 1;

        scale.close += scale_w / 4;
        scale.far   -= scale_w / 4;
        scale.newer -= scale_h / 4;
        scale.older += scale_h / 4;
    }
    else
    {
        if(ui->field_widget->isIko())
        {
            ikoScale.size /= 2;
        }
    }
    ui->field_widget->forceRepaint();
    ui->field_widget->clearZoomStack();
    // Очищаем зум-стек при изменении масштаба вручную.

    setSpinBoxRanges();
}

//------------------------------------------------------------------------------

void ViewWindow::on_viewParams_button_clicked()
{
    emit view_params_requested();
}

//------------------------------------------------------------------------------

void ViewWindow::waitForPaint(bool bProcess)
{
    while(ui->field_widget->waitForPaintThread(10) == false)
    {
        // Тупо ждем
        if(bProcess)
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}
//------------------------------------------------------------------------------
void ViewWindow::on_gTrace_toolButton_clicked()
{
    flipMode();
}
//------------------------------------------------------------------------------
void ViewWindow::on_gZoom_toolButton_clicked()
{
    flipMode();
}
//------------------------------------------------------------------------------
void ViewWindow::flipMode()
{
    rangeZoomer->setEnabled(ui->gZoom_toolButton->isChecked());
    timeZoomer->setEnabled(ui->gZoom_toolButton->isChecked());
    rangePicker->setEnabled(ui->gTrace_toolButton->isChecked());
    timePicker->setEnabled(ui->gTrace_toolButton->isChecked());
    ui->trackerX_edit->setVisible(ui->gTrace_toolButton->isChecked());
    ui->trackerY_edit->setVisible(ui->gTrace_toolButton->isChecked());

    // Маркеры "включаются" только в соответствующем слоте, тут их можно
    // разве что выключить
    if( !(ui->gTrace_toolButton->isChecked()) )
    {
        rangePickMarker->setVisible(false);
        timePickMarker->setVisible(false);
        ui->trackerX_edit->setText("");
        ui->trackerY_edit->setText("");
    }

    ui->range_plot->replot();
    ui->time_plot->replot();
}
//------------------------------------------------------------------------------
void ViewWindow::onRangeGraphPointPicked(QPoint pt)
{
    double ranges[3] = {0, 0, 0};
    double idx[3] = {-1, -1, -1};

    // Измерили расстояния
    for(int a=0; a<3; a++)
    {
        if(bCurves[a])
            idx[a] = curves[a]->closestPoint(pt, &(ranges[a]));
    }

    // Поищем ближайшую кривую
    int curve_idx = -1;
    int minrange = 0xFFFFFF;
    for(int a=0; a<3; a++)
    {
        if((ranges[a] < minrange) && (idx[a] != -1))
        {
            minrange = ranges[a];
            curve_idx = a;
        }
    }
    if(curve_idx == -1) return;

    double X, Y;
#if (QWT_VERSION >> 16) < 6
    X = curves[curve_idx]->x(idx[curve_idx]);
    Y = curves[curve_idx]->y(idx[curve_idx]);
#else
    QPointF sample = curves[curve_idx]->sample(idx[curve_idx]);
    X = sample.x();
    Y = sample.y();
#endif

    rangePickMarker->setVisible(true);
    timePickMarker->setVisible(false);
    rangePickMarker->setXValue( X );
    rangePickMarker->setYValue( Y );

    ui->trackerX_edit->setText( QString::number(X) );
    ui->trackerY_edit->setText( QString::number(Y) );
    ui->range_plot->replot();
    ui->time_plot->replot();
}
//------------------------------------------------------------------------------
void ViewWindow::onTimeGraphPointPicked(QPoint pt)
{
    // Внимание! Кривые 4-5-6!!!
    double ranges[3] = {0, 0, 0};
    double idx[3] = {-1, -1, -1};

    // Измерили расстояния
    for(int a=0; a<3; a++)
    {
        if(bCurves[a+3])
            idx[a] = curves[a+3]->closestPoint(pt, &(ranges[a]));
    }

    // Поищем ближайшую кривую
    int curve_idx = -1;
    int minrange = 0xFFFFFF;
    for(int a=0; a<3; a++)
    {
        if((ranges[a] < minrange) && (idx[a] != -1))
        {
            minrange = ranges[a];
            curve_idx = a+3;
        }
    }
    if(curve_idx == -1) return;

    double X, Y;
#if (QWT_VERSION >> 16) < 6
    X = curves[curve_idx]->x(idx[curve_idx-3]);
    Y = curves[curve_idx]->y(idx[curve_idx-3]);
#else
    QPointF sample = curves[curve_idx]->sample(idx[curve_idx-3]);
    X = sample.x();
    Y = sample.y();
#endif

    timePickMarker->setVisible(true);
    rangePickMarker->setVisible(false);
    timePickMarker->setXValue( X );
    timePickMarker->setYValue( Y );

    ui->trackerX_edit->setText( QString::number(X) );
    ui->trackerY_edit->setText( QString::number(Y) );
    ui->range_plot->replot();
    ui->time_plot->replot();
}
//------------------------------------------------------------------------------
void ViewWindow::on_help_button_clicked()
{
    helpWindow->show();
    helpWindow->move(mapToGlobal(ui->help_button->pos()));
    helpWindow->activateWindow();
}
//------------------------------------------------------------------------------
double ViewWindow::getSizeLimits(int* mfs)
{
    double maxrange = 0;
    maxframesize = 0;
    for(int a=0; a<frameset.size(); a++)
    {
        int framesize = frameset[a].size;
        double range = 0;
        int beamMode = frameset[a].flags & 0xF;
        if      (beamMode == SKI_FLAG)
            range = ikoMetrics.minRange_SKI + framesize*ikoMetrics.rangeStep;
        else if (beamMode == LFM_FLAG)
            range = ikoMetrics.minRange_LFM + framesize*ikoMetrics.rangeStep;
        else
        {
            // неизвестные тип сигнала - проигнорировать эти данные?
        }
        /*
    if((beamMode == SKI_TIME) || (beamMode == SKI_ANGLE))
      range = ikoMetrics.minRange_SKI + framesize*ikoMetrics.rangeStep;
    if((beamMode == LFM_TIME) || (beamMode == LFM_ANGLE))
      range = ikoMetrics.minRange_LFM + framesize*ikoMetrics.rangeStep;
    */
        if(framesize > maxframesize) maxframesize = framesize;
        if(maxrange < range) maxrange = range;

    }
    if(mfs != 0)
        *mfs = maxframesize;
    return maxrange;
}
//------------------------------------------------------------------------------
bool ViewWindow::isFramesetLocked()
{
    return ui->field_widget->isFramesetLocked();
}
//------------------------------------------------------------------------------
void ViewWindow::on_clear_toolButton_clicked()
{
    dropFrames();
}
//------------------------------------------------------------------------------
void ViewWindow::on_openFile_action_triggered()
{
      // Проверим, не протухла ли запись о том, откуда мы читали.
  QFileInfo fi(dumpDir);
  if(!fi.exists())
    dumpDir = ".";

   QString path =
    QFileDialog::getOpenFileName(0,
            QString::fromUtf8("Сохранить"),
            dumpDir,
            QString::fromUtf8("Файл с данными (*.dmp *.txt);;Все файлы(*)")
                                );
  if(path.length() == 0) return;

  QFile F;
  F.setFileName(path);
  if( !F.open(QIODevice::ReadOnly)   ) return; //Открывается?

  dropFrames();

  fi.setFile(path);
  dumpDir = fi.path();

  QList<SkirlFrame> filelist;
  filelist.clear();

  while(true)
  {
//    printf("%d frames read\n", filelist.size());
    fflush(stdout);
    SkirlFrame fr;
    int res = readFrame(&F, &fr); // тут делается new!!!

    // Если один кадр побился, остальные все равно не соберем толком.
    if (res == -1)
      break;

    int SignalFlag = fr.flags & 0x0f;

    if      ((SignalFlag == OLD_SKI_TIME) ||
             (SignalFlag == OLD_SKI_ANGLE))
    {
      fr.flags &= 0xf0;
      fr.flags |= SKI_FLAG;
    }
    else if ((SignalFlag == OLD_LFM_TIME) ||
             (SignalFlag == OLD_LFM_ANGLE))
    {
      fr.flags &= 0xf0;
      fr.flags |= LFM_FLAG;
    }
    else
    {
      // неизвестные тип сигнала - проигнорировать эти данные?
    }

    filelist.append(fr);
  }
  F.close();
  qDebug() << "The file has ended";

  addFrames(filelist);
  filelist.clear();
  // Чистить квадратуры уже не надо, они там где нужно!!!

  QString log = QString::fromUtf8("Прочитан файл дампа данных: ");
  log.append(path);
  qDebug() << log;

//  dumpLog(log.toUtf8().data());
}
//------------------------------------------------------------------------------
int ViewWindow::readFrame(QFile* F, SkirlFrame* fr)
{
  qint64 num;
  int packsize=0;

  num = F->read((char*)&(fr->flags),        sizeof(fr->flags));
  if(num == 0) return -1; // Файл кончился или не прочитался
  packsize += num;

  num = F->read((char*)&(fr->size),         sizeof(fr->size));
  if(num == 0) return -1; // Файл кончился или не прочитался
  packsize += num;

  num = F->read((char*)&(fr->angle),        sizeof(fr->angle));
  if(num == 0) return -1; // Файл кончился или не прочитался
  packsize += num;

  num = F->read((char*)&(fr->reserved),     sizeof(fr->reserved));
  if(num == 0) return -1; // Файл кончился или не прочитался
  packsize += num;

  num = F->read((char*)&(fr->time),         sizeof(fr->time));
  if(num == 0) return -1; // Файл кончился или не прочитался
  packsize += num;

  int frsize = fr->size;
  // Проверка на целостность данных
  if((frsize < 0) || (frsize > MAX_QUADS) )
    return -1;
  // Пакеты с левыми углами остаются, просто потом не отображаются на ико
//  int A = fr->angle;
//  if( (A == 0x7FFF) || (A > 0xFFFF) || (A < 0) )
//    return -1;

  fr->quads = new SkirlQuad[frsize];

  num = F->read( (char*)fr->quads, sizeof(SkirlQuad)*frsize );
  if (num<sizeof(SkirlQuad)*frsize)
    return -1; // Неполный кадр
  packsize += num;

  return packsize;
}
//------------------------------------------------------------------------------
void ViewWindow::on_clear_action_triggered()
{
    dropFrames();
}
//------------------------------------------------------------------------------
void ViewWindow::on_thr_slider_valueChanged(int value)
{
    ui->field_widget->setThreshold((float)value);
    ui->thr_label->setText(QString::number(value));
}
