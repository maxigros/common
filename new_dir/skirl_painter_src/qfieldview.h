/// @file qfieldview.h
/// @brief Заголовочный файл виджета отрисовки двухмерного градиента.
/// @author Каранкевич Антон Александрович. НИИ РЭТ МГТУ им. Баумана.
///
/// Файл содержит описание виджета отрисовки поля (двухмерного градиента) и
/// сопутствующих ему структур.
/// @see MainWindow, ViewWindow, QFieldView, QRichTreeWidget, QColorPicker

#ifndef QFIELDVIEW_H
#define QFIELDVIEW_H

#include <QWidget>
#include <QPainter>
#include <QTime>
#include <QFontMetrics>
#include <QList>
#include <QSpinBox>
#include <QMouseEvent>
#include <QPoint>
#include <QStack>
#include <QThread>
#include <QTimer>

#include <math.h>
#include <float.h>
#include <stdint.h>
#include <colorpalette.h>

#define MAX_QUADS       0xffffff    // 16 млн. точек на кадр должно хватить
#define TIMER_INTERVAL  10          // Интервал таймера, мс.
#define NUM_CIRCLES     10          // 10 обзоров копить - хватит?
#define NUM_LAYERS      10          // Максимальное число наложений данных
                                    // при построении ИКО.

// Флаги режимов работы (определяют метрику)
#define SKI_FLAG        0x4         // признак СКИ сигнала
#define LFM_FLAG        0x8         // признак ЛЧМ (сложного) сигнала
#define OLD_SKI_TIME    0x0         // Режим СКИ, эквидистантный по времени
#define OLD_SKI_ANGLE   0x2         // Режим СКИ, эквидистантный по углу
#define OLD_LFM_TIME    0x1         // Режим ЛЧМ, эквидистантный по времени
#define OLD_LFM_ANGLE   0x3         // Режим ЛЧМ, эквидистантный по углу

// DEFAULT VALUES:
#define AMP_LIMIT   46340       // 32767 * sqrt(2)
#define BORDER      20          // Размер границы по умолчанию.
#define MINIMAP     50          // Размер "мини-карты" по умолчанию.
#define PIESIZE     30          // Размер рисунка-миниатюры полного кадра
#define MAX_ANGLE   0xFFFF      // Максимальный угол
#define GRID_COLOR  0xFFFF8C00  // Цвет сетки - оранжевый
#define GRID_ALPHA  255         // Альфа-канал сетки

//------------------------------------------------------------------------------

///// @brief Структура, содержащая заголовок .bmp-файла.
/////
///// Стркутура содержит описание заголовка bmp-файла.
///// @warning Из-за выравнивания структура становится 16 байт вместо 14, и файл
/////          рассыпается. Структура лежит тут для общего вида, запись идет через
/////          переменную типа char[14].
//typedef struct
//{
//   char          bfType[2];     ///< Тип файла. Должно хранить "BM".
//   unsigned int  bfSize;        ///< Размер файла в байтах.
//   unsigned int  reserved;      ///< Зарезервированное поле.
//   unsigned int  bfOffBits;     ///< Смещение в байтах до начала данных.
//}tagBITMAPFILEHEADER;

///// Структура заголовка, содержащего информацию о BMP-файле
//typedef struct
//{
//   unsigned int  biSize;            ///< Размер структуры-заголовка.
//   int           biWidth;           ///< Ширина изображения в пикселях.
//   int           biHeight;          ///< Высота изображения в пикселях.
//   short int     biPlanes;          ///< Число плоскостей. Всегда 1.
//   short int     biBitCount;        ///< Битность изображения.
//   unsigned int  biCompression;     ///< Сжатие. Не используется.
//   unsigned int  biSizeImage;       ///< Параметр привязки к реальному размеру.
//   int           biXPelsPerMeter;   ///< Параметр привязки к реальному размеру.
//   int           biYPelsPerMeter;   ///< Параметр привязки к реальному размеру.
//   unsigned int  biClrUsed;         ///< Не используется.
//   unsigned int  biClrImportant;    ///< Не используется.
//}tagBITMAPINFOHEADER;

