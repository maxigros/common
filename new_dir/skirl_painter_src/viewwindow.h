/// @file viewwindow.h
/// @brief Заголовочный файл окна графиков.
/// @author Каранкевич Антон Александрович. НИИ РЭТ МГТУ им. Баумана.
///
/// Файл описывает класс окна, отображающего графики, поле (градиент) и
/// сопутствующие им радости.
///
/// @see MainWindow, ViewWindow, QFieldView, QRichTreeWidget, QColorPicker

#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_picker_machine.h>

#include "qfieldview.h"

#define MAX_CURVES 6        ///< Число кривых. По 3 на график.

// Константы цветов графиков, значения по-умолчанию
// Красный - Re
#define GRAPH_RE_COLOR  0xFFFF0000

// Синий - Im
#define GRAPH_IM_COLOR  0xFF0000FF

// Черный - Abs
#define GRAPH_ABS_COLOR 0xFF000000

// Белый - цвет маркеров
#define MARKER_COLOR    0xFFFFFFFF

// Красный - цвет трассировщиков
#define TRACER_COLOR    0xFFFF0000

namespace Ui {
    class ViewWindow;
    class ViewWindow_Help;
}

/// @brief Класс окна, рисующего графики.
///
/// Класс окна с графиками. Окно обеспечивает отрисовку двух графиков QWT внутри
/// QDockWidget'а, отрисовку плоского градиента, обеспечивает хранение данных
/// внутри frameset.
///
/// При отображении qwt-графиков используются массивы размера MAX_CURVES. Они
/// соответствуют графикам в следующем порядке:
/// - Re(R)
/// - Im(R)
/// - Abs(R)
/// - Re(t)
/// - Im(t)
/// - Abs(t)
///
/// Основные графические элементы содержатся внутри ui. Для ознакомления с
/// используемыми классами следует смотреть файлы @ref qfieldview.h.
class ViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewWindow(QWidget *parent = 0);
    ~ViewWindow();
    QFieldView*   fieldWidget();    // КОСТЫЛЬ!

    /// @brief Добавление пачки кадров к общей пачке.
    ///
    /// Дописывает в конец frameset переданную пачку кадров, обрезая frameset
    /// по мере надобности.
    void addFrames(QList<SkirlFrame> frames ///< Новая пачка кадров.
                   );

    /// Удаляет все накопленные кадры.
    void dropFrames();

    /// @brief Построение qwt-графиков.
    ///
    /// Строит двухмерные qwt-графики с сечениями принятых данных по текущим
    /// каналам дальности и времени.
    void makeRectGraphs();

    /// @brief Построение qwt-графиков.
    ///
    /// Строит двухмерные qwt-графики с сечениями принятых данных по расстоянию
    /// и углу.
    void makeIkoGraphs();

    /// Обновление графиков. Используются последние запомненные номера каналов.
    void updateGraphs();

    /// Чтение настроек из системного конфига (параметры расположения окон).
    void readSettings();

    /// Запись настроек в системный конфиг (параметры расположения окон).
    void writeSettings();

    /// Возвращает, заблокирован ли frameset (идет отрисовка)
    bool isFramesetLocked();

    bool eventFilter(QObject *, QEvent *);

public slots:
    void on_mode_comboBox_currentIndexChanged(int index);

signals:
    /// Сигнал порождается когда запрашиваются параметры отображения.
    void view_params_requested();

protected:
//    void mouseReleaseEvent(QMouseEvent *);
    void closeEvent(QCloseEvent *);

private slots:
    void onSpinBoxesUpdated();
    void onPointPicked(int r_ch, int t_ch);
    void onScaleChanged();
    void onIkoPointPicked(double range, int fr_trace, int fr_min, int fr_max);
    void onRangeGraphPointPicked(QPoint pt);
    void onTimeGraphPointPicked(QPoint pt);
    void onHelpWindowClosed();

    void on_resetScale_button_clicked();
    void on_zoom_toolButton_clicked();
    void on_trace_toolButton_clicked();
    void on_halfScale_button_clicked();
    void on_doubleScale_button_clicked();
    void on_pan_toolButton_clicked();
    void on_viewParams_button_clicked();
    void on_gTrace_toolButton_clicked();
    void on_gZoom_toolButton_clicked();
    void on_help_button_clicked();
    void on_clear_toolButton_clicked();
    void on_openFile_action_triggered();

    void on_clear_action_triggered();
    void on_thr_slider_valueChanged(int value);

