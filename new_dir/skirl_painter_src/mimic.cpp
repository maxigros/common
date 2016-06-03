/// @file mimic.cpp
/// @brief Исполнительный файл библиотеки Mimic.
/// @author Каранкевич Антон Александрович. НИИ РЭТ МГТУ им. Баумана.
///
/// Библиотека Mimic представляет собой гибкий тип данных с динамической
/// типизацией. Данный тип может хранить, принимать и представлять значения
/// следующих типов языка С++:
/// - boolean
/// - int
/// - long int
/// - float
/// - double
/// - complex double
/// - char[]
/// - void*
/// .
/// Хранимые данные могут быть организованы в виде массива произвольного
/// размера. Возможно изменение размера массива хранимых данных и их типа
/// в процессе работы.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

#include "mimic.h"

#define CONVERT_P2V(VAR1, TYPE1, VAR2, TYPE2) VAR2 = (TYPE2) ( * ( (TYPE1 * ) VAR1 ) )
///< Конвертирование из указателя в переменную (на отдачу). Преобразование 1->2.
///

#define CONVERT_V2P(VAR1, TYPE1, VAR2, TYPE2) * ( (TYPE2*) VAR2 ) =  (TYPE2)VAR1;
///< Конвертирование из переменной в указатель (на получение).
/// Преобразование 1->2.


