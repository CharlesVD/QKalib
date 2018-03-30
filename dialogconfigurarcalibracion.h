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

    int obtenerAltoPatron();
    int obtenerAnchoPatron();
    double obtenerTamCuadro();

private:
    Ui::DialogConfigurarCalibracion *ui;
};

#endif // DIALOGCONFIGURARCALIBRACION_H
