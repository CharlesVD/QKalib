#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dialogvermuestras.h"
#include "dialogconfigurarcalibracion.h"

#include <QMainWindow>
#include <QtMultimedia/QCameraInfo>
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

    void on_pushButtonCaptura_clicked();

    void on_pushButtonCalibrar_clicked();

    void on_pushButtonCalcularCalib_clicked();

    void on_actionPanel_principal_toggled(bool arg1);

    void on_pushButtonActualizaListaDeCamaras_clicked();

    void on_pushButtonExaminarArchivosGrabar_clicked();

    void on_pushButtonGrabar_clicked();

    void on_pushButtonVerMuestras_clicked();

    void on_pushButtonGuardarMuestras_clicked();

    void on_pushAgregarPatron_clicked();

    void on_pushButton_clicked();

    void on_pushButtonEliminarMuestras_clicked();

    void on_actionAcerca_de_Qt_triggered();

    void on_actionAcerca_de_triggered();

private:
    Ui::MainWindow *ui;

    DialogVerMuestras *dialogVerMuestras;
    DialogConfigurarCalibracion *dialogConfigCalib;

    QTimer *actualiza;
    cv::VideoCapture camara;
    QList<QCameraInfo> camaras;
    bool calibrar;
    bool capturando;
    bool grabando;
    int noImgGrabadas;
    cv::Mat imgGrabadas[10000];

    double tamCuadroCalib;
    int altoPatron;
    int anchoPatron;

    std::vector<std::vector<cv::Point2f>> coord2D;
    std::vector<std::vector<cv::Point3f>> coord3D;
    int noMuestrasCalib;
    cv::Mat imagenesMustra[50];
    static void calcPosEsquinas(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners);

    void actualizaListaDeCamaras();
    void eliminarMuestras();
    void detenerCaptura();
    void detenerGrabacion();
    void iniciarCaptura();
    bool camaraDisponible();

    void mostrarMensaje(QString info);
};

#endif // MAINWINDOW_H
