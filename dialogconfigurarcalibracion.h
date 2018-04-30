#ifndef DIALOGCONFIGURARCALIBRACION_H
#define DIALOGCONFIGURARCALIBRACION_H

#include <QDialog>

namespace Ui {
class DialogConfigurarCalibracion;
}

class DialogConfigurarCalibracion : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfigurarCalibracion(QWidget *parent = 0);
    ~DialogConfigurarCalibracion();

    void ponerTamanioPatron(int alto,int ancho);
    void ponerTamanioCuadro(double tam);

    void ponerFx(double fx);
    void ponerFy(double fy);

    void ponerCx(double cx);
    void ponerCy(double cy);

    void ponerCorrecX1(double x1);
    void ponerCorrecX2(double x2);
    void ponerCorrecX3(double x3);
    void ponerCorrecX4(double x4);

    void ponerCorrecY1(double y1);
    void ponerCorrecY2(double y2);
    void ponerCorrecY3(double y3);
    void ponerCorrecY4(double y4);

    int obtenerAltoPatron();
    int obtenerAnchoPatron();
    double obtenerTamCuadro();

    double obtenerFx();
    double obtenerFy();

    double obtenerCx();
    double obtenerCy();

    double obtenerCorrecX1();
    double obtenerCorrecX2();
    double obtenerCorrecX3();
    double obtenerCorrecX4();

    double obtenerCorrecY1();
    double obtenerCorrecY2();
    double obtenerCorrecY3();
    double obtenerCorrecY4();

private:
    Ui::DialogConfigurarCalibracion *ui;
};

#endif // DIALOGCONFIGURARCALIBRACION_H
