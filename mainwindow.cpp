#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dialogVerMuestras = new DialogVerMuestras(this);
    dialogConfigCalib = new DialogConfigurarCalibracion(this);

    actualiza = new QTimer(this);

    calibrar = false;
    grabando = false;
    capturando=false;
    noMuestrasCalib=0;
    noImgGrabadas=0;

    tamCuadroCalib=0.0257142857143;
    altoPatron=6;
    anchoPatron=9;


    actualizaListaDeCamaras();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mostrarMensaje(QString info)
{
    ui->textEditResultado->setHtml(ui->textEditResultado->toHtml()+info);
    ui->textEditResultado->verticalScrollBar()->setSliderPosition(ui->textEditResultado->verticalScrollBar()->maximum());
}

void MainWindow::actualizaListaDeCamaras()
{
    ui->comboBoxCamaras->clear();

    camaras = QCameraInfo::availableCameras();
    ui->comboBoxCamaras->addItem("Selecciona la cámara");
    for(int i=0;i<camaras.length();i++)
    {
        ui->comboBoxCamaras->addItem(camaras.at(i).description());
    }
}

void MainWindow::eliminarMuestras()
{
    coord2D.clear();
    coord3D.clear();

    for(int i=0;i<noMuestrasCalib;i++)
    {
        imagenesMustra[i].release();
    }
    noMuestrasCalib=0;
    mostrarMensaje("<b style='color:red;'>Muestras eliminadas</b>");
}

bool MainWindow::camaraDisponible()
{
    bool disponible;
    camaras = QCameraInfo::availableCameras();
    for(int i=0;i<camaras.length();i++)
    {
        if(ui->comboBoxCamaras->currentText()==camaras.at(i).description())
        {
            disponible=true;
        }
    }
    return disponible;
}

void MainWindow::detenerGrabacion()
{
    grabando=false;
    ui->pushButtonGrabar->setIcon(QIcon(":/Acciones/Grabar.png"));
    mostrarMensaje("<b style='color:red;'>Grabación detenida</b>");

    ui->lineEditDireccionGrabar->setEnabled(true);
    ui->pushButtonExaminarArchivosGrabar->setEnabled(true);

    QString direccion=ui->lineEditDireccionGrabar->text();
    if(noImgGrabadas>0)
    {
        mostrarMensaje("<b>Guardando archivos...</b>");
        for(int i=0;i<noImgGrabadas;i++)
        {
            QString nombreArchivo=QString::number(i);
            QString zeros = "";
            for(int zero=(4-nombreArchivo.length());zero>=0;zero--)
            {
                zeros+="0";
            }
            QString archivoGrabado=direccion+zeros+nombreArchivo+".png";

            if(cv::imwrite(archivoGrabado.toStdString(),imgGrabadas[i]))
            {
                mostrarMensaje("<i>"+archivoGrabado+"</i>");
            }
            else
            {
                mostrarMensaje("<b style='color:red;'>error al guardar: "+archivoGrabado+"</b>");
                break;
            }

        }
        mostrarMensaje("<b>Archivos guardados</b>");
        noImgGrabadas=0;
    }
}

void MainWindow::iniciarCaptura()
{
    connect(actualiza,SIGNAL(timeout()),this,SLOT(actualizaVista()));
    actualiza->start();
    ui->pushButtonCaptura->setIcon(QIcon(":/Acciones/Parar.png"));
    ui->comboBoxCamaras->setEnabled(false);
    ui->pushButtonActualizaListaDeCamaras->setEnabled(false);

    capturando=true;
    ui->pushButtonCalibrar->setEnabled(true);
}

void MainWindow::detenerCaptura()
{
    actualiza->stop();
    camara.release();

    capturando=false;

    ui->pushButtonCaptura->setIcon(QIcon(":/Acciones/Captura.png"));
    mostrarMensaje("<b style='color:red;'>Sin cámara capturando</b>");
    ui->comboBoxCamaras->setEnabled(true);
    ui->pushButtonActualizaListaDeCamaras->setEnabled(true);

    calibrar=false;
    //ui->pushButtonCalibrar->setText("Iniciar calibración");
    ui->pushButtonCalibrar->setIcon(QIcon(":/Acciones/Patron.png"));
    ui->pushButtonTomarMuestra->setEnabled(false);
    ui->pushButtonCalibrar->setEnabled(false);
}

