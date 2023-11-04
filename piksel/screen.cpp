#include "screen.h"
#include "ui_screen.h"
#include "colorpicker.h"

#include <QMouseEvent>
#include <QPainter>
#include <algorithm>
#include <QMessageBox>
#include <windows.h>
#include <QDir>
#include <QPoint>
#include <QDebug>
#include <QStack>
#include <QThread>

#define MAX_OBJECTS 200
#define M_PI 3.14159265358979323846
using namespace std;

Screen::Screen(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Screen)
{
	ui->setupUi(this);
    colorp = QImage(18,18,QImage::Format_RGB32);
    colorp.fill(Qt::white);
}

Screen::~Screen()
{
    delete ui;
}

void Screen::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.fillRect(0,0,width(),22,Qt::black);
    p.drawImage(0,22,im);
    p.drawImage(79,2,colorp);
}

int calcParts(QPoint P0, QPoint P1, QPoint P2, QPoint P3){
    int DPT[4] = {abs(P0.x()-P1.x())+abs(P0.y()-P1.y()),
                  abs(P1.x()-P2.x())+abs(P1.y()-P2.y()),
                  abs(P2.x()-P3.x())+abs(P2.y()-P3.y()),
                  abs(P3.x()-P0.x())+abs(P3.y()-P0.y())};
    return *(std::max_element(DPT,DPT+4));
}

int calcBSklejane(int P0, int P1, int P2, int P3, double j){
    return ((-pow(j,3)+(3*pow(j,2))-(3*j)+1)/6*P0)+((3*pow(j,3)-(6*pow(j,2))+4)/6*P1)+(((-3*pow(j,3))+(3*pow(j,2))+(3*j)+1)/6*P2)+pow(j,3)/6*P3;
}

int calcBezier(int P0, int P1, int P2, int P3, double j){
    double tmp = 1-j;
    return pow(tmp,3)*P0 + 3*pow(tmp,2)*j*P1 + 3*tmp*pow(j,2)*P2 + pow(j,3)*P3;
}

void PointsStructures::drawPointsStructure(){
    if (points.size()<4) return;
    QPoint last(0,0);
    if(pointsMode == 0){
        for(int i = 3; i<points.size(); i+=3){
            int PARTS = calcParts(points[i-3],points[i-2],points[i-1],points[i]);
            for(int jj = 0; jj<PARTS; jj++){
                double j = (double)jj/PARTS;
                int x = calcBezier(points[i-3].x(),points[i-2].x(),points[i-1].x(),points[i].x(), j);
                int y = calcBezier(points[i-3].y(),points[i-2].y(),points[i-1].y(),points[i].y(), j);
                if (jj > 0) mainScreen->drawObject(last.x(), last.y(), x, y, r,g,b, 0);
                last = QPoint(x,y);
            }
        }
    } else if (pointsMode == 1){
        for(int i = 3; i<points.size(); i++){
            int PARTS = calcParts(points[i-3],points[i-2],points[i-1],points[i]);
            for(int jj = 0; jj<PARTS; jj++){
                double j = (double)jj/PARTS;
                int x = calcBSklejane(points[i-3].x(),points[i-2].x(),points[i-1].x(),points[i].x(), j);
                int y = calcBSklejane(points[i-3].y(),points[i-2].y(),points[i-1].y(),points[i].y(), j);
                if (jj > 0) mainScreen->drawObject(last.x(), last.y(), x, y, r,g,b, 0);
                last = QPoint(x,y);
            }
        }
    } else if (pointsMode == 2) {
        if(points[0] == points[points.size()-1]){
            QVector<double> xDy;
            int min = 100000, max = -1;
            for(int i = 0; i<points.size(); i++) {
                if (i <points.size()-1) xDy.push_back((double)(points[i].x()-points[i+1].x())/(points[i].y()-points[i+1].y()));
                if (min > points[i].y()) min = points[i].y();
                if (max < points[i].y()) max = points[i].y();
            }
            for(int i = 1;i<points.size(); i++){
                mainScreen->drawObject(points[i].x(),points[i].y(),points[i-1].x(),points[i-1].y(),r,g,b,0);
            }
            for(int y = min; y<=max; y++) {
                QVector<int> foundX;
                foundX.clear();
                for(int i = 0; i<points.size()-1; i++){
                    if (y > std::min(points[i].y(),points[i+1].y()) && y <= std::max(points[i].y(),points[i+1].y())){
                        foundX.push_back(ceil(points[i].x()+(y-points[i].y())*xDy[i]));
                    }
                }
                if(foundX.size() > 1) {
                    sort(foundX.begin(), foundX.end());
                    for(int i = 1; i<foundX.size(); i+= 2){
                        mainScreen->drawObject(foundX[i],y,foundX[i-1],y,r,g,b,0);
                    }
                }
            }
        }
    }
}

