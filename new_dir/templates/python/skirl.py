# coding=utf-8

from struct import pack,unpack
from math import sqrt,floor

# Параметры:
#  path — путь к файлу
# Возвращаемое значение: двумерный массив отсчетов, оформленный как
# массив из массивов.
def readArray(path):
    res = []
    try:
        F=open(path, "rb")
    except IOError:
        return
    while True:
        s = F.read(16)
        if(len(s) < 16):
            break
        header = unpack("iihhi", s)
        flags = header[0]
        size = header[1]
        angle = header[2]
        time = header[4]
        line=[]
        for a in range(size):
            s = F.read(4);
            if(len(s) < 4):
                break
            pt = unpack("hh", s)
            line.append(list(pt));
        res.append(line)
    F.close()
    return res

# Параметры:
#  field — массив, записываемый в файл
#  path — путь к файлу, в который следует сохранить данные
def writeArray(field, path):
    F=open(path, "wb")
    for a in range(len(field)):
        line = field[a]
        size=len(line)
        angle = floor(a*10000.0/2400.0)
        time=100
        s=pack("iihHi", 0, size, angle, 0xDEAD, time)
        F.write(s)
        for b in range(len(line)):
            s = pack("hh", line[b][0], line[b][1])
            F.write(s)
    F.close()