void MainWindow::calcPosEsquinas(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners)
{
    corners.clear();

    for( int i = 0; i < boardSize.height; ++i )
    {
        for( int j = 0; j < boardSize.width; ++j )
        {
            corners.push_back(cv::Point3f(float( j*squareSize ), float( i*squareSize ), 0));
        }
    }
}

void MainWindow::actualizaVista()
{
    if(camaraDisponible())
    {
        cv::Size tamPatron(altoPatron,anchoPatron);
        std::vector<cv::Point2f> Esquinas2D;
        std::vector<cv::Point3f> Esquinas3D;
        cv::Mat img;
        camara.read(img);

        cv::cvtColor(img,img,CV_RGB2GRAY);

        if(grabando)
        {
            if(noImgGrabadas<10000)
            {
                imgGrabadas[noImgGrabadas]=img;
                noImgGrabadas++;
            }
            else
            {
                detenerGrabacion();
                mostrarMensaje("<b style='color:red;'>Grabación detenida por exeder límite de imágenes</b>");
            }
        }

        if(calibrar)
        {
            //ui->statusBar->showMessage("Calibrando",100);
            bool patronEncontrado = cv::findChessboardCorners(img, tamPatron, Esquinas2D,
                                                              cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE
                                                              + cv::CALIB_CB_FAST_CHECK);
            if(patronEncontrado)
            {
                cv::cornerSubPix(img, Esquinas2D, cv::Size(10, 10), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

                cv::Mat imgMuestra;
                img.copyTo(imgMuestra);
                cv::drawChessboardCorners(img, tamPatron, cv::Mat(Esquinas2D), patronEncontrado);

                calcPosEsquinas(tamPatron, tamCuadroCalib, Esquinas3D);

                ui->pushButtonTomarMuestra->setEnabled(true);

                if(ui->pushButtonTomarMuestra->isChecked())
                {
                    if(noMuestrasCalib<50)
                    {
                    imgMuestra.copyTo(imagenesMustra[noMuestrasCalib]);
                    coord2D.push_back(Esquinas2D);
                    coord3D.push_back(Esquinas3D);
                    noMuestrasCalib++;
                    mostrarMensaje("<p>Número de muestras: "+QString::number(noMuestrasCalib)+"</p>");
                    }
                    else
                    {
                        QMessageBox::warning(this,"Alerta","No se pueden agregar más muestras. Se han tomado el máximo de muestras permitido.",QMessageBox::Ok);
                    }
                }
                ui->pushButtonTomarMuestra->setChecked(false);
            }
            else
            {
                ui->pushButtonTomarMuestra->setEnabled(false);
            }

        }

        QImage mostrarVideoF =  QImage((const unsigned char*)(img.data),
                                       img.cols,img.rows,QImage::Format_Indexed8);

        ui->labelSalidaCam->setPixmap(QPixmap::fromImage(mostrarVideoF));
    }
    else
    {
        detenerCaptura();
        actualizaListaDeCamaras();

        QMessageBox::warning(this,"Cámara no disponible","Alerta: La cámara se ha desconectado",QMessageBox::Ok);
    }
}

void MainWindow::on_pushButtonCaptura_clicked()
{
    if(capturando)
    {
        if(grabando)
        {
            detenerGrabacion();
            mostrarMensaje("<b style='color:red;'>Se forzó para detener la grabación</b>");
        }
        detenerCaptura();
    }
    else
    {
        if(ui->comboBoxCamaras->currentIndex()!=0)
        {
            bool disponible=camaraDisponible();

            if(disponible)
            {
                //            QString noCamara="";
                //            for(int i=ui->comboBoxCamaras->currentText().length()-1;i>0-1;i--)
                //            {
                //                if(ui->comboBoxCamaras->currentText().at(i).isDigit())
                //                {
                //                    noCamara.prepend(ui->comboBoxCamaras->currentText().at(i));
                //                }
                //                else
                //                {
                //                    break;
                //                }
                //            }
                camara.release();
                camara = cv::VideoCapture(ui->comboBoxCamaras->currentIndex()-1);
                //camara.set(CV_CAP_PROP_FRAME_WIDTH ,1024);
                //camara.set(CV_CAP_PROP_FRAME_HEIGHT,768);
                //camara.set(CV_CAP_PROP_FPS,10);
                mostrarMensaje("<b style='color:red;'>Capturando video...</b>");
                mostrarMensaje("<b>"+camaras.at(ui->comboBoxCamaras->currentIndex()-1).description()+"</b>");
                mostrarMensaje("<b>Resolución: "+QString::number(camara.get(CV_CAP_PROP_FRAME_WIDTH))+"x"+QString::number(camara.get(CV_CAP_PROP_FRAME_HEIGHT))+"</b>");
                mostrarMensaje("<b>Velocidad: "+QString::number(camara.get(CV_CAP_PROP_FPS))+" FPS</b>");
                if(!actualiza->isActive())
                {
                    ui->pushButtonGrabar->setEnabled(true);
                    iniciarCaptura();
                }
            }
            else
            {
                QMessageBox::warning(this,"Cámara no disponible","Alerta: La cámara seleccionada no está disponible.\nSeleccione un dispositivo que esté conectado al equipo.",QMessageBox::Ok);
                actualizaListaDeCamaras();
            }
        }
    }
}

void MainWindow::on_pushButtonCalibrar_clicked()
{
    calibrar = !calibrar;
    if(calibrar)
    {
        ui->pushButtonCalibrar->setIcon(QIcon(":/Acciones/CancelarPatron.png"));
        //ui->pushButtonCalibrar->setText("Parar");
        ui->pushButtonTomarMuestra->setEnabled(true);
        ui->pushButtonTomarMuestra->setVisible(true);
    }
    else
    {
        ui->pushButtonCalibrar->setIcon(QIcon(":/Acciones/Patron.png"));
        //ui->pushButtonCalibrar->setText("Iniciar calibración");
        ui->pushButtonTomarMuestra->setEnabled(false);
        ui->pushButtonTomarMuestra->setVisible(true);
    }
}

void MainWindow::on_pushButtonCalcularCalib_clicked()
{
    if(noMuestrasCalib>7)
    {
        cv::Mat img = imagenesMustra[0].clone();

        cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;

        double rms = cv::calibrateCamera(coord3D, coord2D, img.size(), cameraMatrix,
                                         distCoeffs, rvecs, tvecs,
                                         cv::CALIB_FIX_PRINCIPAL_POINT +
                                         cv::CALIB_FIX_ASPECT_RATIO +
                                         cv::CALIB_ZERO_TANGENT_DIST
                                         ,cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 2.22e-16));
        QString resultado ="<b>Resultado de Calibración</b><br>";
        resultado +="Error: "+QString::number(rms)+"<br>";
        resultado += "fx: "+QString::number(cameraMatrix.at<double>(0,0))+"<br>";
        resultado += "fy: "+QString::number(cameraMatrix.at<double>(1,1))+"<br>";
        resultado += "cx: "+QString::number(cameraMatrix.at<double>(0,2))+"<br>";
        resultado += "cy: "+QString::number(cameraMatrix.at<double>(1,2))+"<br>";
        resultado += "distorción: <br>";
        for(int i=0;i<8;i++)
        {
        resultado+=QString::number(distCoeffs.at<double>(i,0))+"<br>";
        }
        mostrarMensaje(resultado);
    }
    else
    {
        ui->statusBar->showMessage("Faltan muestras",1000);
        QMessageBox::information(this,"Muestras insuficientes","Es necesario capturar por lo menos 10 muestras para calibrar",QMessageBox::Ok);
    }
}