void PointsStructures::drawPoints(){
    for(int i = 0; i<points.size(); i++) {
        int tmpr = r, tmpg = g, tmpb = b;
        for (int j = 2; j<=5; j++) {
            mainScreen->paintPiksel(points[i].x(),points[i].y(),0,0,0);
            if (i == 0 || i == points.size()-1) {
                if (i == 0) tmpr = 255, tmpg = 0, tmpb = 0;
                if (i == points.size()-1) tmpr = 0, tmpg = 255, tmpb = 0;
                if (j == 5) tmpr = 0, tmpg = 0, tmpb = 0;
            } else {
                if (j == 4) tmpr = 0, tmpg = 0, tmpb = 0;
                if (j == 5) break;
            }
            mainScreen->drawObject(points[i].x()-j, points[i].y()-j, points[i].x()+j, points[i].y()-j, tmpr, tmpg, tmpb, 0);
            mainScreen->drawObject(points[i].x()-j, points[i].y()+j, points[i].x()+j, points[i].y()+j, tmpr, tmpg, tmpb, 0);
            mainScreen->drawObject(points[i].x()+j, points[i].y()+j, points[i].x()+j, points[i].y()-j, tmpr, tmpg, tmpb, 0);
            mainScreen->drawObject(points[i].x()-j, points[i].y()-j-1, points[i].x()-j, points[i].y()+j, tmpr, tmpg, tmpb, 0);
        }
    }
}

int PointsStructures::posPlacementPoint(QPoint point){
    for(int i = 0; i<points.size(); i++){
        if (round(sqrt(pow(point.x()-points[i].x(),2)+pow(point.y()-points[i].y(), 2))) < 7) return i;
    }
    return -1;
}

void PointsStructures::addPoint(QPoint position){
    if (points.size() == 0) {
        r = mainScreen->r; g = mainScreen->g; b = mainScreen->b;
    }
    if(posPlacementPoint(position) == -1) {
        if(!(points.size() > 2 && points.constFirst() == points.constLast())) points.push_back(position);
    } else {
        if(pointsMode == 2 && points.size() > 2 && posPlacementPoint(position) == 0){
            points.push_back(points[0]);
        }
    }
    mainScreen->drawFrame();
}

void PointsStructures::delPoint(QPoint position){
    if(posPlacementPoint(position) != -1) {
        if (posPlacementPoint(position) == 0 && points.size() > 2){
            points.remove(points.size()-1);
        } else {
            points.remove(posPlacementPoint(position));
        }
        mainScreen->drawFrame();
   }
}

void Screen::drawFrame(int points){
    im.fill(Qt::black);
    pStruct->drawPointsStructure();
    paintObjects();
    if (!points && currentMode == 3) pStruct->drawPoints();
    updateBar();
    update();
}

void Screen::paintPiksel(int x, int y, int r, int g, int b){
    y -= 22;
    if (x<0 || x>im.width()-1 || y<0 || y>im.height()-1) return;
    uchar *pix = im.scanLine(y);
    pix[x*4 + 0] = b;
    pix[x*4 + 1] = g;
    pix[x*4 + 2] = r;
}

