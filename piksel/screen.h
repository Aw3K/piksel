#ifndef SCREEN_H
#define SCREEN_H

#include "colorpicker.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Screen; }
QT_END_NAMESPACE

class Data {
    public:
        int x1,x2,y1,y2,r,g,b,mode;
    Data(){}
    Data(int a, int b, int c, int d, int rr, int gg, int bb, int mode) {
        x1 = a; y1 = b; x2 = c; y2 = d; r = rr; g = gg; this->b = bb; this->mode = mode;
    }
};

class PointsStructures;

class Screen : public QWidget
{
    Q_OBJECT
public:
	Screen(QWidget *parent = nullptr);
    ~Screen();
    int r = 255, g = 255, b = 255, grabbed = -1;
    bool secound;
    Ui::Screen *ui;
    PointsStructures *pStruct;
    QImage im, colorp;
    colorPicker *cpick = NULL;
    int currentMode = 0, currentGrabbed = -1;
    void paintPiksel(int, int, int, int, int);
    void drawObject(int,int,int,int,int,int,int,int);
    void drawFrame(int points = 0);
    QColor getColor(int,int);
    void floodFill(int,int,QColor,QColor);
    void setColour(int,int,int);

protected:
    int x1,y1;
    QVector<Data> Objects;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent*);
    void paintObjects();
    void updateBar();

private slots:
    void on_clearAll_clicked();
    void on_clearLst_clicked();
    void on_saveFile_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_comboBox_2_currentIndexChanged(int index);
    void on_pushButton_clicked();
};

class PointsStructures {
    public:
        Screen *mainScreen;
        int r,g,b;
        int pointsMode = 0;
        QVector<QPoint> points;
        void drawPointsStructure();
        void drawPoints();
        int  posPlacementPoint(QPoint);
        void addPoint(QPoint);
        void delPoint(QPoint);
    PointsStructures(){ points.clear(); }
};
#endif // SCREEN_H
