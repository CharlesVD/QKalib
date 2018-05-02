#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dialogVerMuestras = new DialogVerMuestras(this);
    dialogConfigCalib = new DialogConfigurarCalibracion(this);

    ui->actionVer_Panel->setChecked(true);
    ui->actionVer_herramientas_de_calibracion->setChecked(true);
    ui->actionVer_informacion->setChecked(true);

    ui->frameEspacio->setVisible(false);
    ui->horizontalSliderProgresoGrabacion->setVisible(false);

    actualiza = new QTimer(this);
    relojGrabacion = new QTimer(this);
    conteoGuardarArchivos = new QTimer(this);
    fps = new QTimer(this);
    actualizaVerGrabacion = new QTimer(this);

    horasGrabacion=0;
    minGrabacion=0;
    segGrabacion=0;
    segTotalGrabacion=0;

    tiempoCuadroGrabando=0;

    calibrar = false;
    grabando = false;
    capturando=false;
    reproduciendoGrabacion=false;
    navegarGrabacion=false;
    quitarDistorcion=false;
    noMuestrasCalib=0;
    noImgGrabadas=0;

    noFramesCap=0;
    framesPerSecond=0;

    indiceVerGrabacion=0;

    tamCuadroCalib=0.0257142857143;
    altoPatron=6;
    anchoPatron=9;

    matrizCamara = cv::Mat::eye(3, 3, CV_64F);
    distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

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
    #ifdef _WIN32
    int camCount=0;
    int index=0;
    QList<int> numCam;
    while(true)
    {
        camara = cv::VideoCapture(index);
        if(camara.isOpened())
        {
            numCam.append(index);
            camCount++;
        }
        index++;
        camara.release();
        if(camCount==camaras.length())
        {
            break;
        }
    }

    camara.~VideoCapture();
    ui->comboBoxCamaras->addItem("Selecciona la cámara");
    for(int i=0;i<camaras.length();i++)
    {
        ui->comboBoxCamaras->addItem(camaras.at(i).description()+" | Video: "+QString::number(numCam.at(i)));
    }
    #elif __linux__
    ui->comboBoxCamaras->addItem("Selecciona la cámara");
    for(int i=0;i<camaras.length();i++)
    {
        ui->comboBoxCamaras->addItem(camaras.at(i).description()+" | "+camaras.at(i).deviceName());
    }
    #endif
}

void MainWindow::eliminarMuestras()
{
    coord2D.clear();
    coord3D.clear();

    for(int i=0;i<noMuestrasCalib;i++)
    {
        imagenesMustra[i] = cv::Mat();
        imagenesMustra[i].release();
    }
    noMuestrasCalib=0;
    mostrarMensaje("<b style='color:red;'>Muestras eliminadas</b>");
}

void MainWindow::eliminarGrabacion()
{
    if(grabando)
    {
        detenerGrabacion();
    }
    for(int i=0;i<noImgGrabadas;i++)
    {
        imgGrabadas[i] = cv::Mat();
        imgGrabadas[i].release();
    }
    segGrabacion=0;
    segTotalGrabacion=0;
    minGrabacion=0;
    horasGrabacion=0;
    mostrarMensaje("<b style='color:red;'>Se eliminaron "+QString::number(noImgGrabadas)+" fotogramas de grabación</b>");
    noImgGrabadas=0;
    indiceFrame=0;

    ui->pushButtonEliminarGrabacion->setEnabled(false);
    ui->pushButtonGuardarGrabacion->setEnabled(false);

    detenerVerGrabacion();

    ui->pushButtonVerGrabacion->setEnabled(false);
    ui->pushButtonDetenerVerGrabacion->setEnabled(false);
}

