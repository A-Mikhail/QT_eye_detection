/*
   “Copyright 2015 Адаменко Михаил”

    This file is part of EyeDetection.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Alizee.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtWidgets>
#include <QWidgetAction>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "videoInput.h"
#include "constants.h"

#include "findeyecenter.h"

using namespace std;
using namespace cv;

void detectAndDisplay(Mat frame);

// Подключение haarcascade 
String face_cascade_name = "C:\\Users\\micha_000\\Desktop\\Pr.Alizee\\AlizeeQt\\haarcascade_frontalface_default.xml";
String eye_cascade_name = "C:\\Users\\micha_000\\Desktop\\Pr.Alizee\\AlizeeQt\\haarcascade_eye.xml";

CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

RNG rng(12345);
Mat debugImage;
Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);


/*
* MainWindow::MainWindow
* Главная функция файла mainwindow.cpp
* Открытие веб-камеры, сообщение об ошибки, инициализация таймера.
*/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Сообщения об отсутствии face_cascade или eye_cascade
    if( !face_cascade.load(face_cascade_name) ) {QMessageBox::critical(this,
        "Ошибка!", "Ошибка загрузки face_cascade", QMessageBox::Ok); return; };

    if( !eye_cascade.load(eye_cascade_name) ){QMessageBox::critical(this,
        "Ошибка!", "Ошибка загрузки eye_cascade", QMessageBox::Ok); return; };

    ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2),
            43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);

    /*-------- Создание меню на главной форме --------*/

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    layout->setMargin(0);
    widget->setStyleSheet("background-color: #151515; height: 30%;");

    // Кнопка "Меню"
    mainButton = new QPushButton("Меню", widget);
    mainButton->setStyleSheet("color: #fff");
    layout->addWidget(mainButton);

    // Кнопка "Помощь"
    helpButton = new QPushButton("Помощь", widget);
    helpButton->setStyleSheet("color: #fff");
    layout->addWidget(helpButton);

    // Выпадающий список на виджете
    combobox = new QComboBox(widget);
    combobox->setStyleSheet("color: #fff");
    layout->addWidget(combobox);

    setMenuWidget(widget);


    /*-------- Получение списка веб-камер использую библиотеку videoinput --------*/

    // Список подключенных веб-камер
    int numDevices = videoInput::listDevices();

    // Вывод списка веб-камер в comboBox
    for(int i = 0; i < numDevices; i++){
        combobox->addItem(videoInput::getDeviceName(i));
    }

    // Получение индекса выбранной веб-камеры
    connect(combobox, SIGNAL(activated(QString)), this, SLOT(webcamIndex()));
}

void MainWindow::webcamIndex()
{
    int webcamIndexText = combobox->currentIndex();

    // Открытие первой веб-камеры
    capWebCam.open(webcamIndexText);

    // Сообщение об отсутствии веб-камеры
    if (!capWebCam.isOpened())
    {
        QMessageBox::warning(this, "Ошибка!", "Камера не найдена");

        return; // выход

    } else {

        // Инициализация таймера с обновлением в 120мс.
        tmrTimer = new QTimer(this);
        connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
        tmrTimer->start(120);

    }

    qDebug() << webcamIndexText;
}

/*
* MainWindow::processFrameAndUpdateGUI
* Функция отображения изображения с веб-камеры в lblWebCam
*/
void MainWindow::processFrameAndUpdateGUI()
{
    capWebCam.read(frame);

    QImage qimgOriginal((uchar*)frame.data, frame.cols, frame.rows,
                        frame.step, QImage::Format_RGB888);

    // зеркальное отображение
    flip(frame, frame, 1);

    cvtColor(frame, frame, CV_BGR2RGB);

    if (!frame.empty())
    {
        detectAndDisplay(frame);
    } else {
        QMessageBox::warning(this, "Ошибка!", "Камера не найдена");

        this -> close();
    }

    ui -> lblWebCam -> setPixmap(QPixmap::fromImage(qimgOriginal));
}