/// Структура, хранящая цвет в BMP-изображении.
typedef struct
{
  unsigned char rgbBlue;        ///< Синий цвет.
  unsigned char rgbGreen;       ///< Зеленый цвет.
  unsigned char rgbRed;         ///< Красный цвет.
  unsigned char rgbAlpha;       ///< Альфа-канал.
} RgbQuad;

/// Структура, хранящая отображаемые квадратуры.
typedef struct
{
  int16_t re;     ///< Действительная часть.
  int16_t im;     ///< Мнимая часть.
} SkirlQuad;

/// @brief Структура, хранящая данные в формате АЦП СКИРЛ.
///
/// Структура хранит один кадр АЦП СКИРЛ.
/// @attention Угловые единицы измеряются от 0 до 65535 на полный оборот
///            антенны, привязки к реальному азимуту нет.
typedef struct
{
  uint32_t      flags;          ///< Флаги.
  uint32_t      size;           ///< Размер данных (в отсчетах).
  uint16_t      angle;          ///< Угол на момент снятия кадра [Угл. Ед.].
  uint16_t      reserved;       ///< Зарезервированное поле.
  uint32_t      time;           ///< Время на момент снятия кадра, [мкс].
  // Здесь заканчивается реальный заголовок
  SkirlQuad*        quads;      ///< Непосредственно данные.
} SkirlFrame;

/// @brief Структура с параметрами масштаба.
/// Отображаются каналы в интервале
/// [close, far) по дальности и [older, newer) по времени.
typedef struct
{
  int close;    ///< Минимально отображаемый канал по дальности.
  int far;      ///< Максимально отображаемый канал по дальности.
  int newer;    ///< Наиболее "новый" канал времени.
  int older;    ///< Наиболее "старый" канал времени.
} ScaleParams;

/// @brief Структура с параметрами масштаба для ИКО. Описывает квадрат через
///        угол и сторону.
typedef struct
{
  double x_mid;     /// Середени масштаба по Х [м].
  double y_mid;     /// Середина масштаба по Y [м].
  double size;      /// Минимально отображаемое расстояние (радиус) [м].
} IKO_ScaleParams;

/// Структура с параметрами метрики ИКО - привязки к реальным расстояниям/углам.
typedef struct
{
  int       A_max;          ///< Предел угловых измерений [угл. отсч.].
  bool      A_inverse;      ///< Инвертированное ли направление азимута.
  double    A_offset;       ///< Смещение азимута в градусах.
  double    minRange_SKI;   ///< Стартовое расстояние для СКИ [м].
  double    minRange_LFM;   ///< Стартовое расстояние для ЛЧМ [м].
  double    rangeStep;      ///< Шаг по расстоянию [м].
  int       angleSKO;       ///< Ширина ДН антенны [угл. отсч.].
} IKO_Metrics;

/// Параметры генерации изображения.
enum FieldDrawMethod
{
  FDM_QUICK   = 0,  ///< Быстрая генерация изображения (по ближайшему отсчету).
  FDM_GETMAX  = 1,  ///< Смотрим, сколько отсчетов накрывает пиксель (минимум
                    ///  один) и выбираем максимальный.
  FDM_AVERAGE = 2   ///< Усреднение всех накрытых отсчетов.
};

/// Используемый инструмент на поле (градиенте).
enum ViewTool
{
  VT_TRACE,     ///< Трассировка графика.
  VT_ZOOM,      ///< Масштабирование графика.
  VT_PAN        ///< Перетаскивание графика.
};

/// Показывает, что в данный момент перетаскивается по виджету
enum DragState
{
  DS_NONE,          ///< Ничего не тащим.
  DS_ZOOM_FRAME,    ///< Тащим окно зума.
  DS_MINIMAP,       ///< Тащим окошко в миникарте.
  DS_PAN            ///< Тащим само поле.
};