QColor Screen::getColor(int x, int y) {
    QColor out;
    y -= 22;
    if (x<0 || x>im.width()-1 || y<0 || y>im.height()-1) return QColor(-1);
    uchar *pix = im.scanLine(y);
    out.setBlue(pix[x*4 + 0]);
    out.setGreen(pix[x*4 + 1]);
    out.setRed(pix[x*4 + 2]);
    return out;
}

void Screen::floodFill(int x0, int y0, QColor toChange, QColor withThat) {
    QStack<QPoint> Q;
    Q.push(QPoint(x0,y0));
    while (!Q.empty()) {
        QPoint last = Q.pop();
        if (getColor(last.x(), last.y()) == toChange) {
            int w = last.x(); int e = last.x();
            while (getColor(w,last.y()) == toChange && w>=0) w--;
            while (getColor(e,last.y()) == toChange && e<im.width()) e++;
            for (int i=w+1; i<e; i++) paintPiksel(i,last.y(), withThat.red(), withThat.green(), withThat.blue());
            for (int i=w+1; i<e; i++) {
                if (getColor(i,last.y()+1) == toChange && last.y()+1 <= im.height()+22) Q.push(QPoint(i,last.y()+1));
                if (getColor(i,last.y()-1) == toChange && last.y()-1 >= 22) Q.push(QPoint(i,last.y()-1));
            }
        }
    }
}

void Screen::drawObject(int x1, int y1, int x2, int y2, int r, int g, int b, int mode){
    if (x1 == x2 && y1 == y2) {
        paintPiksel(x1,y1,r,g,b);
        update();
        return;
    } else if (mode == 0) { //LINIA
        double L = (double)(y2-y1)/(double)(x2-x1);
        int i = x1;
        if(abs(L) < 1) {
            while(i != x2){
                if(x2>i) i++;
                else i--;
                Screen::paintPiksel(i,y1+(L*(i-x1)),r,g,b);
            }
        }else {
            i = y1;
            L = (double)(x2-x1)/(double)(y2-y1);
            while(i != y2){
                if(y2>i) i++;
                else i--;
                Screen::paintPiksel(x1+(L*(i-y1)),i,r,g,b);
            }
        }
    } else if (mode == 1) { // OKRĄG
        int R = sqrt(pow((x2-x1),2)+pow((y2-y1),2));
        for(int i = 0; i<=(R/sqrt(2))+0.5; i++) {
            int y = (sqrt(pow(R,2)-pow(i,2)))+0.5;
            paintPiksel(i+x1,  y+y1,r,g,b);
            paintPiksel(-i+x1, y+y1,r,g,b);
            paintPiksel(i+x1, -y+y1,r,g,b);
            paintPiksel(-i+x1,-y+y1,r,g,b);
            paintPiksel(y+x1,  i+y1,r,g,b);
            paintPiksel(-y+x1, i+y1,r,g,b);
            paintPiksel(y+x1, -i+y1,r,g,b);
            paintPiksel(-y+x1,-i+y1,r,g,b);
        }
    } else if (mode == 2) { // ELIPSA
        int R1 = abs(x2-x1);
        int R2 = abs(y2-y1);
        int x,y,xo,yo, n = sqrt(R1+R2)*6;
        for(int i = 0; i<=n; i++) {
            double L = i*2*M_PI/n;
            x = R1*cos(L);
            y = R2*sin(L);
            if (i>0) drawObject(x+x1,y+y1,xo+x1,yo+y1,r,g,b,0);
            xo = x;
            yo = y;
        }
    } else if (mode == 4){
        floodFill(x1,y1,getColor(x1,y1),QColor(r,g,b));
    }
    update();
}

void Screen::paintObjects(){
    for(int i = 0; i<Objects.size(); i++){
        drawObject(Objects[i].x1, Objects[i].y1, Objects[i].x2, Objects[i].y2, Objects[i].r, Objects[i].g, Objects[i].b, Objects[i].mode);
    }
}

void Screen::resizeEvent(QResizeEvent*){
    im = QImage(this->width(),this->height()-22,QImage::Format_RGB32);
    drawFrame();
}