/*
* MainWindow::findEyes
* Функция отображения изображения с веб-камеры в lblWebCam
*/
void MainWindow::findEyes(Mat frame_gray, Rect face)
{
    findEyeCenter fecenter;

    Mat faceROI = frame_gray(face);
    Mat debugFace = faceROI;

    if (smoothFaceImage)
    {
        double sigma = smoothFaceFactor * face.width;
        GaussianBlur(faceROI, faceROI, Size(0, 0), sigma);
    }

    // Поиск региона глаз и отрисовка
    int eye_region_width    = face.width  * (eyePercentHeight / 100.0);
    int eye_region_height   = face.height * (eyePercentHeight / 100.0);
    int eye_region_top      = face.height * (eyePercentTop / 100.0);

    Rect leftEyeRegion(face.width * (eyePercentSide / 100.0),
                        eye_region_top, eye_region_width, eye_region_height);

    Rect rightEyeRegion(face.width - eye_region_width - face.width * (eyePercentSide / 100.0),
                        eye_region_top, eye_region_width, eye_region_height);

    // Поиск центра глаза
    Point leftPupil = fecenter.eyeCenter(faceROI, leftEyeRegion);
    Point rightPupil = fecenter.eyeCenter(faceROI, rightEyeRegion);

    // Смена координат центра глаз относительно лица
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;

    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;

    // Отрисовка центра глаз
    circle(debugFace, rightPupil, 1, 1234);
    circle(debugFace, leftPupil, 1, 1234);
}

/*
* MainWindow::findSkin
* Функция отображения изображения с веб-камеры в lblWebCam
*/
Mat MainWindow::findSkin(Mat &frame)
{
    Mat input;
    Mat output = Mat(frame.rows, frame.cols, CV_8U);

    cvtColor(frame, input, CV_BGR2YCrCb);

    for (int y = 0; y < input.rows; ++y)
    {
        const Vec3b *Mr = input.ptr<Vec3b>(y);
        Vec3b *Or = frame.ptr<Vec3b>(y);

        for (int x = 0; x < input.cols; ++x)
        {
            Vec3b ycrcb = Mr[x];

            if (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) == 0)
            {
                Or[x] = Vec3b(0, 0, 0);
            }
        }
    }

    return output;
}

/*
* MainWindow::detectAndDisplay
* Функция отображения изображения с веб-камеры в lblWebCam
*/
void MainWindow::detectAndDisplay(Mat frame)
{
    vector<Rect> faces;

    vector<Mat> rgbChannels(3);

    split(frame, rgbChannels);

    face_cascade.detectMultiScale(frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(150, 150));

    /*for (int i = 0; i < faces.size(); i++)
    {
        rectangle(frame_gray, faces[i], 1234);
    }*/

    if (faces.size() > 0)
    {
        findEyes(frame, faces[0]);
    }
}

/*
 * MainWindow::on_action_about_triggered
 * Функция вывода окна about по нажатию кнопки "О программе" в диалоговом меню "Помощь"
 */
void MainWindow::on_action_about_triggered()
{
    aboutwindow = new aboutWindow();
    aboutwindow->show();
}

/*
* MainWindow::on_action_settings_triggered
* Функция вывода окна options по нажатию кнопки "Настройки" в диалоговом меню "Меню"
*/
void MainWindow::on_action_settings_triggered()
{
    optionswindow = new optionsWindow();
    optionswindow->show();
}

/*
* MainWindow::on_action_exit_triggered
* Функция выхода по нажатию кнопки "Выход" в диалоговом меню "Меню"
*/
void MainWindow::on_action_exit_triggered()
{
    this->close();
}

/*
* MainWindow::~MainWindow()
* Удаление Ui
*/
MainWindow::~MainWindow()
{
    delete ui;
}

