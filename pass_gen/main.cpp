#include <QCoreApplication>
#include <QFile>
#include <ctime>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 2) {
        printf("Arguments error! \n");
        exit(EXIT_FAILURE);
    }
    bool ok;
    int symbols_num = QString("%1").arg(argv[1]).toInt(&ok, 10);

    QFile source;
    source.setFileName("../pass_gen/symbols.txt");
    if (!source.open(QFile::ReadOnly)) {
        printf("File error! \n");
        exit(EXIT_FAILURE);
    }
    QByteArray data = source.readAll();
    source.close();
    const int source_size = data.size();
    char symbols_array[source_size];
    memcpy(&symbols_array, data.data(), source_size*sizeof(char));

    QString result;
    srand(time(0));
    for (int i = 0; i < symbols_num; i++)
        result.append(symbols_array[rand() % source_size]);

    cout << result.toStdString() << endl;

    exit(EXIT_SUCCESS);

    return a.exec();
}