void MainWindow::on_actionPanel_principal_toggled(bool arg1)
{
    ui->dockWidgetPanelPrincipal->setVisible(arg1);
}

void MainWindow::on_pushButtonActualizaListaDeCamaras_clicked()
{
    actualizaListaDeCamaras();
}

void MainWindow::on_pushButtonExaminarArchivosGrabar_clicked()
{
    QString direccion=QFileDialog::getExistingDirectory(this,"Seleciona la ubicacion para grabar","");
    if(direccion!="")
    {
        ui->lineEditDireccionGrabar->setText(direccion+"/");
    }
}

void MainWindow::on_pushButtonGrabar_clicked()
{
    if(ui->lineEditDireccionGrabar->text()=="Guardar grabación en...")
    {
        QMessageBox::information(this,"Sin ruta","Seleccione una dirección válida para grabar",QMessageBox::Ok);
    }
    else
    {
        grabando=!grabando;
        if(grabando)
        {
            ui->pushButtonGrabar->setIcon(QIcon(":/Acciones/PararGrabacion.png"));
            mostrarMensaje("<b style='color:red;'>Grabando...</b>");
            ui->lineEditDireccionGrabar->setEnabled(false);
            ui->pushButtonExaminarArchivosGrabar->setEnabled(false);
        }
        else
        {
            detenerGrabacion();
        }
    }
}

