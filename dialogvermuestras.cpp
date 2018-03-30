#include "dialogvermuestras.h"
#include "ui_dialogvermuestras.h"

DialogVerMuestras::DialogVerMuestras(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVerMuestras)
{
    ui->setupUi(this);
    indice=0;
    noImagenes=0;
    actualizaIndice();
}

DialogVerMuestras::~DialogVerMuestras()
{
    delete ui;
}

void DialogVerMuestras::mostrarImagen(cv::Mat imagenCv)
{
    QImage imagen =  QImage((const unsigned char*)(imagenCv.data),
            imagenCv.cols,imagenCv.rows,QImage::Format_Indexed8);
    ui->labelImagenMuestra->setPixmap(QPixmap::fromImage(imagen));
}

void DialogVerMuestras::actualizaIndice()
{
    ui->labelIndice->setText(QString::number(indice+1)+" de "+QString::number(noImagenes+1));
}

void DialogVerMuestras::leerImagenesMuestra(cv::Mat imagenesMuestra[50], int noMuestras)
{
    for(int i=0;i<noMuestras;i++)
    {
        imagenes[i]=imagenesMuestra[i];
    }
    noImagenes=noMuestras-1;

    if(noMuestras>0)
    {
        mostrarImagen(imagenes[0]);
    }
    actualizaIndice();
}

void DialogVerMuestras::on_pushButtonAdelante_clicked()
{
    if(indice<noImagenes)
    {
        indice++;
        mostrarImagen(imagenes[indice]);
        actualizaIndice();
    }
}

void DialogVerMuestras::on_pushButtonAtras_clicked()
{
    if(indice>0)
    {
        indice--;
        mostrarImagen(imagenes[indice]);
        actualizaIndice();
    }
}

void DialogVerMuestras::on_pushButtonInicio_clicked()
{
    indice=0;
    mostrarImagen(imagenes[indice]);
    actualizaIndice();
}

void DialogVerMuestras::on_pushButtonFinal_clicked()
{
    if(indice<noImagenes)
    {
        indice=noImagenes;
        mostrarImagen(imagenes[indice]);
        actualizaIndice();
    }
}