/// Одна структура для хранения/передачи в поток всего сразу.
typedef struct
{
  QList<SkirlFrame>* frameset;      ///< Сами данные.
  int*               maxFrameSize;  ///< Максимальный размер кадра.
  bool*              bIko;          ///< Рисуем ли мы ИКО
  bool*              bTimeMode;     ///< Режим "по времени".
  ScaleParams*       scale;         ///< Параметры масштаба.
  IKO_ScaleParams*   ikoScale;      ///< Параметры масштаба для ИКО.
  IKO_Metrics*       ikoMetrics;    ///< Метрика ИКО.
  FieldDrawMethod*   fdm;           ///< Метод рисования.
  int*               amp_limit;     ///< Предел амплитуды.
  int*               fullViewIndex; ///< Номер обзора.
  double*            maxRange;      ///< Максимальное расстояние.
  int*               indices;       ///< Индексный массив
  Palette*           palette;       ///< Палитра амплитуды
  float              threshold;     ///< Порог отображения
} DrawDataPack;

//------------------------------------------------------------------------------
//                          QFieldView_PaintThread
//------------------------------------------------------------------------------

/// @brief Класс для создания потока рисования картинки.
///
/// Класс создает в памяти сырые данные для формирования изображения. Генерация
/// происходит в отдельном потоке. Процедура @ref fillImage заполняет
/// предоставленное изображение (QImage) сформированными данными.
/// Следует проверять, закончил ли поток выполнение, перед заполнением
/// изображения.
class QFieldView_PaintThread: public QThread
{
public:
  QFieldView_PaintThread();
  ~QFieldView_PaintThread();
  void run();           ///< Поток.
  void fillImage(QImage& img);   ///< Возвращает сгенерированное изображение.
private:
  // Переменные начинаются на t_ (thread), чтобы в запарке не перепутать
  bool t_bIko;                  ///< Рисуем ли мы ИКО.
  bool t_bTimeMode;             ///< Рисуем ли по времени.
  QRect t_bound;                ///< Прямоугольник, в который вписывается
                                ///  изображение.
  FieldDrawMethod t_fdm;        ///< Метод рисования изображения.
  QList<SkirlFrame>* t_frameset;///< Адрес структуры с квадратурами.
  ScaleParams t_scale;          ///< Хранит масштаб на время построения
                                ///  изображения.
  IKO_ScaleParams t_ikoScale;   ///< Масштаб для ИКО.
  IKO_Metrics t_ikoMetrics;     ///< Метрика ИКО.
  int* t_indices;               ///< Массив индексов.
  int t_amp_limit;              ///< Предел амплитуды (нормировка).
  Palette* t_palette;            ///< Палитра амплитуды
  int t_maxframesize;           ///< Максимальный размер кадра.
  int t_fullViewIndex;          ///< Индекс полного обзора.
  double t_maxRange;            ///< Предел расстояния.
  QImage t_img;                 ///< Изображение, по которому рисует поток
  unsigned int RawSize;         ///< Размер сырых данных.
  float t_threshold;            ///< Порог, ниже него ничего не рисуем

  int elapsedTime;              ///< Время, потраченное на генерацию, мс.

  void makeSquareImage(int W, int H);
  void makeIKO(int W, int H);
  RgbQuad getColorByAmp(float amp);

public:
//SET
  /// Задает прямоугольник, в который будет вписано изображение.
  void setBound(QRect b) { t_bound = b; }
  /// Отправляет в поток данные отрисовки.
  void setDrawData(DrawDataPack ddp);
//GET
  /// Возвращает параметры масштаба, для которых строилось изображение.
  ScaleParams* getScale() { return &t_scale; }
  /// Возвращает параметры масштаба ИКО, для которых строилось изображение.
  IKO_ScaleParams* getIkoScale() { return &t_ikoScale; }
  /// Возвращает измеренное время работы потока.
  int getElapsedTime() { return elapsedTime; }
};

//------------------------------------------------------------------------------
//                                QFieldView
//------------------------------------------------------------------------------