void Screen::mousePressEvent(QMouseEvent *e){
    secound = false;
    if((Objects.size()+pStruct->points.size()) == MAX_OBJECTS) QMessageBox::warning(this, "ERROR MESSAGE", "Osiągnięto limit możliwych obiektów do narysowania.");
    QPoint position = e->pos();
    this->x1 = position.x();
    this->y1 = position.y();
    if(currentMode == 3) {
        if(e->button() == Qt::RightButton) {
            grabbed = pStruct->posPlacementPoint(position);
        } else if (e->button() == Qt::LeftButton) {
            pStruct->addPoint(position);
        } else if (e->button() == Qt::MiddleButton) {
            pStruct->delPoint(position);
        }
    }
    if (currentMode == 4) {
        if(getColor(x1,y1) == QColor(r,g,b)) return;
        Objects.push_back(Data(x1,y1,0,0,r,g,b,currentMode));
    }
}

void Screen::mouseMoveEvent(QMouseEvent *e) {
    QPoint position = e->pos();
    if (currentMode < 3){
        drawFrame();
        drawObject(x1,y1,position.x(),position.y(),r,g,b,currentMode);
    } else if (currentMode == 3 && grabbed != -1) {
        if (position.x()<0 || position.x()>im.width()-1 || position.y()<22 || position.y()>im.height()+22) return;
        if(pStruct->pointsMode == 2 && grabbed == 0 && pStruct->points.constFirst() == pStruct->points.constLast()){
            grabbed = pStruct->points.size()-1;
        } else {
            if (pStruct->posPlacementPoint(position) == -1) secound = true;
            if (grabbed == pStruct->points.size()-1 && pStruct->posPlacementPoint(position) == 0){
                if (secound){
                    pStruct->points[grabbed] = pStruct->points[0];
                    grabbed = -1;
                }
            } else {
                pStruct->points[grabbed] = position;
            }
        }
        drawFrame();
    }
}

void Screen::mouseReleaseEvent(QMouseEvent *e){
    if (currentMode < 3) {
        QPoint position = e->pos();
        Objects.push_back(Data(x1,y1,position.x(),position.y(),r,g,b,currentMode));
    } else if (currentMode == 3 && grabbed != -1) grabbed = -1;
    drawFrame();
}

void Screen::setColour(int r, int g, int b) {
    this->r = r; this->g = g; this->b = b;
    colorp.fill(QColor(this->r,this->g,this->b));
    delete(cpick);
    cpick = NULL;
    update();
}

void Screen::on_clearAll_clicked()
{
    im.fill(Qt::black);
    Objects.clear();
    pStruct->points.clear();
    updateBar();
}
void Screen::on_clearLst_clicked()
{
    if ((Objects.size()+pStruct->points.size()) < 1) return;
    if (currentMode != 3 && Objects.size() > 0) Objects.pop_back();
    else if (currentMode == 3 && pStruct->points.size() > 0) pStruct->points.pop_back();
    drawFrame();
}

void Screen::updateBar(){
    ui->progressBar->setValue(round(((Objects.size()+pStruct->points.size())/(double)(MAX_OBJECTS))*100));
    update();
}

void Screen::on_saveFile_clicked()
{
    QDir desktop = QDir::home();
    desktop.cd("Desktop");
    QString input = ui->lineEdit->text();
    if(input.length()<1) return;
    input += ".png";
    drawFrame(1);
    if(!im.save(desktop.absolutePath() + '\\' + input)) QMessageBox::warning(this, "ERROR MESSAGE", "Nie udało się zapisać pliku.");
    drawFrame();
}

void Screen::on_comboBox_currentIndexChanged(int index)
{
    currentMode = index;
    drawFrame();
}

void Screen::on_comboBox_2_currentIndexChanged(int index)
{
    pStruct->pointsMode = index;
    drawFrame();
}

void Screen::on_pushButton_clicked()
{
    if (cpick == NULL) {
        cpick = new colorPicker(this);
        cpick->setFixedSize(cpick->width(), cpick->height());
        cpick->show();
        cpick->setVals(r, g, b);
        cpick->setSliders();
    }
}
