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
    along with EyeDetection.  If not, see <http://www.gnu.org/licenses/>.
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

CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

findEyeCenter fecenter;

int filenumber;
string filename;

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

    /*-------- haarcascade --------*/

    // Путь к haarcascade
    QString face_cascade_path = QString(QCoreApplication::applicationDirPath() + "/haarcascade/haarcascade_frontalface_default.xml");
    QString eye_cascade_path = QString(QCoreApplication::applicationDirPath() + "/haarcascade/haarcascade_eye.xml");

    String face_cascade_utf8 = face_cascade_path.toUtf8().constData();
    String eye_cascade_utf8 = eye_cascade_path.toUtf8().constData();

    // Подключение haarcascade
    String face_cascade_name   = face_cascade_utf8;
    String eye_cascade_name    = eye_cascade_utf8;

    // Сообщения об отсутствии face_cascade или eye_cascade
    if( !face_cascade.load(face_cascade_name) ) {QMessageBox::critical(this,
        "Ошибка!", "Ошибка загрузки face_cascade", QMessageBox::Ok); return; };

    if( !eye_cascade.load(eye_cascade_name) ){QMessageBox::critical(this,
        "Ошибка!", "Ошибка загрузки eye_cascade", QMessageBox::Ok); return; };

    /*-------- Создание меню на главной форме --------*/

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    layout->setMargin(0);
    widget->setStyleSheet("background-color: #151515; height: 30%;");

    // Кнопка "Выход"
    exitButton = new QPushButton("Выход", widget);
    exitButton->setStyleSheet("color: #fff");

    connect(exitButton, SIGNAL(clicked()), this, SLOT(on_action_exit_triggered()));

    layout->addWidget(exitButton);

    // Кнопка "Настройки"
    settingsButton = new QPushButton("Настройки", widget);
    settingsButton->setStyleSheet("color: #fff");

    connect(settingsButton, SIGNAL(clicked()), this, SLOT(on_action_settings_triggered()));

    layout->addWidget(settingsButton);

    // Кнопка "О программе"
    helpButton = new QPushButton("О программе", widget);
    helpButton->setStyleSheet("color: #fff");

    connect(helpButton, SIGNAL(clicked()), this, SLOT(on_action_about_triggered()));

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

/*
* MainWindow::processFrameAndUpdateGUI
* Открытие веб-камеры по индексу, запуск таймера
*/
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
        // Инициализация таймера с обновлением в 25fps.
        tmrTimer = new QTimer(this);
        connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
        tmrTimer->start(1000/25);
    }
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

    // стандартные цвета на фрейм веб-камеры
    cvtColor(frame, frame, CV_BGR2RGB);

    if (!frame.empty())
    {
        detectAndDisplay(frame);

    } else {
        QMessageBox::warning(this, "Ошибка!", "Камера не найдена");

        this->close();
    }

    ui->lblWebCam->setPixmap(QPixmap::fromImage(qimgOriginal));
}

/*
* MainWindow::findEyes
* Функция поиска глаз
*/
void MainWindow::findEyes(Mat frame_gray, Rect face)
{
    Mat faceROI = frame_gray(face);

    // Поиск региона глаз и отрисовка
    int eye_region_width    = face.width  * (eyePercentHeight / 100.0);
    int eye_region_height   = face.height * (eyePercentHeight / 100.0);
    int eye_region_top      = face.height * (eyePercentTop / 100.0);

    Rect leftEyeRegion(face.width * (eyePercentSide / 100.0),
                        eye_region_top, eye_region_width, eye_region_height);

    Rect rightEyeRegion(face.width - eye_region_width - face.width * (eyePercentSide / 100.0),
                        eye_region_top, eye_region_width, eye_region_height);

    // Поиск центра глаза
    fecenter.eyeCenter(faceROI, leftEyeRegion);
    fecenter.eyeCenter(faceROI, rightEyeRegion);
}

/*
* MainWindow::detectAndDisplay
* Функция отображения изображения с веб-камеры в lblWebCam
*/
void MainWindow::detectAndDisplay(Mat frame)
{
    vector<Rect> faces;
    vector<Rect> eyes;
    vector<Mat> rgbChannels(3);

    split(frame, rgbChannels);

    // Обнаружение лица
    face_cascade.detectMultiScale(frame, faces, 1.1, 2,
                                  0 |CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(150, 150));


    // Обнаружение глаз
    eye_cascade.detectMultiScale(frame, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(30, 30) );

    // отображение прямоугольника лица
    /*for (int i = 0; i < faces.size(); i++)
    {
        rectangle(frame, faces[i], 1234);
    }*/

    if (faces.size() > 0)
    {
        findEyes(frame, faces[0]);

        Mat crop;
        Mat gray;

        // Set Region of Interest
        cv::Rect roi_b;
        cv::Rect roi_c;

        size_t index_current = 0; // index of current element
        int area_current = 0; // area of current element

        size_t index_biggest_element = 0; // index of biggest element
        int area_biggest_element = 0; // area of biggest element


        // Сохранение найденной области в .png файл
        for (index_current = 0; index_current < eyes.size(); index_current++) // Iterate through all current elements (detected faces)
        {
            roi_c.x = eyes[index_current].x;
            roi_c.y = eyes[index_current].y;
            roi_c.width = (eyes[index_current].width);
            roi_c.height = (eyes[index_current].height);

            area_current = roi_c.width * roi_c.height; // Get the area of current element (detected face)

            roi_b.x = eyes[index_biggest_element].x;
            roi_b.y = eyes[index_biggest_element].y;
            roi_b.width = (eyes[index_biggest_element].width);
            roi_b.height = (eyes[index_biggest_element].height);

            area_biggest_element = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

            if (area_current > area_biggest_element)
            {
                index_biggest_element = index_current;
                roi_b.x = eyes[index_biggest_element].x;
                roi_b.y = eyes[index_biggest_element].y;
                roi_b.width = (eyes[index_biggest_element].width);
                roi_b.height = (eyes[index_biggest_element].height);
            }

            crop = frame(roi_b);
            cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

            // Form a filename
            filename = "";
            stringstream ssfn;
            ssfn << filenumber << ".png";
            filename = ssfn.str();
            filenumber++;

            // Сохранение в .png формате
            imwrite(filename, gray);
        }

        qDebug("1");

    } else {
        qDebug("0");
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

