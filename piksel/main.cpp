#include <QApplication>
#include "screen.h"
#include "colorpicker.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PointsStructures pStruct;
    Screen N;
    pStruct.mainScreen = &N;
    N.pStruct = &pStruct;
    N.show();
    return a.exec();
}