/// @brief Виджет, рисующий двухмерный градиент.
///
/// Виджет отображает двухмерный градиент. Данными для отрисовки являются:
/// - QList<SkirlFrame>* frameset - набор кадров для отображения.
/// - ScaleParams* scale - параметры масштаба. Описывает отрисовываемую область.
/// - r_channel и t_channel - каналы дальности и времени соответственно,
///   которые трассируются в данный момент (отображаются белыми линиями).
///   Канал дальности изменяется от 0 до максимального размера кадра - 1
///   включительно, канал времени изменяется от -frameset->size()+1 до 0
///   включительно (нулевой канал соответствует последнему кадру).
///
/// Виджет формирует изображение градиента в отдельном потоке в переменной
/// fieldImage типа QImage и отображает его на экране, растягивая до
/// необходимого размера. В случае, если реальные размеры fieldImage не
/// соответствуют отображаемым, в фоновом режиме запускается поток, обновляющий
/// изображение (генерирует его в памяти на основе переданных потоку
/// параметров и данных). По окончании работы потока обновленное изображение
/// отдается обратно в виджет. Мини-карта, линии трассировки и "резиновая рамка"
/// масштабирования рисуются стандартными средствами.
class QFieldView : public QWidget
{
    Q_OBJECT
public:
    explicit QFieldView(QWidget *parent = 0);
    ~QFieldView();

    /// Переводит отображение в режим ИКО.
    void iko() { bIko = true; }
    /// Переводит отображение в режим "прямоугольник".
    void square() { bIko = false;  }
    /// Перевод в режим сканирования по времени.
    void timeMode() { bTimeMode = true; }
    /// Перевод в режим кругового сканирования.
    void scanMode() { bTimeMode = false; }

    /// Задает извне трассируемую точку.
    void setTracePoint(int rangeChannel,        ///< Канал дальности.
                       int timeChannel          ///< Номер кадра "с конца",
                                                ///  изменяется от -беск. до 0.
                       );

    /// Задает извне трассируемую точку.
    void setTracePoint(double range,            ///< Дальность.
                       int angle                ///< Угол в текущем кадре.
                       );

    /// Заставляет перепроверить все границы и полностью перерисовать виджет.
    void forceRepaint();

    /// Очищает стек зумов.
    void clearZoomStack();

    /// Идет ли в данный момент работа потока по перерисовке изображения. В
    /// процессе такой работы нельзя менять frameset и удалять FieldView.
    bool isFramesetLocked();

    /// Заставляет сидеть и ждать msec миллисекунд до окончания либо потока,
    /// либо заданного времени.
    /// @return
    /// - true, если завершился поток
    /// - false, если наступил таймаут
    bool waitForPaintThread(int msec);

    /// Пересоздает массив индексов и перезаполняет его (вызывая
    /// @ref refillIndices). Использовать только при изменении ikoMetrics.A_max
    void rebuildIndices();

    /// Перезаполняет массив индексов без пересоздания массива.
    void refillIndices();

    // Тут часть вынесена в public, ибо полезна не только тут.

    /// @brief Перевод координат дальность-азимут в координаты X-Y.
    ///
    /// Переводит координаты точки из геодезических (дальность-азимут) в
    /// декартовы. Углы - в угловых единицах СКИРЛ, расстояния в метрах.
    QPointF getCoordsByGeo(QPointF pt);

    /// @brief Перевод координат дальность-азимут в координаты X-Y.
    ///
    /// Переводит координаты точки из геодезических (дальность-азимут) в
    /// декартовы. Углы - в угловых единицах СКИРЛ, расстояния в метрах.
    QPointF getCoordsByGeo(double R, double A);

signals:
    /// Выдается при выборе трассируемой точки в координатах "в каналах".
    void pointPicked(int r_ch, int t_ch);
    /// Выдается при выборе трассируемой точки в режиме ИКО. Отдается
    /// трассируемое расстояние, номер трассируемого кадра и пределы номеров
    /// кадров для построения графиков.
    void pointPicked(double range, int tr_frame, int fr_min, int fr_max);
    /// Выдается при изменении масштаба. Сигнал окну об изменении пределов
    /// спинбоксов и масштабов графиков.
    void scaleChanged();
    /// Сигнал порождается когда заканчивается рендеринг
    void paintFinished();

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);
    void wheelEvent(QWheelEvent *e);


public slots:

