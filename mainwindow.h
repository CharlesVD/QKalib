#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dialogvermuestras.h"
#include "dialogconfigurarcalibracion.h"

#include <QMainWindow>
#include <QtMultimedia/QCameraInfo>
#include <QStandardPaths>
#include <QMessageBox>
#include <QScrollBar>
#include <QFileDialog>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d.hpp>
//#include <opencv2/ccalib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void actualizaVista();

    void actualizaVistaGrabacion();

    void segTranscurrido();

    void guardadndoArchivos();

    void calcFps();

    void on_pushButtonCaptura_clicked();

    void on_pushButtonCalibrar_clicked();

    void on_pushButtonCalcularCalib_clicked();

    void on_pushButtonActualizaListaDeCamaras_clicked();

    void on_pushButtonGrabar_clicked();

    void on_pushButtonVerMuestras_clicked();

    void on_pushButtonGuardarMuestras_clicked();

    void on_pushAgregarPatron_clicked();

    void on_pushButtonConfigCalib_clicked();

    void on_pushButtonEliminarMuestras_clicked();

    void on_actionAcerca_de_Qt_triggered();

    void on_actionAcerca_de_triggered();

    void on_pushButtonDist_clicked();

    void on_pushButtonGuardarGrabacion_clicked();

    void on_actionVer_Panel_triggered();

    void on_actionVer_herramientas_de_calibracion_triggered();

    void on_actionVer_informacion_triggered();

    void on_pushButtonEliminarGrabacion_clicked();

    void on_pushButtonVerGrabacion_clicked();

    void on_horizontalSliderProgresoGrabacion_sliderPressed();

    void on_horizontalSliderProgresoGrabacion_sliderReleased();

    void on_pushButtonDetenerVerGrabacion_clicked();

    void on_horizontalSliderProgresoGrabacion_sliderMoved(int position);

private:
    Ui::MainWindow *ui;

    QString dir;
    int indiceVerGrabacion;
    int indiceFrame;
    int noFramesCap;
    float framesPerSecond;
    int tiempoCuadroGrabando;

    DialogVerMuestras *dialogVerMuestras;
    DialogConfigurarCalibracion *dialogConfigCalib;

    QTimer *actualiza;
    QTimer *fps;
    QTimer *relojGrabacion;
    QTimer *conteoGuardarArchivos;
    QTimer *actualizaVerGrabacion;

    cv::VideoCapture camara;
    QList<QCameraInfo> camaras;
    bool calibrar;
    bool capturando;
    bool grabando;
    bool reproduciendoGrabacion;
    bool quitarDistorcion;
    int noImgGrabadas;
    bool navegarGrabacion;
    cv::Mat imgGrabadas[10000];

    float segTotalGrabacion;
    float segGrabacion;
    uint minGrabacion;
    uint horasGrabacion;

    double tamCuadroCalib;
    int altoPatron;
    int anchoPatron;

    std::vector<std::vector<cv::Point2f>> coord2D;
    std::vector<std::vector<cv::Point3f>> coord3D;
    int noMuestrasCalib;
    cv::Mat imagenesMustra[50];
    static void calcPosEsquinas(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners);

    cv::Mat matrizCamara;
    cv::Mat distCoeffs;

    void actualizaListaDeCamaras();
    void eliminarMuestras();
    void detenerCaptura();
    void detenerGrabacion();
    void detenerVerGrabacion();
    void pausarVerGrabacion();
    void eliminarGrabacion();
    void iniciarCaptura();
    bool camaraDisponible();

    void mostrarMensaje(QString info);
};

#endif // MAINWINDOW_H
