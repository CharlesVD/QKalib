#ifndef DIALOGVERMUESTRAS_H
#define DIALOGVERMUESTRAS_H

#include <QDialog>
#include <opencv2/highgui.hpp>

namespace Ui {
class DialogVerMuestras;
}

class DialogVerMuestras : public QDialog
{
    Q_OBJECT

public:
    explicit DialogVerMuestras(QWidget *parent = 0);
    ~DialogVerMuestras();
    void leerImagenesMuestra(cv::Mat imagenesMuestra[50],int noMuestras);
    void mostrarImagen(cv::Mat imagenCv);
    void ponIndice(int index);

private slots:
    void on_pushButtonAdelante_clicked();

    void on_pushButtonAtras_clicked();

    void on_pushButtonInicio_clicked();

    void on_pushButtonFinal_clicked();

private:
    Ui::DialogVerMuestras *ui;
    cv::Mat imagenes[50];
    int indice;
    int noImagenes;
    void actualizaIndice();
};

#endif // DIALOGVERMUESTRAS_H