void Mimic::flushParams()
{
  type = DT_NOPE;
  numVars = 0;
  high = DBL_MAX;
  low = -DBL_MAX;
  dataptr = 0;
  datasize = 0;
  memset(paramName, 0, CHARSIZE);
  memset(chardata, 0, CHARSIZE);
  memset(description, 0, CHARSIZE);
}
//------------------------------------------------------------------------------
Mimic::Mimic(int num)
{
  flushParams();
  numVars = num;
}
//------------------------------------------------------------------------------
Mimic::Mimic(dataType t, int num)
{
  flushParams();
  type = t;
  numVars = num;
  unbound();
}
//------------------------------------------------------------------------------
Mimic::Mimic(dataType t, double h, double l, int num)
{
  flushParams();
  type = t;
  numVars = num;
  if(h>l)
  {
    high = h;
    low = l;
  }
}
//------------------------------------------------------------------------------
Mimic::Mimic(dataType t, const char *name, int num)
{
  flushParams();
  type = t;
  numVars = num;
  strcpy(paramName, name);
  unbound();
}
//------------------------------------------------------------------------------
Mimic::Mimic(dataType t, const char *name, double h, double l, int num)
{
  flushParams();
  type = t;
  if(h>l)
  {
    high = h;
    low = l;
  }
  numVars = num;
  strcpy(paramName, name);
}
//------------------------------------------------------------------------------
Mimic::~Mimic()
{
  if(dataptr != 0)
  {
    free(dataptr);
    dataptr = 0;
    datasize = 0;
  }
}
//------------------------------------------------------------------------------
dataType Mimic::getType()
{
  return type;
}
//------------------------------------------------------------------------------
int Mimic::getNumVars()
{
  return numVars;
}
//------------------------------------------------------------------------------
double Mimic::getHigh()
{
  return high;
}
//------------------------------------------------------------------------------
double Mimic::getLow()
{
  return low;
}
//------------------------------------------------------------------------------
char* Mimic::getName()
{
  return (char*)&paramName;
}
//------------------------------------------------------------------------------
char* Mimic::getDesc()
{
  return (char*)&description;
}
//------------------------------------------------------------------------------
bool Mimic::isSingleReal()
{
  if(
      (type == DT_FLOAT)  ||
      (type == DT_DOUBLE)
    )
  {
    if(numVars == 1) // Числовой тип и 1 переменная
      return true;
  }

  return false;
}
//------------------------------------------------------------------------------
bool Mimic::isSingleInt()
{
  if(
      (type == DT_INT)  ||
      (type == DT_LONG)
    )
  {
    if(numVars == 1) // Числовой тип и 1 переменная
      return true;
  }

  return false;
}
//------------------------------------------------------------------------------
void Mimic::setHigh(double h)
{
  if(h>low)
    high = h;
}
//------------------------------------------------------------------------------
void Mimic::setLow(double l)
{
  if(l<high)
    low = l;
}
//------------------------------------------------------------------------------
void Mimic::setNumVars(int n)
{
  size_t vs = varsize();
  void *pt = malloc(n*vs);
  memset(pt, 0, n*vs);

  if(numVars > n)
    memcpy(pt, dataptr, n * vs);
  else
    memcpy(pt, dataptr, numVars*vs);

  free(dataptr);
  numVars = n;
  datasize = numVars * sizeof(char);
  dataptr = pt;
}
//------------------------------------------------------------------------------
void Mimic::setParamName(const char *name)
{
  memset(paramName, 0, CHARSIZE);
  strcpy(paramName, name);
}
//------------------------------------------------------------------------------
void Mimic::setDesc(const char *name)
{
  memset(description, 0, CHARSIZE);
  strncpy(description, name, CHARSIZE-1);
}
//------------------------------------------------------------------------------
void Mimic::setType(dataType t)
{
  if(t!=type) //Переконвертация
  {
    if( (t == DT_STRING) || (t == DT_BOOL) || (t == DT_RAW) || (t == DT_NOPE) )
      return; // Не делаем преобразования с такими типами. Пусть перезаводят.

    char *tmp = toCString(0, numVars);
    type = t;
    set(tmp);
    return;
  }
  type = t;
}
//------------------------------------------------------------------------------
size_t Mimic::varsize()
{
  switch(type)
  {
    case DT_NOPE:   return 0;
    case DT_BOOL:   return 1;
//    case DT_BYTE:   return sizeof(char);
//    case DT_SHORT:  return sizeof(short int);
    case DT_INT:    return sizeof(TYPE_INT);
    case DT_LONG:   return sizeof(TYPE_LONG);
    case DT_FLOAT:  return sizeof(TYPE_FLOAT);
    case DT_DOUBLE: return sizeof(TYPE_DOUBLE);
    case DT_COMPLEX:return sizeof(TYPE_COMPLEX);
    case DT_STRING: return sizeof(char);
    case DT_RAW:    return 0;
  }
  return 0;
}
//------------------------------------------------------------------------------
void Mimic::setRaw(void *ptr, int size)
{
  if(type == DT_NOPE) return; // Не работаем с пустым типом
  if(dataptr != 0) free(dataptr);
  dataptr = malloc(size);
  memcpy(dataptr, ptr, size);
}
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_BOOL
void Mimic::set(TYPE_BOOL c, int index)
{
  size_t s = varsize();
  if(s==0) return;
  if((index>=numVars) || (index < 0)) return;

  // Заводим память по факту необходимости
  if( (dataptr == 0) && (type != DT_STRING) )
  {
    datasize = numVars * s;
    dataptr = malloc (datasize);
    memset(dataptr, 0, datasize);
  }

  // Хитрое вычисление адреса
  void *ptr = ( (char*)dataptr + index*s );
  // Укладывание переменной по адресу
  switch(type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      return;
    }
    case DT_BOOL:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_BOOL);
      break;
    }
    case DT_INT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_INT);
      break;
    }
    case DT_LONG:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_LONG);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_FLOAT);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_DOUBLE);
      break;
    }
    case DT_COMPLEX:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_COMPLEX);
      break;
    }
    case DT_STRING:
    {
      sprintf(chardata, "%d", c);
      break;
    }
  }
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_INT
void Mimic::set(TYPE_INT c, int index)
{
  size_t s = varsize();
  if(s==0) return;
  if((index>=numVars) || (index < 0)) return;

  // Заводим память по факту необходимости
  if( (dataptr == 0) && (type != DT_STRING) )
  {
    datasize = numVars * s;
    dataptr = malloc (datasize);
    memset(dataptr, 0, datasize);
  }

  c = limitValue(c);

  // Хитрое вычисление адреса
  void *ptr = ( (char*)dataptr + index*s );
  // Укладывание переменной по адресу
  switch(type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      return;
    }
    case DT_BOOL:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_BOOL);
      break;
    }
    case DT_INT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_INT);
      break;
    }
    case DT_LONG:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_LONG);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_FLOAT);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_DOUBLE);
      break;
    }
    case DT_COMPLEX:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_COMPLEX);
      break;
    }
    case DT_STRING:
    {
      sprintf(chardata, "%d", c);
      break;
    }
  }
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_LONG
void Mimic::set(TYPE_LONG c, int index)
{
  size_t s = varsize();
  if(s==0) return;
  if((index>=numVars) || (index < 0)) return;

  // Заводим память по факту необходимости
  if( (dataptr == 0) && (type != DT_STRING) )
  {
    datasize = numVars * s;
    dataptr = malloc (datasize);
    memset(dataptr, 0, datasize);
  }

  c = limitValue(c);

  // Хитрое вычисление адреса
  void *ptr = ( (char*)dataptr + index*s );
  // Укладывание переменной по адресу
  switch(type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      return;
    }
    case DT_BOOL:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_BOOL);
      break;
    }
    case DT_INT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_INT);
      break;
    }
    case DT_LONG:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_LONG);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_FLOAT);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_DOUBLE);
      break;
    }
    case DT_COMPLEX:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_COMPLEX);
      break;
    }
    case DT_STRING:
    {
      sprintf(chardata, "%ld", c);
      break;
    }
  }
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_FLOAT
void Mimic::set(TYPE_FLOAT c, int index)
{
  size_t s = varsize();
  if(s==0) return;
  if((index>=numVars) || (index < 0)) return;

  // Заводим память по факту необходимости
  if( (dataptr == 0) && (type != DT_STRING) )
  {
    datasize = numVars * s;
    dataptr = malloc (datasize);
    memset(dataptr, 0, datasize);
  }

  c = limitValue(c);

  // Хитрое вычисление адреса
  void *ptr = ( (char*)dataptr + index*s );
  // Укладывание переменной по адресу
  switch(type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      return;
    }
    case DT_BOOL:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_BOOL);
      break;
    }
    case DT_INT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_INT);
      break;
    }
    case DT_LONG:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_LONG);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_FLOAT);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_DOUBLE);
      break;
    }
    case DT_COMPLEX:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_COMPLEX);
      break;
    }
    case DT_STRING:
    {
      sprintf(chardata, "%g", c);
      break;
    }
  }
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_DOUBLE
void Mimic::set(TYPE_DOUBLE c, int index)
{
  size_t s = varsize();
  if(s==0) return;
  if((index>=numVars) || (index < 0)) return;

  // Заводим память по факту необходимости
  if( (dataptr == 0) && (type != DT_STRING) )
  {
    datasize = numVars * s;
    dataptr = malloc (datasize);
    memset(dataptr, 0, datasize);
  }

  c = limitValue(c);

  // Хитрое вычисление адреса
  void *ptr = ( (char*)dataptr + index*s );
  // Укладывание переменной по адресу
  switch(type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      return;
    }
    case DT_BOOL:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_BOOL);
      break;
    }
    case DT_INT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_INT);
      break;
    }
    case DT_LONG:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_LONG);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_FLOAT);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_DOUBLE);
      break;
    }
    case DT_COMPLEX:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_COMPLEX);
      break;
    }
    case DT_STRING:
    {
      sprintf(chardata, "%g", c);
      break;
    }
  }
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_COMPLEX
void Mimic::set(TYPE_COMPLEX c, int index)
{
  size_t s = varsize();
  if(s==0) return;
  if((index>=numVars) || (index < 0)) return;

  // Заводим память по факту необходимости
  if( (dataptr == 0) && (type != DT_STRING) )
  {
    datasize = numVars * s;
    dataptr = malloc (datasize);
    memset(dataptr, 0, datasize);
  }

  c = limitValue(c);

  double cmod = abs(c);

  // Хитрое вычисление адреса
  void *ptr = ( (char*)dataptr + index*s );
  // Укладывание переменной по адресу
  switch(type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      return;
    }
    case DT_BOOL:
    {
      CONVERT_V2P(cmod, TMP_TYPE, ptr, TYPE_BOOL);
      break;
    }
    case DT_INT:
    {
      CONVERT_V2P(cmod, TMP_TYPE, ptr, TYPE_INT);
      break;
    }
    case DT_LONG:
    {
      CONVERT_V2P(cmod, TMP_TYPE, ptr, TYPE_LONG);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_V2P(cmod, TMP_TYPE, ptr, TYPE_FLOAT);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_V2P(cmod, TMP_TYPE, ptr, TYPE_DOUBLE);
      break;
    }
    case DT_COMPLEX:
    {
      CONVERT_V2P(c, TMP_TYPE, ptr, TYPE_COMPLEX);
      break;
    }
    case DT_STRING:
    {
      sprintf(chardata, "(%g:%g)", c.real(), c.imag());
      break;
    }
  }
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
void Mimic::set(const char *str, int index, bool bWipe)
{
  size_t s = varsize();
  if(s==0) return;
//  if((index>=numVars) || (index < 0)) return;

  if(type == DT_STRING)
  {
    memset(chardata, 0, CHARSIZE);
    strncpy(chardata, str, CHARSIZE-1);
    return;
  }

  // Заводим память по факту необходимости
  if( (dataptr == 0) )
  {
    datasize = numVars * s;
    dataptr = malloc (datasize);
    memset(dataptr, 0, datasize);
  }
  else
  {
    if(bWipe) // Вырезаем все данные
      memset(dataptr, 0, datasize);
  }


  // Через строку можно передать несколько значений сразу. Это надо учесть.
  int a_index = -1;     // Добавочный индекс
  int str_p = 0;        // Номер обрабатываемого символа строки
  char buf[CHARSIZE];   // Промежуточный буфер
  int strsize = strlen(str);
  while(true)
  {
    if ( str_p >= strsize ) return; // Строка ну совсем кончилась
    memset(buf, 0, CHARSIZE);
    int i=0;
    while(
           (str[str_p] != ' ') &&
           (str[str_p] != ';') &&
           ( (str_p) < strlen(str) )
          )
    {
      if (str[str_p] == '\0') break; // строка кончилась
      buf[i] = str[str_p];
      i++;
      str_p ++;
    }
    str_p ++; //Установка на следующий символ после пропуска разделителя
    a_index ++;
    // Здесь в buf выдранная подстрока без разделителей, str_p - индекс
    // начала следующей подстроки. a_index - добавочный номер в массиве

    if( (index + a_index >= numVars) || (index + a_index < 0) ) return;
    // Хитрое вычисление адреса
    void *ptr = ( (char*)dataptr + (index+a_index)*s );
    // Укладывание переменной по адресу
    switch(type)
    {
      case DT_NOPE:
      case DT_RAW:
      {
        return;
      }
      case DT_BOOL:
      {
        TYPE_BOOL b;
        double td=0;
        sscanf(buf,"%lf",&td);
        if( (strcmp(buf, "true") == 0) || (td != 0) )
          b = true;
        else
          b = false;
        CONVERT_V2P(b, TYPE_BOOL, ptr, TYPE_BOOL);
        break;
      }
      case DT_INT:
      {
        TYPE_INT i;
        sscanf(buf, "%d", &i);
        i = limitValue(i);
        CONVERT_V2P(i, TYPE_INT, ptr, TYPE_INT);
        break;
      }
      case DT_LONG:
      {
        TYPE_LONG l;
        sscanf(buf, "%ld", &l);
        l = limitValue(l);
        CONVERT_V2P(l, TYPE_LONG, ptr, TYPE_LONG);
        break;
      }
      case DT_FLOAT:
      {
        TYPE_FLOAT f;
        sscanf(buf, "%f", &f);
        f = limitValue(f);
        CONVERT_V2P(f, TYPE_FLOAT, ptr, TYPE_FLOAT);
        break;
      }
      case DT_DOUBLE:
      {
        TYPE_DOUBLE d;
        sscanf(buf, "%lf", &d);
        d = limitValue(d);
        CONVERT_V2P(d, TYPE_DOUBLE, ptr, TYPE_DOUBLE);
        break;
      }
      case DT_COMPLEX: // Тут свистопляска
      {
        if(buf[0] == '(') // Запись в комплексном виде
        {
          TYPE_DOUBLE re, im;
          sscanf(buf, "(%lf:%lf)", &re, &im);
          TYPE_COMPLEX cd(re,im);
          cd = limitValue(cd);
          CONVERT_V2P(cd, TYPE_COMPLEX, ptr, TYPE_COMPLEX);
          break;
        }
        else    // Запись в вещественном виде
        {
          TYPE_DOUBLE cmod;
          sscanf(buf, "%lf", &cmod);
          cmod = limitValue(cmod);
          CONVERT_V2P(cmod, TYPE_DOUBLE, ptr, TYPE_COMPLEX);
          break;
        }
      }
      case DT_STRING:
      {
        break; //не должны вообще сюда попадать
      }
    } //switch(type)
  }// while(true)
}
//------------------------------------------------------------------------------
void* Mimic::toRawData()
{
  return dataptr;
}
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_BOOL
TYPE_BOOL Mimic::toBool(int index)
{
  if(dataptr == 0) return 0;
  size_t s = varsize();
  if(s==0) return 0;
  if ( (index < 0) || (index >= numVars) ) return 0;

  void *ptr = (char*)dataptr + s*index;
  TMP_TYPE res=0;
  switch (type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      break;
    }
    case DT_BOOL:
    {
      CONVERT_P2V(ptr, TYPE_BOOL, res, TMP_TYPE);
      break;
    }
    case DT_INT:
    {
      CONVERT_P2V(ptr, TYPE_INT, res, TMP_TYPE);
      break;
    }
    case DT_LONG:
    {
      CONVERT_P2V(ptr, TYPE_LONG, res, TMP_TYPE);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_P2V(ptr, TYPE_FLOAT, res, TMP_TYPE);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_P2V(ptr, TYPE_DOUBLE, res, TMP_TYPE);
      break;
    }
    case DT_COMPLEX:
    {
      TYPE_DOUBLE cmod = abs( *((TYPE_COMPLEX*)ptr) );
      res = cmod;
      break;
    }
    case DT_STRING:
    {
      TYPE_DOUBLE d;
      sscanf(chardata, "%lg", &d);
      if( (d != 0 ) || (strcmp(chardata, "true") ==0) )
        res = true;
      else
        res = false;
      break;
    }
  }
  return res;
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_INT
TYPE_INT Mimic::toInt(int index)
{
  if(dataptr == 0) return 0;
  size_t s = varsize();
  if(s==0) return 0;
  if ( (index < 0) || (index >= numVars) ) return 0;

  void *ptr = (char*)dataptr + s*index;
  TMP_TYPE res=0;
  switch (type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      break;
    }
    case DT_BOOL:
    {
      CONVERT_P2V(ptr, TYPE_BOOL, res, TMP_TYPE);
      break;
    }
    case DT_INT:
    {
      CONVERT_P2V(ptr, TYPE_INT, res, TMP_TYPE);
      break;
    }
    case DT_LONG:
    {
      CONVERT_P2V(ptr, TYPE_LONG, res, TMP_TYPE);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_P2V(ptr, TYPE_FLOAT, res, TMP_TYPE);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_P2V(ptr, TYPE_DOUBLE, res, TMP_TYPE);
      break;
    }
    case DT_COMPLEX:
    {
      TYPE_DOUBLE cmod = abs( *((TYPE_COMPLEX*)ptr) );
      res = cmod;
      break;
    }
    case DT_STRING:
    {
      sscanf(chardata, "%d", &res);
      break;
    }
  }
  return res;
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_LONG
TYPE_LONG Mimic::toLong(int index)
{
  if(dataptr == 0) return 0;
  size_t s = varsize();
  if(s==0) return 0;
  if ( (index < 0) || (index >= numVars) ) return 0;

  void *ptr = (char*)dataptr + s*index;
  TMP_TYPE res=0;
  switch (type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      break;
    }
    case DT_BOOL:
    {
      CONVERT_P2V(ptr, TYPE_BOOL, res, TMP_TYPE);
      break;
    }
    case DT_INT:
    {
      CONVERT_P2V(ptr, TYPE_INT, res, TMP_TYPE);
      break;
    }
    case DT_LONG:
    {
      CONVERT_P2V(ptr, TYPE_LONG, res, TMP_TYPE);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_P2V(ptr, TYPE_FLOAT, res, TMP_TYPE);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_P2V(ptr, TYPE_DOUBLE, res, TMP_TYPE);
      break;
    }
    case DT_COMPLEX:
    {
      TYPE_DOUBLE cmod = abs( *((TYPE_COMPLEX*)ptr) );
      res = cmod;
      break;
    }
    case DT_STRING:
    {
      sscanf(chardata, "%ld", &res);
      break;
    }
  }
  return res;
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_FLOAT
TYPE_FLOAT Mimic::toFloat(int index)
{
  if(dataptr == 0) return 0;
  size_t s = varsize();
  if(s==0) return 0;
  if ( (index < 0) || (index >= numVars) ) return 0;

  void *ptr = (char*)dataptr + s*index;
  TMP_TYPE res=0;
  switch (type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      break;
    }
    case DT_BOOL:
    {
      CONVERT_P2V(ptr, TYPE_BOOL, res, TMP_TYPE);
      break;
    }
    case DT_INT:
    {
      CONVERT_P2V(ptr, TYPE_INT, res, TMP_TYPE);
      break;
    }
    case DT_LONG:
    {
      CONVERT_P2V(ptr, TYPE_LONG, res, TMP_TYPE);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_P2V(ptr, TYPE_FLOAT, res, TMP_TYPE);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_P2V(ptr, TYPE_DOUBLE, res, TMP_TYPE);
      break;
    }
    case DT_COMPLEX:
    {
      TYPE_DOUBLE cmod = abs( *((TYPE_COMPLEX*)ptr) );
      res = cmod;
      break;
    }
    case DT_STRING:
    {
      sscanf(chardata, "%g", &res);
      break;
    }
  }
  return res;
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_DOUBLE
TYPE_DOUBLE Mimic::toDouble(int index)
{
  if(dataptr == 0) return 0;
  size_t s = varsize();
  if(s==0) return 0;
  if ( (index < 0) || (index >= numVars) ) return 0;

  void *ptr = (char*)dataptr + s*index;
  TMP_TYPE res=0;
  switch (type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      break;
    }
    case DT_BOOL:
    {
      CONVERT_P2V(ptr, TYPE_BOOL, res, TMP_TYPE);
      break;
    }
    case DT_INT:
    {
      CONVERT_P2V(ptr, TYPE_INT, res, TMP_TYPE);
      break;
    }
    case DT_LONG:
    {
      CONVERT_P2V(ptr, TYPE_LONG, res, TMP_TYPE);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_P2V(ptr, TYPE_FLOAT, res, TMP_TYPE);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_P2V(ptr, TYPE_DOUBLE, res, TMP_TYPE);
      break;
    }
    case DT_COMPLEX:
    {
      TYPE_DOUBLE cmod = abs( *((TYPE_COMPLEX*)ptr) );
      res = cmod;
      break;
    }
    case DT_STRING:
    {
      sscanf(chardata, "%lg", &res);
      break;
    }
  }
  return res;
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
#define TMP_TYPE TYPE_COMPLEX
TYPE_COMPLEX Mimic::toComplex(int index)
{
  if(dataptr == 0) return 0;
  size_t s = varsize();
  if(s==0) return 0;
  if ( (index < 0) || (index >= numVars) ) return 0;

  void *ptr = (char*)dataptr + s*index;
  TMP_TYPE res=0;
  switch (type)
  {
    case DT_NOPE:
    case DT_RAW:
    {
      break;
    }
    case DT_BOOL:
    {
      CONVERT_P2V(ptr, TYPE_BOOL, res, TMP_TYPE);
      break;
    }
    case DT_INT:
    {
      CONVERT_P2V(ptr, TYPE_INT, res, TMP_TYPE);
      break;
    }
    case DT_LONG:
    {
      CONVERT_P2V(ptr, TYPE_LONG, res, TMP_TYPE);
      break;
    }
    case DT_FLOAT:
    {
      CONVERT_P2V(ptr, TYPE_FLOAT, res, TMP_TYPE);
      break;
    }
    case DT_DOUBLE:
    {
      CONVERT_P2V(ptr, TYPE_DOUBLE, res, TMP_TYPE);
      break;
    }
    case DT_COMPLEX:
    {
      CONVERT_P2V(ptr, TYPE_COMPLEX, res, TMP_TYPE);
      break;
    }
    case DT_STRING:
    {
      if(chardata[0] == '(') // Запись в комплексном виде
      {
        TYPE_DOUBLE re, im;
        sscanf(chardata, "(%lf:%lf)", &re, &im);
        TYPE_COMPLEX cd(re,im);
        res = cd;
        break;
      }
      else // Запись в вещественном виде
      {
        TYPE_DOUBLE cmod;
        sscanf(chardata, "%lf", &cmod);
        cmod = limitValue(cmod);
        res = cmod;
        break;
      }
    }
  }
  return res;
}
#undef TMP_TYPE
//------------------------------------------------------------------------------
char* Mimic::toCString(int index, int amount, const char* delimiter)
{
  if(type == DT_STRING)
  {
    return &(chardata[0]);
  }

  if(amount < 0) // Вывод всех элементов массива
  {
    amount = numVars;
    index = 0;
  }

  memset(chardata, 0, CHARSIZE);
  size_t s = varsize();
  if( (s==0) || (dataptr == 0) ||
      (index < 0) || (index >= numVars))
    {
      sprintf(chardata, "<no data>");
      return &(chardata[0]); //при ошибке возвращаем пустую строку
    }

  int charbuf_index = 0;
  int max_index;
  if(index+amount > numVars)
    max_index = numVars;
  else
    max_index = index + amount;

  for(int a=0; a<max_index; a++)
  {

    void *ptr = (char*)dataptr + s*a;

    switch (type)
    {
      case DT_NOPE:
      case DT_RAW:
      case DT_STRING: //уже обработано, тут для исключения варнинга
        break;
      case DT_BOOL:
      {
        TYPE_BOOL b;
        CONVERT_P2V(ptr, TYPE_BOOL, b, TYPE_BOOL);
        if(b)
          sprintf(&(chardata[0]) + charbuf_index, "true");
        else
          sprintf(&(chardata[0]) + charbuf_index, "false");
        break;
      }

      case DT_INT:
      {
        TYPE_INT i;
        CONVERT_P2V(ptr, TYPE_INT, i, TYPE_INT);
        sprintf(&(chardata[0]) + charbuf_index,"%d", i);
        break;
      }

      case DT_LONG:
      {
        TYPE_LONG l;
        CONVERT_P2V(ptr, TYPE_LONG, l, TYPE_LONG);
        sprintf(&(chardata[0]) + charbuf_index,"%ld", l);
        break;
      }

      case DT_FLOAT:
      {
        TYPE_FLOAT f;
        CONVERT_P2V(ptr, TYPE_FLOAT, f, TYPE_FLOAT);
        char    *old = setlocale(LC_NUMERIC,"C");
        sprintf(&(chardata[0]) + charbuf_index,"%g", f);
        setlocale(LC_NUMERIC,old);
        break;
      }

      case DT_DOUBLE:
      {
        TYPE_DOUBLE d;
        CONVERT_P2V(ptr, TYPE_DOUBLE, d, TYPE_DOUBLE);
        char    *old = setlocale(LC_NUMERIC,"C");
        sprintf(&(chardata[0]) + charbuf_index,"%lg", d);
        setlocale(LC_NUMERIC,old);
        break;
      }

      case DT_COMPLEX:
      {
        TYPE_COMPLEX cd;
        CONVERT_P2V(ptr, TYPE_COMPLEX, cd, TYPE_COMPLEX);
        sprintf(&(chardata[0]) + charbuf_index,"(%lg:%lg)", cd.real(), cd.imag());
        break;
      }

    } //case
    charbuf_index = strlen(chardata);
    if (a < max_index - 1)
    {
      sprintf(&(chardata[0]) + charbuf_index,"%s", delimiter);
      charbuf_index = strlen(chardata);
    }

    if(charbuf_index > (CHARSIZE-10)) break; // 10 символов резерва
  } // for

  return &(chardata[0]);
}
//------------------------------------------------------------------------------
TYPE_INT Mimic::limitValue(TYPE_INT i)
{
  if(i>high) i = high;
  if(i<low)  i = low;
  return i;
}
//------------------------------------------------------------------------------
TYPE_LONG Mimic::limitValue(TYPE_LONG i)
{
  if(i>high) i = high;
  if(i<low)  i = low;
  return i;
}
//------------------------------------------------------------------------------
TYPE_FLOAT Mimic::limitValue(TYPE_FLOAT i)
{
  if(i>high) i = high;
  if(i<low)  i = low;
  return i;
}
//------------------------------------------------------------------------------
TYPE_DOUBLE Mimic::limitValue(TYPE_DOUBLE i)
{
  if(i>high) i = high;
  if(i<low)  i = low;
  return i;
}
//------------------------------------------------------------------------------
TYPE_COMPLEX Mimic::limitValue(TYPE_COMPLEX i)
{
  TYPE_COMPLEX res = i;
  TYPE_DOUBLE cmod = abs(i);
  if(cmod > high)
    res = i * high / cmod;
  if(cmod < low)
    res = i * low / cmod;

  return res;
}
//------------------------------------------------------------------------------
void Mimic::unbound()
{
  switch(type)
  {
    case DT_INT:
      high = INT32_MAX;
      low = INT32_MIN;
      break;
    case DT_LONG:
      high = INT64_MAX;
      low = INT64_MIN;
      break;
    case DT_FLOAT:
      high = FLT_MAX;
      low = FLT_MIN;
      break;
    case DT_DOUBLE:
    case DT_COMPLEX:
      high = DBL_MAX;
      low = DBL_MIN;
      break;
    default:
      high = INT32_MAX;
      low = INT32_MIN;
      break;
  }
}
//------------------------------------------------------------------------------