bool MainWindow::camaraDisponible()
{
    bool disponible=false;
    camaras = QCameraInfo::availableCameras();
    #ifdef _WIN32
    for(int i=0;i<camaras.length();i++)
    {

        QString camaraSelec=ui->comboBoxCamaras->currentText();
        camaraSelec = camaraSelec.split("|").at(0);

        if( camaraSelec==camaras.at(i).description()+" " )
        {
            disponible=true;
        }
    }
    #elif __linux__
    for(int i=0;i<camaras.length();i++)
    {
        if(ui->comboBoxCamaras->currentText()==camaras.at(i).description()+" | "+camaras.at(i).deviceName())
        {
            disponible=true;
        }
    }
    #endif
    return disponible;
}

void MainWindow::detenerGrabacion()
{
    grabando=false;
    relojGrabacion->stop();
    relojGrabacion->~QTimer();
    relojGrabacion = new QTimer(this);

    ui->pushButtonGrabar->setIcon(QIcon(":/Acciones/Grabar.png"));
    ui->pushButtonGrabar->setToolTip("Iniciar grabación");
    mostrarMensaje("<b style='color:red;'>Grabación detenida</b>");
    //tiempoGrabacion.currentTime();
}

void MainWindow::detenerVerGrabacion()
{
    pausarVerGrabacion();
    ui->horizontalSliderProgresoGrabacion->setVisible(false);
    ui->pushButtonCaptura->setEnabled(true);
    //ui->pushButtonGrabar->setEnabled(true);
    ui->pushButtonDetenerVerGrabacion->setEnabled(false);
}

void MainWindow::iniciarCaptura()
{
    connect(actualiza,SIGNAL(timeout()),this,SLOT(actualizaVista()));
    connect(fps,SIGNAL(timeout()),this,SLOT(calcFps()));
    fps->setInterval(250);
    actualiza->start();
    fps->start();


    ui->pushButtonCaptura->setIcon(QIcon(":/Acciones/Parar.png"));
    ui->pushButtonCaptura->setToolTip("Detener captura");
    ui->comboBoxCamaras->setEnabled(false);
    ui->pushButtonActualizaListaDeCamaras->setEnabled(false);

    capturando=true;
    ui->pushButtonCalibrar->setEnabled(true);
}