private:
    bool bDebugMode;    ///< Режим отладки.
    FieldDrawMethod fdm;///< Способ генерации изображения.
    bool bRedrawFlag;   ///< Была затребована перерисовка.
    bool bIko;          ///< Индикатор кругового обзора.
    bool bTimeMode;     ///< Режим отображения по времени. Только квадрат.
                        ///  Проверяется всегда в первую очередь.
    bool bCheckResize;  ///< Обрабатывать ли resizeEvent.
    int border;         ///< Размер поля (отступа), [пикс].
    int minimap;        ///< Размер мини-карты, [пикс].
    int piesize;        ///< Размер "пирога" - рисунка-миниатюры полного кадра.
    int maxframesize;   ///< Максимальный размер кадра.
    double maxRange;    ///< Максимальное расстояние в ИКО.
    int r_channel;      ///< Трассируемый канал по расстоянию.
    int t_channel;      ///< Трассируемый канал по времени (углу), от 0
                        ///  (последний кадр) до -беск.
    double traceRange;  ///< Трассируемое расстояние.
    int traceAngle;     ///< Трассируемый угол.
    int amp_limit;      ///< Предел амплитуды для отрисовки.
    Palette palette;    ///< Палитра с цветами под амплитуды
    int fullViewIndex;  ///< Индекс номера полного обзора.
    double gridStep_r;  ///< Шаг по сетке, дальность [м].
    double gridStep_a;  ///< Шаг по сетке, азимут [град.].
    DragState dragState;///< Состояние перетаскивания в данный момент.
    QPoint dragStart;   ///< Точка, с которой начали тянуть зум.
    QPoint dragEnd;     ///< Точка, до которой растянуто окно зума.
    QPoint panCenter;   ///< Координаты отсчета, за который мы начали тащить
                        ///  поле.
    QList<SkirlFrame>*  frameset;
                        ///< QList с отображаемыми кадрами.
    ScaleParams* scale; ///< Масштаб (хранит число отображаемых кадров).
    IKO_ScaleParams* ikoScale;
                        ///< Масштаб для ИКО (в метрах).
    IKO_Metrics *ikoMetrics;
                        ///< Метрика ико.
    ViewTool tool;      ///< Режим работы мышки.
    QImage fieldImage;  ///< Изображение с градиентом.
    ScaleParams imageScale;
                        ///< Масштаб, которому соответствует изображение.
    IKO_ScaleParams imageIkoScale;
                        ///< Масштаб, по которому построен ИКО.
    QStack<ScaleParams> scaleStack;
                        ///< Стек масштабов для прямоугольника.
    QStack<IKO_ScaleParams> ikoScaleStack;
                        ///< Стек масштабов для ИКО.
    DrawDataPack pack;  ///< Структура с данными, для общения с потоком.
    QFieldView_PaintThread pt;
                        ///< Поток, рисующий картинку.
    int*        indices;///< Массив индексов строк, устанавливает соответствие
                        ///  между угловыми координатами (номер эл-та массива)
                        ///  и номерами строк в @ref frameset, соответствующими
                        ///  данной угловой координате. Размером NUM_CIRCLES
                        ///  * ikoMetrics->A_max, индексируются начиная с
                        ///  последнего кадра.
                        ///  Массив трехмерный с одномерной адресацией.
    bool bInverseFrames;///< Флаг того, что кадры идут наоборот. Нужен в
                        ///  отрисовке "пирогов".
    int* frameLimits;   ///< Массив индексов строк с границами полных кадров.
                        ///  Содержит 2*NUM_CIRCLES элементов. Если лежит
                        ///  -1 хоть где-нибудь, то полного кадра нет вообще.
                        ///  @warning В элементе [0] лежит ПОСЛЕДНИЙ угол,
                        ///           а в элементе [1] - ПЕРВЫЙ!.
    QColor gridColor;   ///< Цвет сетки ИКО (с учетом альфа-канала).
    float threshold;    ///< Порог отображения


    /// Создание стандартной палитры (256 оттенков зеленого)
    void setDefaultPalette();

    /// Проверка на попадание мышкой в поле.
    bool bHitField(int x, int y);

    /// Проверка на попадание мышкой в минимап.
    bool bHitMap(int x, int y);

    /// Проверка на попадание в набор "пирогов" - миниатюр обзоров.
    bool bHitPie(int x, int y);

    /// Трассировка координаты с заполнением qwt-графиков.
    void tracePoint(int x, int y);

    /// Проверка значений номеров каналов на границу окна отображения (scale).
    void checkChannelBounds();

    /// Проверка окна отображения на предмет выхода за границы отображаемой
    /// области.
    void checkScaleBounds();

    /// Заставляет поток заново отрисовать изображение.
    void invalidateImage();

    /// Начинает "тащить" окно зума
    void startZoomDrag(int x, int y);

    /// Продолжает "тащить" окно зума, меняя его по ходу.
    void keepZoomDrag(int x, int y);

    /// Заканчивает "тащить" окно зума.
    void endZoomDrag();

    /// Начинает тащить поле.
    void startPanDrag(int x, int y);

    /// Продолжает тащить поле.
    void keepPanDrag(int x, int y);

    /// Перетаскивает окно отображения в новую позицию. На вход берет точку,
    /// нажатую внутри мини-карты.
    void moveScale(int x, int y);

    /// Изменяет номер отображаемого полного обзора в зависимости от того, куда
    /// было нажато в "пироге".
    void changePieIndex(int x, int y);


    /// @brief Переводит координаты на экране в номера каналов дальности.
    ///
    /// Возвращает координаты "дальность-номер_кадра" по пикселю,
    /// x() - номер канала дальности, y() - номер кадра времени/угла.
    /// Работает как по карте, так и по полю в зависимости от значения параметра
    /// bMap
    /// @return QPoint::x() - канал дальности, QPoint::y() - канал времени.
    QPoint getDataPointByPixel (QPoint pixel, bool bMap = false);
    /// @brief переводит координаты на экране в координаты в данных.
    ///
    /// Перегруженная версия функции getDataPointByPixel.
    QPoint getDataPointByPixel (int x, int y, bool bMap = false);

    /// @brief Переводит номера каналов дальности в координаты на экране.
    ///
    /// Возвращает координаты пикселя по координатам "канал дальности - канал
    /// времени".
    QPoint getPixelByDataPoint(QPoint dataPoint);
    /// @brief переводит координаты на экране в координаты в данных.
    ///
    /// Перегруженная версия функции getPixelByDataPoint.
    QPoint getPixelByDataPoint(int r_ch, int t_ch);

    /// @brief Переводит координаты на экране в координаты x-y в ИКО
    ///
    /// Возвращает координаты в метрах по точке на экране.
    QPointF getCoordsByPixel(int x, int y, bool bMap = false);
    /// @brief Переводит координаты на экране в координаты x-y в ИКО
    ///
    /// Возвращает координаты в метрах по точке на экране.
    QPointF getCoordsByPixel(QPoint p, bool bMap = false);
    /// @brief Переводит координаты на экране в координаты x-y в ИКО
    ///
    /// Возвращает координаты в метрах по точке на экране.
    QPointF getCoordsByPixel(double x, double y, bool bMap = false);
    /// @brief Переводит координаты на экране в координаты x-y в ИКО
    ///
    /// Возвращает координаты в метрах по точке на экране.
    QPointF getCoordsByPixel(QPointF p, bool bMap = false);

    /// @brief Переводит координаты ИКО в точку на экране
    ///
    /// Возвращает точку, в которую попадают переданные координаты в ИКО.
    QPoint getPixelByCoords(QPointF p);

    /// @brief Переводит координаты ИКО в точку на экране
    ///
    /// Возвращает точку, в которую попадают переданные координаты в ИКО.
    QPoint getPixelByCoords(double x, double y);

    /// @brief Переводит координаты ИКО в точку на экране (вещественный вариант)
    ///
    /// Возвращает точку, в которую попадают переданные координаты в ИКО.
    QPointF getFloatPixelByCoords(QPointF p);

    /// @brief Переводит координаты ИКО в точку на экране (вещественный вариант)
    ///
    /// Возвращает точку, в которую попадают переданные координаты в ИКО.
    QPointF getFloatPixelByCoords(double x, double y);

    /// @brief Перевод координат X-Y в дальность-азимут.
    ///
    /// Переводит декартовы координаты (X-Y) в геодезические координаты.
    /// Углы - в угловых единицах СКИРЛ, расстояния в метрах.
    QPointF getGeoByCoords(QPointF pt);

    /// @brief Перевод координат X-Y в дальность-азимут.
    ///
    /// Переводит декартовы координаты (X-Y) в геодезические координаты.
    /// Углы - в угловых единицах СКИРЛ, расстояния в метрах.
    QPointF getGeoByCoords(double X, double Y);

    /// @brief Определяет прямоугольник, в который надо вписать изображение.
    ///
    /// Возвращает прямоугольник, в который должно быть вписано изображение
    /// при текущих параметрах масштаба.
    QRect getImageBound(ScaleParams imageScale);

    /// @brief Определяет прямоугольник, в который надо вписать изображение.
    ///
    /// Возвращает прямоугольник, в который должно быть вписано изображение
    /// при текущих параметрах масштаба.
    QRect getImageBound(IKO_ScaleParams imageScale);

