#include "colorpicker.h"
#include "ui_colorpicker.h"
#include "screen.h"

#include <QImage>
#include <QMouseEvent>
#include <QPainter>
using namespace std;

colorPicker::colorPicker(QWidget *parent)
    : QDialog{parent}
    , ui(new Ui::colorPicker)
{
    ui->setupUi(this);
    main = QImage(480,480,QImage::Format_RGB32);
    pickedc = QImage(80,80,QImage::Format_RGB32);
    main.fill(Qt::black);
    pickedc.fill(Qt::black);
}

colorPicker::~colorPicker()
{
    delete ui;
}

void colorPicker::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(10,10,main);
    p.drawImage(500,332,pickedc);
}

void colorPicker::reject(){
    Screen *par = (Screen*)this->parent();
    par->cpick = NULL;
    this->close();
}

void colorPicker::on_RSlide_valueChanged(int value)
{
    whatChanged = 0;
    this->r = value;
    calcColors();
}
void colorPicker::on_GSlide_valueChanged(int value)
{
    whatChanged = 1;
    this->g = value;
    calcColors();
}
void colorPicker::on_BSlide_valueChanged(int value)
{
    whatChanged = 2;
    this->b = value;
    calcColors();
}
void colorPicker::on_HSlide_valueChanged(int value)
{
    whatChanged = 3;
    h = value;
    calcColors();
}
void colorPicker::on_SSlide_valueChanged(int value)
{
    whatChanged = 4;
    s = value/255.0;
    calcColors();
}
void colorPicker::on_VSlide_valueChanged(int value)
{
    whatChanged = 5;
    v = value/255.0;
    calcColors();
}

void colorPicker::setSliders()
{
    QSlider *sliders[6] = {ui->RSlide,ui->GSlide,ui->BSlide,ui->HSlide,ui->SSlide,ui->VSlide};
    for (int i = 0; i<6; i++) sliders[i]->blockSignals(true);
    sliders[0]->setSliderPosition(r);
    sliders[1]->setSliderPosition(g);
    sliders[2]->setSliderPosition(b);
    sliders[3]->setSliderPosition(h);
    sliders[4]->setSliderPosition(s*255);
    sliders[5]->setSliderPosition(v*255);
    for(int i = 0; i<6; i++) sliders[i]->blockSignals(false);
    pickedc.fill(QColor(r,g,b));
    update();
}

void colorPicker::setVals(int x,int y,int z){
    r = x, g = y, b = z;
}

void colorPicker::mousePressEvent(QMouseEvent *e){
    QPoint position = e->pos();
    position = QPoint(position.x()-10,position.y()-10);
    if (position.x()<0 || position.x()>main.width()-1 || position.y()<0 || position.y()>main.height()-1) return;
    uchar *pix = main.scanLine(position.y());
    setVals(pix[position.x()*4+2], pix[position.x()*4+1], pix[position.x()*4+0]);
    setSliders();
}

void colorPicker::paintPikselRGB(int x, int y, int r, int g, int b){
    if (x<0 || x>main.width()-1 || y<0 || y>main.height()-1) return;
    uchar *pix = main.scanLine(y);
    pix[x*4 + 0] = b;
    pix[x*4 + 1] = g;
    pix[x*4 + 2] = r;
}

void colorPicker::paintPikselHSV(int x, int y, int H, double S, double V){
    if (x<0 || x>main.width()-1 || y<0 || y>main.height()-1) return;
    int tmpr = 0, tmpg = 0, tmpb = 0;
    double C = V*S;
    double tmph = H/60.0;
    double X = C*(1.0-abs(fmod(tmph,2)-1.0));
    double m = (V-C)*255;
    tmph = ceil(tmph);
    C *= 255;
    X *= 255;
    if (tmph == 1)      {tmpr = C; tmpg = X; tmpb = 0;}
    else if (tmph == 2) {tmpr = X; tmpg = C; tmpb = 0;}
    else if (tmph == 3) {tmpr = 0; tmpg = C; tmpb = X;}
    else if (tmph == 4) {tmpr = 0; tmpg = X; tmpb = C;}
    else if (tmph == 5) {tmpr = X; tmpg = 0; tmpb = C;}
    else if (tmph == 6) {tmpr = C; tmpg = 0; tmpb = X;}
    else {tmpr = 0; tmpg = 0; tmpb = 0;}
    tmpr += m; tmpg += m; tmpb += m;
    uchar *pix = main.scanLine(y);
    pix[x*4 + 0] = tmpb;
    pix[x*4 + 1] = tmpg;
    pix[x*4 + 2] = tmpr;
}

void colorPicker::calcColors() {
    if (whatChanged >= 0 && whatChanged <= 2){
        double tmp[3] = {r/255.0,g/255.0,b/255.0};
        double M = *std::max_element(begin(tmp),end(tmp));
        double m = *std::min_element(begin(tmp),end(tmp));
        double C = M - m;
        if (M == m) h = 0;
        else if (M == tmp[0]) h = 60.0*(tmp[1]-tmp[2])/C;
        else if (M == tmp[1]) h = 60.0*(2.0 + (tmp[2]-tmp[0])/C);
        else if (M == tmp[2]) h = 60.0*(4.0 + (tmp[0]-tmp[1])/C);
        if (h < 0) h = h+360;
        v = M;
        s = C/M;
        for(int i = 0; i<main.height(); i++){
            for(int j = 0; j<main.width(); j++){
                if (whatChanged == 0) paintPikselRGB(i,j, r, (i*255)/main.width(), (j*255)/main.width());
                if (whatChanged == 1) paintPikselRGB(i,j, (i*255)/main.width(), g, (j*255)/main.width());
                if (whatChanged == 2) paintPikselRGB(i,j, (i*255)/main.width(), (j*255)/main.width(), b);
            }
        }
    } else if (whatChanged >= 3 && whatChanged <= 5) {
        double C = v*s;
        double tmph = h/60.0;
        double X = C*(1.0-abs(fmod(tmph,2)-1.0));
        double m = (v-C)*255;
        tmph = ceil(tmph);
        C *= 255;
        X *= 255;
        if (tmph == 1) setVals(C,X,0);
        else if (tmph == 2) setVals(X,C,0);
        else if (tmph == 3) setVals(0,C,X);
        else if (tmph == 4) setVals(0,X,C);
        else if (tmph == 5) setVals(X,0,C);
        else if (tmph == 6) setVals(C,0,X);
        else setVals(0,0,0);
        setVals(r+m,g+m,b+m);
        for(int i = 0; i<main.height(); i++){
            for(int j = 0; j<main.width(); j++){
                if (whatChanged == 3) paintPikselHSV(i,j, h, i/(double)main.height(), j/(double)main.width());
                if (whatChanged == 4) paintPikselHSV(i,j, (i*360)/(double)main.height(), s, j/(double)main.width());
                if (whatChanged == 5) paintPikselHSV(i,j, (i*360)/(double)main.height(), j/(double)main.width(), v);
            }
        }
    }
    setSliders();
}

void colorPicker::on_pushButton_clicked()
{
    Screen *par = (Screen*)this->parent();
    par->setColour(r,g,b);
}