void MainWindow::detenerCaptura()
{
    actualiza->stop();
    fps->stop();
    actualiza->~QTimer();
    fps->~QTimer();
    actualiza = new QTimer(this);
    fps = new QTimer(this);
    camara.release();

    capturando=false;
    detenerGrabacion();

    ui->pushButtonCaptura->setIcon(QIcon(":/Acciones/Captura.png"));
    ui->pushButtonCaptura->setToolTip("Iniciar captura");
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

void MainWindow::segTranscurrido()
{
    segTotalGrabacion+=0.25;
    segGrabacion+=0.25;
    if(segGrabacion>60)
    {
        minGrabacion++;
        segGrabacion=0;
    }
    if(minGrabacion>60)
    {
        horasGrabacion++;
        minGrabacion=0;
    }

    ui->statusBar->showMessage("Fotogramas grabados: "+QString::number( noImgGrabadas ),500);

    //ui->timeEditTiempoGrabacion->setTime(QTime(horasGrabacion,minGrabacion,segGrabacion,0));
}

void MainWindow::guardadndoArchivos()
{
    if(indiceFrame<noImgGrabadas)
    {
        QString nombreArchivo=QString::number(indiceFrame);
        QString zeros = "";
        for(int zero=(4-nombreArchivo.length());zero>=0;zero--)
        {
            zeros+="0";
        }
        QString archivoGrabado=dir+zeros+nombreArchivo+".png";

        if(cv::imwrite(archivoGrabado.toStdString(),imgGrabadas[indiceFrame]))
        {
            ui->statusBar->showMessage("Guardando: "+archivoGrabado,500);
        }
        else
        {
            mostrarMensaje("<b style='color:red;'>error al guardar: "+archivoGrabado+"</b>");
            //noImgGrabadas=0;
            indiceFrame=0;
            conteoGuardarArchivos->stop();
            conteoGuardarArchivos->~QTimer();
            conteoGuardarArchivos = new QTimer(this);
        }
        indiceFrame++;

    }
    else
    {
        mostrarMensaje("<b>Archivos guardados</b>");

        eliminarGrabacion();

        conteoGuardarArchivos->stop();
        conteoGuardarArchivos->~QTimer();
        conteoGuardarArchivos = new QTimer(this);
    }
}

void MainWindow::calcFps()
{
    if(noFramesCap>0)
    {
        framesPerSecond=noFramesCap/0.250;
        noFramesCap=0;
    }
}

void MainWindow::actualizaVistaGrabacion()
{
    indiceVerGrabacion=ui->horizontalSliderProgresoGrabacion->value();

    cv::Mat img;
    imgGrabadas[indiceVerGrabacion].copyTo(img);
    cv::cvtColor(img,img,CV_GRAY2RGB);

    if(segTotalGrabacion>0)
    {
        int horas=  std::floor( (indiceVerGrabacion/(noImgGrabadas/segTotalGrabacion))/3600);
        int minutos= std::floor(  std::fmod((indiceVerGrabacion/(noImgGrabadas/segTotalGrabacion)),3600)  /60);
        int segundos= std::fmod((  std::fmod((indiceVerGrabacion/(noImgGrabadas/segTotalGrabacion)),3600) ),60);

        QString tiempo="";

        if(QString::number(horas).length()<2)
        {
            tiempo += "0"+QString::number(horas)+":";
        }
        else
        {
            tiempo += QString::number(horas)+":";
        }

        if(QString::number(minutos).length()<2)
        {
            tiempo += "0"+QString::number(minutos)+":";
        }
        else
        {
            tiempo += QString::number(minutos)+":";
        }

        if(QString::number(segundos).length()<2)
        {
            tiempo += "0"+QString::number(segundos);
        }
        else
        {
            tiempo += QString::number(segundos);
        }

        cv::putText(img,tiempo.toStdString(),cv::Point((img.cols/2)-35,12),cv::FONT_HERSHEY_TRIPLEX,0.5,cv::Scalar(255,255,0));
    }
    cv::putText(img,"Fotograma: "+QString::number(indiceVerGrabacion).toStdString(),cv::Point(6,12),cv::FONT_HERSHEY_TRIPLEX,0.5,cv::Scalar(255,255,0));

    QImage mostrarVideoF =  QImage((const unsigned char*)(img.data),
                                   img.cols,img.rows,QImage::Format_RGB888);

    ui->labelSalidaCam->setPixmap(QPixmap::fromImage(mostrarVideoF));

    if(indiceVerGrabacion<noImgGrabadas && navegarGrabacion==false)
    {
        ui->horizontalSliderProgresoGrabacion->setValue( ui->horizontalSliderProgresoGrabacion->value()+1 );
    }

    if(indiceVerGrabacion==noImgGrabadas-1)
    {
        pausarVerGrabacion();
        ui->horizontalSliderProgresoGrabacion->setValue(0);
    }
}

void MainWindow::actualizaVista()
{
    if(camaraDisponible())
    {
        cv::Size tamPatron(altoPatron,anchoPatron);
        std::vector<cv::Point2f> Esquinas2D;
        std::vector<cv::Point3f> Esquinas3D;
        bool patronEncontrado=false;
        cv::Mat img;
        camara.read(img);
        noFramesCap++;
        cv::cvtColor(img,img,CV_RGB2GRAY);

        if(quitarDistorcion)
        {
            cv::Mat temp = img.clone();
            cv::undistort(temp,img,matrizCamara,distCoeffs);
        }

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
            patronEncontrado = cv::findChessboardCorners(img, tamPatron, Esquinas2D,
                                                              cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE
                                                              + cv::CALIB_CB_FAST_CHECK);


            if(patronEncontrado)
            {
                cv::cornerSubPix(img, Esquinas2D, cv::Size(10, 10), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

                cv::Mat imgMuestra;
                img.copyTo(imgMuestra);

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

        cv::cvtColor(img,img,CV_GRAY2RGB);
        cv::drawChessboardCorners(img, tamPatron, cv::Mat(Esquinas2D), patronEncontrado);
        cv::putText(img,QString::number(framesPerSecond).toStdString()+" FPS",cv::Point(6,12),cv::FONT_HERSHEY_TRIPLEX,0.5,cv::Scalar(255,255,0));

        if(segGrabacion>0 || minGrabacion>0 || horasGrabacion>0)
        {
            cv::putText(img,QString::number( std::round(noImgGrabadas/segTotalGrabacion)).toStdString()+" FPS",cv::Point(6,24),cv::FONT_HERSHEY_TRIPLEX,0.5,cv::Scalar(255,255,0));
            QString tiempo="";

            if(QString::number(horasGrabacion).length()<2)
            {
                tiempo += "0"+QString::number(horasGrabacion)+":";
            }
            else
            {
                tiempo += QString::number(horasGrabacion)+":";
            }

            if(QString::number(minGrabacion).length()<2)
            {
                tiempo += "0"+QString::number(minGrabacion)+":";
            }
            else
            {
                tiempo += QString::number(minGrabacion)+":";
            }

            int seg=segGrabacion;
            if(QString::number(seg).length()<2)
            {
                tiempo += "0"+QString::number(seg);
            }
            else
            {
                tiempo += QString::number(seg);
            }

            cv::putText(img,tiempo.toStdString(),cv::Point((img.cols/2)-35,12),cv::FONT_HERSHEY_TRIPLEX,0.5,cv::Scalar(255,255,0));
        }

        int grueso=1;
        if(grabando)
        {
            tiempoCuadroGrabando++;

            if(tiempoCuadroGrabando<10)
            {
                cv::Scalar color(255,255,0);

                cv::rectangle(img,cv::Point((img.cols/2)+50,(img.rows/2)+50),cv::Point((img.cols/2)-50,(img.rows/2)-50),color,grueso);
                cv::circle(img,cv::Point(img.cols/2,img.rows/2),8,color,grueso);

                cv::line(img,cv::Point((img.cols/2)+60,(img.rows/2)),cv::Point((img.cols/2)+40,(img.rows/2)),color,grueso);
                cv::line(img,cv::Point((img.cols/2)-60,(img.rows/2)),cv::Point((img.cols/2)-40,(img.rows/2)),color,grueso);

                cv::line(img,cv::Point((img.cols/2),(img.rows/2)-60),cv::Point((img.cols/2),(img.rows/2)-40),color,grueso);
                cv::line(img,cv::Point((img.cols/2),(img.rows/2)+60),cv::Point((img.cols/2),(img.rows/2)+40),color,grueso);
            }
            else
            {
                if(tiempoCuadroGrabando>15)
                {
                    tiempoCuadroGrabando=0;
                }
            }
        }
        else
        {
            cv::Scalar color(255,255,255);

            cv::rectangle(img,cv::Point((img.cols/2)+50,(img.rows/2)+50),cv::Point((img.cols/2)-50,(img.rows/2)-50),color,grueso);
            cv::circle(img,cv::Point(img.cols/2,img.rows/2),8,color,grueso);
            cv::line(img,cv::Point((img.cols/2)+60,(img.rows/2)),cv::Point((img.cols/2)+40,(img.rows/2)),color,grueso);
            cv::line(img,cv::Point((img.cols/2)-60,(img.rows/2)),cv::Point((img.cols/2)-40,(img.rows/2)),color,grueso);

            cv::line(img,cv::Point((img.cols/2),(img.rows/2)-60),cv::Point((img.cols/2),(img.rows/2)-40),color,grueso);
            cv::line(img,cv::Point((img.cols/2),(img.rows/2)+60),cv::Point((img.cols/2),(img.rows/2)+40),color,grueso);
        }

        QImage mostrarVideoF =  QImage((const unsigned char*)(img.data),
                                       img.cols,img.rows,QImage::Format_RGB888);

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
        ui->pushButtonGrabar->setEnabled(false);
    }
    else
    {
        if(ui->comboBoxCamaras->currentIndex()!=0)
        {
            bool disponible=camaraDisponible();

            if(disponible)
            {
                QString noCamara="";
                for(int i=ui->comboBoxCamaras->currentText().length()-1;i>0-1;i--)
                {
                    if(ui->comboBoxCamaras->currentText().at(i).isDigit())
                    {
                        noCamara.prepend(ui->comboBoxCamaras->currentText().at(i));
                    }
                    else
                    {
                        break;
                    }
                }
                camara.release();
                //mostrarMensaje(noCamara);
                camara = cv::VideoCapture(noCamara.toInt());
//                //camara.set(CV_CAP_PROP_FRAME_WIDTH ,1024);
//                //camara.set(CV_CAP_PROP_FRAME_HEIGHT,768);
//                //camara.set(CV_CAP_PROP_FPS,10);
                mostrarMensaje("<b style='color:red;'>Capturando video...</b>");
                mostrarMensaje("<b>"+camaras.at(ui->comboBoxCamaras->currentIndex()-1).description()+"</b>");
                mostrarMensaje("<b>Resolución: "+QString::number(camara.get(CV_CAP_PROP_FRAME_WIDTH))+"x"+QString::number(camara.get(CV_CAP_PROP_FRAME_HEIGHT))+"</b>");
                if(camara.get(CV_CAP_PROP_FPS)>0)
                {
                    mostrarMensaje("<b>Velocidad: "+QString::number(camara.get(CV_CAP_PROP_FPS))+" FPS</b>");
                }
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
        ui->pushButtonCalibrar->setToolTip("Detener detección de patrón");
        //ui->pushButtonCalibrar->setText("Parar");
        ui->pushButtonTomarMuestra->setEnabled(true);
        ui->pushButtonTomarMuestra->setVisible(true);
    }
    else
    {
        ui->pushButtonCalibrar->setIcon(QIcon(":/Acciones/Patron.png"));
        ui->pushButtonCalibrar->setToolTip("Activar detección de patrón");
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

        //matrizCamara = cv::Mat::eye(3, 3, CV_64F);
        //distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;

        double rms = cv::calibrateCamera(coord3D, coord2D, img.size(), matrizCamara,
                                         distCoeffs, rvecs, tvecs,
                                         cv::CALIB_FIX_PRINCIPAL_POINT +
                                         cv::CALIB_FIX_ASPECT_RATIO +
                                         cv::CALIB_ZERO_TANGENT_DIST
                                         ,cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 2.22e-16));
        QString resultado ="<b>Resultado de Calibración</b><br>";
        resultado +="Error: "+QString::number(rms)+"<br>";
        resultado += "fx: "+QString::number(matrizCamara.at<double>(0,0))+"<br>";
        resultado += "fy: "+QString::number(matrizCamara.at<double>(1,1))+"<br>";
        resultado += "cx: "+QString::number(matrizCamara.at<double>(0,2))+"<br>";
        resultado += "cy: "+QString::number(matrizCamara.at<double>(1,2))+"<br>";
        resultado += "distorción: <br>";
        for(int i=0;i<8;i++)
        {
        resultado+=QString::number(distCoeffs.at<double>(i,0))+"<br>";
        }
        mostrarMensaje(resultado);
    }
    else
    {
        QMessageBox::information(this,"Muestras insuficientes","Es necesario capturar por lo menos 10 muestras para calibrar",QMessageBox::Ok);
    }
}

void MainWindow::on_pushButtonDist_clicked()
{
    quitarDistorcion=!quitarDistorcion;
    if(quitarDistorcion)
    {
        mostrarMensaje("<p>Se han <b style='color:red;display:inline;'>activado</b> los coeficientes de distorción</p>");
        quitarDistorcion=true;
        ui->pushButtonDist->setIcon(QIcon(":/Acciones/NoDistorcion.png"));
        ui->pushButtonDist->setToolTip("Desactivar coeficientes de distorción");
    }
    else
    {
        mostrarMensaje("<p>Se han <b style='color:red;display:inline;'>desactivado</b> los coeficientes de distorción</p>");
        quitarDistorcion=false;
        ui->pushButtonDist->setIcon(QIcon(":/Acciones/Distorcion.png"));
        ui->pushButtonDist->setToolTip("Activar coeficientes de distorción");
    }
}

void MainWindow::on_pushButtonActualizaListaDeCamaras_clicked()
{
    actualizaListaDeCamaras();
}

void MainWindow::on_pushButtonGrabar_clicked()
{
    grabando=!grabando;
    if(grabando)
    {
        ui->pushButtonGrabar->setIcon(QIcon(":/Acciones/PararGrabacion.png"));
        ui->pushButtonGrabar->setToolTip("Detener grabación");
        mostrarMensaje("<b style='color:red;'>Grabando...</b>");


        connect(relojGrabacion,SIGNAL(timeout()),this,SLOT(segTranscurrido()));
        relojGrabacion->setInterval(250);
        relojGrabacion->start();

        ui->pushButtonEliminarGrabacion->setEnabled(true);
        ui->pushButtonGuardarGrabacion->setEnabled(true);

        ui->pushButtonVerGrabacion->setEnabled(true);
    }
    else
    {
        detenerGrabacion();
    }
}

void MainWindow::on_pushButtonVerMuestras_clicked()
{
    if(capturando)
    {
        detenerCaptura();
    }
    dialogVerMuestras->ponIndice(0);
    dialogVerMuestras->leerImagenesMuestra(imagenesMustra,noMuestrasCalib);
    dialogVerMuestras->exec();
}

void MainWindow::on_pushButtonEliminarGrabacion_clicked()
{
    eliminarGrabacion();
}

void MainWindow::on_pushButtonGuardarMuestras_clicked()
{
    if(noMuestrasCalib>0)
    {
        if(noMuestrasCalib<50)
        {
            detenerCaptura();
            QString dir=QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).last();
            QString direccion=QFileDialog::getExistingDirectory(this,"Seleciona la ubicación para guardar",dir);
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

void MainWindow::on_pushButtonConfigCalib_clicked()
{
    if(noMuestrasCalib>0)
    {
        QMessageBox::warning(this,"Alerta","Si se cambia la configuración, se eliminarán las muestras obtenidas para evitar errores de calibración.",QMessageBox::Ok);
    }
    detenerCaptura();
    dialogConfigCalib->ponerTamanioCuadro(tamCuadroCalib);
    dialogConfigCalib->ponerTamanioPatron(altoPatron,anchoPatron);

//    distCoeffs.at<double>(i,0)

    dialogConfigCalib->ponerFx(matrizCamara.at<double>(0,0));
    dialogConfigCalib->ponerFy(matrizCamara.at<double>(1,1));
    dialogConfigCalib->ponerCx(matrizCamara.at<double>(0,2));
    dialogConfigCalib->ponerCy(matrizCamara.at<double>(1,2));

    dialogConfigCalib->ponerCorrecX1(distCoeffs.at<double>(0,0));
    dialogConfigCalib->ponerCorrecX2(distCoeffs.at<double>(1,0));
    dialogConfigCalib->ponerCorrecX3(distCoeffs.at<double>(2,0));
    dialogConfigCalib->ponerCorrecX4(distCoeffs.at<double>(3,0));

    dialogConfigCalib->ponerCorrecY1(distCoeffs.at<double>(4,0));
    dialogConfigCalib->ponerCorrecY2(distCoeffs.at<double>(5,0));
    dialogConfigCalib->ponerCorrecY3(distCoeffs.at<double>(6,0));
    dialogConfigCalib->ponerCorrecY4(distCoeffs.at<double>(7,0));

    dialogConfigCalib->exec();
    if(dialogConfigCalib->result())
    {
        tamCuadroCalib=dialogConfigCalib->obtenerTamCuadro();
        altoPatron=dialogConfigCalib->obtenerAltoPatron();
        anchoPatron=dialogConfigCalib->obtenerAnchoPatron();

        matrizCamara.at<double>(0,0)=dialogConfigCalib->obtenerFx();
        matrizCamara.at<double>(1,1)=dialogConfigCalib->obtenerFy();
        matrizCamara.at<double>(0,2)=dialogConfigCalib->obtenerCx();
        matrizCamara.at<double>(1,2)=dialogConfigCalib->obtenerCy();

        distCoeffs.at<double>(0,0)=dialogConfigCalib->obtenerCorrecX1();
        distCoeffs.at<double>(1,0)=dialogConfigCalib->obtenerCorrecX2();
        distCoeffs.at<double>(2,0)=dialogConfigCalib->obtenerCorrecX3();
        distCoeffs.at<double>(3,0)=dialogConfigCalib->obtenerCorrecX4();

        distCoeffs.at<double>(4,0)=dialogConfigCalib->obtenerCorrecY1();
        distCoeffs.at<double>(5,0)=dialogConfigCalib->obtenerCorrecY2();
        distCoeffs.at<double>(6,0)=dialogConfigCalib->obtenerCorrecY3();
        distCoeffs.at<double>(7,0)=dialogConfigCalib->obtenerCorrecY4();

        if(noMuestrasCalib>0)
        {

        }
        mostrarMensaje("<b style='color:red;'>Se ha guardado la nueva configuración.</b>");
    }
    else
    {
        mostrarMensaje("<b>No se hicieron cambios en la configuración.</b>");
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
    QMessageBox acercaDe;
    acercaDe.setTextFormat(Qt::RichText);
    acercaDe.setWindowTitle("Acerca de este software");
    acercaDe.setIconPixmap(QPixmap(":/Acciones/QK_Logo.png"));
    acercaDe.setText("Este software está bajo la licencia <a href='https://www.gnu.org/licenses/lgpl-3.0.en.html'>GNU LGPLv3</a>. El código fuente está disponible en <a href='https://github.com/CharlesVD/QKalib'>https://github.com/CharlesVD/QKalib</a>.<br><br>Desarrollado por: Charles Fernando Velázquez Dodge<br>Email: charles.fvd@gmail.com");
    acercaDe.exec();
}

void MainWindow::on_pushButtonGuardarGrabacion_clicked()
{
    if(noImgGrabadas>0)
    {
        mostrarMensaje("<b>Guardando archivos...</b>");
        indiceFrame=0;
        QString dirIni=QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).last();
        dir = QFileDialog::getExistingDirectory(this,"Seleciona la ubicacion para grabar",dirIni)+"/";

        connect(conteoGuardarArchivos,SIGNAL(timeout()),this,SLOT(guardadndoArchivos()));
        conteoGuardarArchivos->start();
    }
    else
    {
        QMessageBox::information(this,"Información insuficiente","No hay fotogramas para guardar.",QMessageBox::Ok);
    }
}

void MainWindow::on_actionVer_Panel_triggered()
{
    if(ui->actionVer_Panel->isChecked())
    {
        ui->dockWidgetPanelPrincipal->setVisible(true);
        ui->actionVer_herramientas_de_calibracion->setVisible(true);
        ui->actionVer_informacion->setVisible(true);
    }
    else
    {
        ui->dockWidgetPanelPrincipal->setVisible(false);
        ui->actionVer_herramientas_de_calibracion->setVisible(false);
        ui->actionVer_informacion->setVisible(false);
    }
}

void MainWindow::on_actionVer_herramientas_de_calibracion_triggered()
{
    if(ui->actionVer_herramientas_de_calibracion->isChecked())
    {
        ui->groupBoxCalibracion->setVisible(true);
    }
    else
    {
        ui->groupBoxCalibracion->setVisible(false);
    }
}

void MainWindow::on_actionVer_informacion_triggered()
{
    if(ui->actionVer_informacion->isChecked())
    {
        ui->groupBoxInformacion->setVisible(true);
        ui->frameEspacio->setVisible(false);
    }
    else
    {
        ui->groupBoxInformacion->setVisible(false);
        ui->frameEspacio->setVisible(true);
    }
}

void MainWindow::on_pushButtonVerGrabacion_clicked()
{
    if( !reproduciendoGrabacion )
    {

        if(capturando)
        {
            detenerCaptura();
        }
        ui->pushButtonCaptura->setEnabled(false);
        ui->pushButtonGrabar->setEnabled(false);
        ui->pushButtonDetenerVerGrabacion->setEnabled(true);
        ui->horizontalSliderProgresoGrabacion->setVisible(true);

        ui->pushButtonVerGrabacion->setIcon(QIcon(":/Acciones/PausarGrabacion.png"));
        ui->pushButtonVerGrabacion->setToolTip("Pausar grabación");
        reproduciendoGrabacion=true;
        ui->horizontalSliderProgresoGrabacion->setMaximum(noImgGrabadas-1);
        connect(actualizaVerGrabacion,SIGNAL(timeout()),this,SLOT(actualizaVistaGrabacion()));
        actualizaVerGrabacion->setInterval( (1/framesPerSecond)*1000);
        actualizaVerGrabacion->start();
    }
    else
    {
        pausarVerGrabacion();
    }
}

void MainWindow::pausarVerGrabacion()
{
    ui->pushButtonVerGrabacion->setIcon(QIcon(":/Acciones/ReproducirGrabacion.png"));
    ui->pushButtonVerGrabacion->setToolTip("Reproducir grabación");
    reproduciendoGrabacion=false;
    actualizaVerGrabacion->stop();
    actualizaVerGrabacion->~QTimer();
    actualizaVerGrabacion=new QTimer(this);
}

void MainWindow::on_horizontalSliderProgresoGrabacion_sliderPressed()
{
    navegarGrabacion=true;
}

void MainWindow::on_horizontalSliderProgresoGrabacion_sliderReleased()
{
    navegarGrabacion=false;
    if(ui->horizontalSliderProgresoGrabacion->value()==ui->horizontalSliderProgresoGrabacion->maximum())
    {
        ui->horizontalSliderProgresoGrabacion->setValue(0);
    }
}

void MainWindow::on_pushButtonDetenerVerGrabacion_clicked()
{
    detenerVerGrabacion();
}

void MainWindow::on_horizontalSliderProgresoGrabacion_sliderMoved(int position)
{
    indiceVerGrabacion=position;

    cv::Mat img;
    imgGrabadas[indiceVerGrabacion].copyTo(img);
    cv::cvtColor(img,img,CV_GRAY2RGB);

    if(segTotalGrabacion>0)
    {
        int horas=  std::floor( (indiceVerGrabacion/(noImgGrabadas/segTotalGrabacion))/3600);
        int minutos= std::floor(  std::fmod((indiceVerGrabacion/(noImgGrabadas/segTotalGrabacion)),3600)  /60);
        int segundos= std::fmod((  std::fmod((indiceVerGrabacion/(noImgGrabadas/segTotalGrabacion)),3600) ),60);

        QString tiempo="";

        if(QString::number(horas).length()<2)
        {
            tiempo += "0"+QString::number(horas)+":";
        }
        else
        {
            tiempo += QString::number(horas)+":";
        }

        if(QString::number(minutos).length()<2)
        {
            tiempo += "0"+QString::number(minutos)+":";
        }
        else
        {
            tiempo += QString::number(minutos)+":";
        }

        if(QString::number(segundos).length()<2)
        {
            tiempo += "0"+QString::number(segundos);
        }
        else
        {
            tiempo += QString::number(segundos);
        }

        cv::putText(img,tiempo.toStdString(),cv::Point((img.cols/2)-35,12),cv::FONT_HERSHEY_TRIPLEX,0.5,cv::Scalar(255,255,0));
    }
    cv::putText(img,"Fotograma: "+QString::number(indiceVerGrabacion).toStdString(),cv::Point(6,12),cv::FONT_HERSHEY_TRIPLEX,0.5,cv::Scalar(255,255,0));

    QImage mostrarVideoF =  QImage((const unsigned char*)(img.data),
                                   img.cols,img.rows,QImage::Format_RGB888);

    ui->labelSalidaCam->setPixmap(QPixmap::fromImage(mostrarVideoF));

}