private:
    Ui::ViewWindow *ui;
    Ui::ViewWindow_Help *help_ui;
    QWidget*            helpWindow;     ///< Окно со справкой.
    bool bTimeKeep;                     ///< Хранение "по времени".
    bool bFreeSpinBoxes;                ///< Разрешается ли обработка изменения
                                        ///  значений спинбоксов. Полезно
                                        ///  запрещать ее на время задания
                                        ///  спинбоксам новых значений.
    ScaleParams scale;                  ///< Масштаб.
    IKO_ScaleParams ikoScale;           ///< Масштаб ИКО.
    IKO_Metrics ikoMetrics;             ///< Метрика ИКО.
    int r_channel;                      ///< Трассируемый канал по дальности.
    int t_channel;                      ///< Трассируемый канал по времени.
    double traceRange;                  ///< Трассируемое расстояние (ИКО)
    int trace_frame;                    ///< Номер трассируемого кадра
    int trace_minFrame;                 ///< Минимальный кадр при трассировке.
    int trace_maxFrame;                 ///< Максимальный кадр при трассировке.
    int maxFrames;                      ///< Число хранимых кадров.
    double framesettime;                ///< Время хранения кадров [c].
    int maxframesize;                   ///< Максимальный размер кадра.
    bool bCurves[MAX_CURVES];           ///< Флаги отображения графиков.

    QList<SkirlFrame> frameset;         ///< Набор кадров для отображения.
                                        ///  Последние кадры идут в конце,
                                        ///  укорачивание идет с начала.
    QwtPlotCurve *curves[MAX_CURVES];   ///< Кривые графиков.

    QString             dumpDir;        ///< Папка, в которой открывали дамп.

    QwtPlotZoomer*      rangeZoomer;    ///< Зумер графика "по расстоянию".
    QwtPlotPanner*      rangePanner;    ///< Двигальщик графика.
    QwtPlotGrid*        rangeGrid;      ///< Сетка.
    QwtPlotMarker*      rangeMarker;    ///< Маркер для графика "по расстоянию".
    QwtPlotPicker*      rangePicker;    ///< Тыкальщик в график "по расстоянию".
    QwtPlotMarker*      rangePickMarker;///< Маркер для выбранной точки.

    QwtPlotZoomer*      timeZoomer;     ///< Зумер графика "по времени".
    QwtPlotPanner*      timePanner;     ///< Двигальщик графика.
    QwtPlotGrid*        timeGrid;       ///< Сетка.
    QwtPlotMarker*      timeMarker;     ///< Маркер для графика "по времени".
    QwtPlotPicker*      timePicker;     ///< Тыкальщик в график "по времени".
    QwtPlotMarker*      timePickMarker; ///< Маркер для выбранной точки.

    void truncateFrameset();            ///< Укорачивание фреймсета под размер
                                        ///  maxFrames.
    void setSpinBoxRanges();            ///< Задает диапазон спинбоксов.
    void updateButtons();               ///< Обновляет tool-кнопки.
    void clearGraphs();                 ///< Очищает графики.

    /// @brief Ожидание окончания рисования в потоке рисовательного виджета.
    void waitForPaint(bool bProcess=true ///< Обрабатывать ли интерфейс в
                                         ///  процессе ожидания
                     );
    void printFrameInfo();              ///< Выводит информацию о трассиуемой
                                        ///  точке.

    /// @brief Переключение режима работы с графиками в зависимости от кнопок.
    void flipMode();

    /// @brief поиск пределов дальности и размера кадра.
    ///
    /// Поиск максимального расстояния и максимального размера кадра. Делается
    /// за один заход по набору кадров. Переменная @ref maxframesize заполняется
    /// автоматически
    ///
    /// @return Максимальная дальность
    double getSizeLimits(int* mfs = 0);

    /// @brief Чтение одной записи из файла данных.
    ///
    /// Считывает один кадр из файла с данными, заполняя соответствующие
    /// поля кадра.
    /// @return Возвращает количество успешно прочитанных байт либо -1, если
    /// произошла ошибка.
    int readFrame(QFile* F, SkirlFrame* fr);

public:
//GET
    /// Возвращает число отображаемых кадров.
    int getMaxFrames()
        { return maxFrames; }

    /// Возвращает предел амплитуды.
    int getAmpLimit();

    int getCurvesVisibility();
                            ///< Возвращает флаги видимости кривых одним
                            ///  bool-массивом.
    /// Возвращает цвет графика под номером graph_id.
    QColor getGraphColor(int graph_id)
        { return curves[graph_id]->pen().color(); }

    /// Возвращает цвет маркера.
    QColor getMarkerColor()
        { return rangeMarker->linePen().color(); }

    /// Возвращает цвет трассировщика.
    QColor getTracerColor()
        { return rangePickMarker->linePen().color(); }

    /// Возвращает тип генерации изображения.
    FieldDrawMethod getGenMode();


//SET
    /// @brief Задает предел хранения данных.
    ///
    /// Задает предел хранения данных. В зависимости от значения bTime, параметр
    /// value трактуется как число кадров (bTime = false) или как время хранения
    /// (bTime = true).
    void setKeepMode(double value,      ///< Параметр хранения.
                     bool bTime = false ///< Тип хранения. True - по времени.
                     );

    /// Переключает режим отладки.
    void setDebugMode(bool bDebug);

    /// Задает максимально хранимое число кадров.
    void setMaxFrames(int framenum);

    /// Задание предела амплитуды для рисовалки.
    void setAmpLimit(int amp_limit);

    /// Задание файла палитры для рисовалки
    bool setPaletteFile(QString F);

    /// @brief Задает видимость графиков.
    ///
    /// Задает видимость графиков одним интом, каждому графику соответствует
    /// 1 бит. Нулевой бит - график Re(R), первый - Im(R) и т.д.
    void setCurvesVisibility(int cv);

    /// Задает цвет графика под номером graph_id.
    void setGraphColor(int graph_id, QColor color)
        { curves[graph_id]->setPen(QPen(color)); }

    /// Задает цвет маркеров каналов на графиках
    void setMarkerColor(QColor color)
        { rangeMarker->setLinePen(QPen(color));
          timeMarker->setLinePen(QPen(color));  }

    /// Задает цвет трассировщиков.
    void setTracerColor(QColor color)
        { rangePickMarker->setLinePen(QPen(color));
          timePickMarker->setLinePen(QPen(color));  }

    /// Задает тип генерации изображения.
    void setGenMode(FieldDrawMethod i);

    /// Задает метрику ИКО.
    void setIkoMetrics(IKO_Metrics im);

    /// Задает параметры сетки ИКО.
    void setGrid(double gridR, double gridA);

    /// Задает цвет сетки ИКО
    void setGridColor(QColor c);


};

#endif // VIEWWINDOW_H