void MainWindow::on_pushButtonVerMuestras_clicked()
{
    if(capturando)
    {
        detenerCaptura();
    }
    dialogVerMuestras->leerImagenesMuestra(imagenesMustra,noMuestrasCalib);
    dialogVerMuestras->exec();
}

void MainWindow::on_pushButtonGuardarMuestras_clicked()
{
    if(noMuestrasCalib>0)
    {
        if(noMuestrasCalib<50)
        {
            detenerCaptura();
            QString direccion=QFileDialog::getExistingDirectory(this,"Seleciona la ubicación para guardar",QDir::homePath());
            if(direccion!="")
            {
                mostrarMensaje("<b>Guardando archivos...</b>");
                for(int i=0;i<noMuestrasCalib;i++)
                {
                    QString archivoGuardado=direccion+"/"+QString::number(i)+".png";
                    if(cv::imwrite(archivoGuardado.toStdString(),imagenesMustra[i]))
                    {
                        mostrarMensaje("<i>"+archivoGuardado+"</i>");
                    }
                    else
                    {
                        mostrarMensaje("<b style='color:red;'>error al guardar: "+archivoGuardado+"</b>");
                        break;
                    }
                }
                mostrarMensaje("<b>Archivos guardados</b>");
            }
            else
            {
                QMessageBox::information(this,"Sin ruta","Seleccione una dirección válida para guardar",QMessageBox::Ok);
            }
        }
        else
        {
            QMessageBox::warning(this,"Alerta","No se pueden agregar más muestras. Se han tomado el máximo de muestras permitido.",QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::information(this,"Sin muestras","No tiene muestras de patrones para guardar",QMessageBox::Ok);
    }
}

void MainWindow::on_pushAgregarPatron_clicked()
{
    QString archivo=QFileDialog::getOpenFileName(this,"Seleciona la ubicación para guardar",QDir::homePath(),"Imagenes (*.png *.jpg *.bmp)");
    if(archivo!="")
    {
        cv::Mat muestra = cv::imread(archivo.toStdString());
        cv::cvtColor(muestra,muestra,CV_RGB2GRAY);

        cv::Size tamPatron(altoPatron,anchoPatron);
        std::vector<cv::Point2f> Esquinas2D;
        std::vector<cv::Point3f> Esquinas3D;

        bool patronEncontrado = cv::findChessboardCorners(muestra, tamPatron, Esquinas2D,
                                                          cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE
                                                          + cv::CALIB_CB_FAST_CHECK);
        if(patronEncontrado)
        {
            cv::cornerSubPix(muestra, Esquinas2D, cv::Size(10, 10), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

            calcPosEsquinas(tamPatron, tamCuadroCalib, Esquinas3D);

            coord2D.push_back(Esquinas2D);
            coord3D.push_back(Esquinas3D);

            imagenesMustra[noMuestrasCalib] = muestra;
            noMuestrasCalib++;
            mostrarMensaje("<p>Número de muestras: "+QString::number(noMuestrasCalib)+"</p>");
        }
        else
        {
            QMessageBox::information(this,"Patrón no encontrado","No se encontró ningún patrón en la imagen seleccionada",QMessageBox::Ok);
        }

    }
    else
    {
        QMessageBox::information(this,"Sin ruta","Seleccione una dirección válida para guardar",QMessageBox::Ok);
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(noMuestrasCalib>0)
    {
        QMessageBox::warning(this,"Alerta","Si se cambia la configuración, se eliminarán las muestras obtenidas para evitar errores de calibración.",QMessageBox::Ok);
    }
    detenerCaptura();
    dialogConfigCalib->ponerTamanioCuadro(tamCuadroCalib);
    dialogConfigCalib->ponerTamanioPatron(altoPatron,anchoPatron);
    dialogConfigCalib->exec();
    if(dialogConfigCalib->result())
    {
        tamCuadroCalib=dialogConfigCalib->obtenerTamCuadro();
        altoPatron=dialogConfigCalib->obtenerAltoPatron();
        anchoPatron=dialogConfigCalib->obtenerAnchoPatron();
        if(noMuestrasCalib>0)
        {

        }
    }
}

void MainWindow::on_pushButtonEliminarMuestras_clicked()
{
    eliminarMuestras();
}

void MainWindow::on_actionAcerca_de_Qt_triggered()
{
    QMessageBox::aboutQt(this,"Acerca de Qt");
}

void MainWindow::on_actionAcerca_de_triggered()
{
    QMessageBox::about(this,"Acerca de...","Este software está bajo la licencia GNU LGPL version 3.\n\n Desarrollado por: Charles Fernando Velázquez Dodge");
}
