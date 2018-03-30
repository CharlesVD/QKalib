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
