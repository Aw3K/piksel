#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class colorPicker; }
QT_END_NAMESPACE

class colorPicker : public QDialog {
    Q_OBJECT
public:
    colorPicker(QWidget *parent = nullptr);
    ~colorPicker();
    Ui::colorPicker *ui;
    QImage main, pickedc;
    int whatChanged = -1;
    int r = 0, g = 0, b = 0, h = 252;
    double s = 0.2, v = 0.7;
    void reject();
    void paintEvent(QPaintEvent *);
    void calcColors();
    void setSliders();
    void setVals(int x, int y, int z);
    void paintPikselRGB(int x, int y, int r, int g, int b);
    void paintPikselHSV(int x, int y, int H, double S, double V);
    void mousePressEvent(QMouseEvent *);
private slots:
    void on_RSlide_valueChanged(int value);
    void on_GSlide_valueChanged(int value);
    void on_BSlide_valueChanged(int value);
    void on_pushButton_clicked();
    void on_HSlide_valueChanged(int value);
    void on_SSlide_valueChanged(int value);
    void on_VSlide_valueChanged(int value);
};

#endif // COLORPICKER_H
