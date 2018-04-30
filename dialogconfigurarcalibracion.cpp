#include "dialogconfigurarcalibracion.h"
#include "ui_dialogconfigurarcalibracion.h"

DialogConfigurarCalibracion::DialogConfigurarCalibracion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfigurarCalibracion)
{
    ui->setupUi(this);
}

DialogConfigurarCalibracion::~DialogConfigurarCalibracion()
{
    delete ui;
}

void DialogConfigurarCalibracion::ponerTamanioPatron(int alto, int ancho)
{
    ui->spinBoxAlto->setValue(alto);
    ui->spinBoxAncho->setValue(ancho);
}

void DialogConfigurarCalibracion::ponerTamanioCuadro(double tam)
{
    ui->doubleSpinBoxTamCuadro->setValue(tam);
}

void DialogConfigurarCalibracion::ponerFx(double fx)
{
    ui->doubleSpinBoxFx->setValue(fx);
}

void DialogConfigurarCalibracion::ponerFy(double fy)
{
    ui->doubleSpinBoxFy->setValue(fy);
}

void DialogConfigurarCalibracion::ponerCx(double cx)
{
    ui->doubleSpinBoxCx->setValue(cx);
}

void DialogConfigurarCalibracion::ponerCy(double cy)
{
    ui->doubleSpinBoxCy->setValue(cy);
}

void DialogConfigurarCalibracion::ponerCorrecX1(double x1)
{
    ui->doubleSpinBoxCorrecX1->setValue(x1);
}

void DialogConfigurarCalibracion::ponerCorrecX2(double x2)
{
    ui->doubleSpinBoxCorrecX2->setValue(x2);
}

void DialogConfigurarCalibracion::ponerCorrecX3(double x3)
{
    ui->doubleSpinBoxCorrecX3->setValue(x3);
}

void DialogConfigurarCalibracion::ponerCorrecX4(double x4)
{
    ui->doubleSpinBoxCorrecX4->setValue(x4);
}

void DialogConfigurarCalibracion::ponerCorrecY1(double y1)
{
    ui->doubleSpinBoxCorrecY1->setValue(y1);
}

void DialogConfigurarCalibracion::ponerCorrecY2(double y2)
{
    ui->doubleSpinBoxCorrecY2->setValue(y2);
}

void DialogConfigurarCalibracion::ponerCorrecY3(double y3)
{
    ui->doubleSpinBoxCorrecY3->setValue(y3);
}

void DialogConfigurarCalibracion::ponerCorrecY4(double y4)
{
    ui->doubleSpinBoxCorrecY4->setValue(y4);
}

double DialogConfigurarCalibracion::obtenerFx()
{
    return ui->doubleSpinBoxFx->value();
}

double DialogConfigurarCalibracion::obtenerFy()
{
    return ui->doubleSpinBoxFy->value();
}

double DialogConfigurarCalibracion::obtenerCx()
{
    return ui->doubleSpinBoxCx->value();
}

double DialogConfigurarCalibracion::obtenerCy()
{
    return ui->doubleSpinBoxCy->value();
}

double DialogConfigurarCalibracion::obtenerCorrecX1()
{
    return ui->doubleSpinBoxCorrecX1->value();
}

double DialogConfigurarCalibracion::obtenerCorrecX2()
{
    return ui->doubleSpinBoxCorrecX2->value();
}

double DialogConfigurarCalibracion::obtenerCorrecX3()
{
    return ui->doubleSpinBoxCorrecX3->value();
}

double DialogConfigurarCalibracion::obtenerCorrecX4()
{
    return ui->doubleSpinBoxCorrecX4->value();
}

double DialogConfigurarCalibracion::obtenerCorrecY1()
{
    return ui->doubleSpinBoxCorrecY1->value();
}

double DialogConfigurarCalibracion::obtenerCorrecY2()
{
    return ui->doubleSpinBoxCorrecY2->value();
}

double DialogConfigurarCalibracion::obtenerCorrecY3()
{
    return ui->doubleSpinBoxCorrecY3->value();
}

double DialogConfigurarCalibracion::obtenerCorrecY4()
{
    return ui->doubleSpinBoxCorrecY4->value();
}

int DialogConfigurarCalibracion::obtenerAltoPatron()
{
    return ui->spinBoxAlto->value();
}

int DialogConfigurarCalibracion::obtenerAnchoPatron()
{
    return ui->spinBoxAncho->value();
}

double DialogConfigurarCalibracion::obtenerTamCuadro()
{
    return ui->doubleSpinBoxTamCuadro->value();
}