private slots:
    void onThreadFinished();    ///< Обработка таймера опроса потока.

public:
// SET
    /// Переключает режим отладки.
    void setDebugMode(bool debug) { bDebugMode = debug; }

    /// Задание режима сканирования.
    void setTimeMode(bool tMode) { bTimeMode = tMode; }

    /// Переключение режима отображения.
    void setIko(bool iko) { bIko = iko; invalidateImage(); }

    /// Передача набора кадров рисовалке.
    void setFrameset(QList<SkirlFrame>* frames) { frameset = frames;
                                                  pack.frameset = frames; }
    /// Задание масштаба для прямоугольного режима.
    void setScaleParams(ScaleParams* s) { scale = s;
                                          pack.scale = s; }

    /// Задает масштаб отображения ИКО
    void setIkoScale(IKO_ScaleParams *is)
        { ikoScale = is; pack.ikoScale = is; }

    /// Задает максимальный размер кадра.
    /// Число кадров можно узнать из frameset->size();
    void setMaxFramesize(int mfs) { maxframesize = mfs; }

    /// Задает максимальное расстояние в данных ИКО.
    void setMaxRange(double mr) { maxRange = mr; }

    /// Задает "инструмент", то есть что делает мышка при использовании на поле.
    void setTool(ViewTool t) { tool = t; }

    /// Задает предел амплитуды для отрисовки.
    void setAmpLimit(int al) { amp_limit = al; }

    /// Задает файл палитры
    bool setPaletteFile(QString f);

    /// Задает тип генерации изображения.
    void setGenMode(FieldDrawMethod i) { fdm = i; }

    /// Задает, обрабатывать ли resizeEvent.
    void setCheckResize(bool s) { bCheckResize = s; }

    /// Задает метрику ИКО
    void setIkoMetrics(IKO_Metrics* im)
        { ikoMetrics = im;
          pack.ikoMetrics = im; }

    /// Задает параметры сетки
    void setGrid(double gridR, double gridA)
        { gridStep_r = gridR;
          gridStep_a = gridA; }

    /// Задает цвет сетки (вместе с альфа-каналом)
    void setGridColor(QColor gc)
        { gridColor = gc; }

    /// Задает порог
    void setThreshold(float thr);


// GET
    /// Возвращает, находимся ли мы в режиме сканирования
    bool isTimeMode() { return bTimeMode; }

    /// Возвращает, рисуем ли мы ИКО
    bool isIko() { return bIko; }


    /// Возвращает текущий "инструмент".
    ViewTool getTool() { return tool; }

    /// Возвращает предел амплитуды.
    int getAmpLimit() { return amp_limit; }

    /// Возвращает тип генерации изображения.
    FieldDrawMethod getGenMode() { return fdm; }

    /// Возвращает, обрабатывает ли виджет resizeEvent.
    bool getCheckResize() { return bCheckResize; }
};

#endif // QFIELDVIEW_H
