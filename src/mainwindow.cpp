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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "videoInput.h"
#include "constants.h"

#include "findeye.h"

using namespace std;
using namespace cv;

void detectAndDisplay(Mat frame);

CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

string filename;
int filenumber = 0;

string folderName;
string folderCreateCommand;

/*
* MainWindow::MainWindow
* Главная функция файла mainwindow.cpp
* Открытие веб-камеры, сообщение об ошибки, инициализация таймера.
*/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*-------- CSS --------*/

    QFile style;

    style.setFileName(":/qss/style.css");
    style.open(QFile::ReadOnly);
    QString qssStr = style.readAll();

    qApp->setStyleSheet(qssStr);

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
        "Ошибка!", "Ошибка загрузки face_cascade", QMessageBox::Ok); return;};

    if( !eye_cascade.load(eye_cascade_name) ){QMessageBox::critical(this,
        "Ошибка!", "Ошибка загрузки eye_cascade", QMessageBox::Ok); return;};

    /*-------- Создание меню на главной форме --------*/

    widget = new QWidget(this);
    layout = new QHBoxLayout(widget);

    layout->setSpacing(0); // Убрать пробелы
    layout->setMargin(0); // Убрать отсутпы

    widget->setObjectName("widget");

    // Кнопка "Выход"
    exitButton = new QPushButton("Выход", widget);
    exitButton->setObjectName("exitButton");

    connect(exitButton, SIGNAL(clicked()), this, SLOT(on_action_exit_triggered()));

    layout->addWidget(exitButton);

    // Кнопка "Настройки"
    settingsButton = new QPushButton("Настройки", widget);
    settingsButton->setObjectName("settingsButton");

    connect(settingsButton, SIGNAL(clicked()), this, SLOT(on_action_settings_triggered()));

    layout->addWidget(settingsButton);

    // Кнопка "О программе"
    aboutButton = new QPushButton("О программе", widget);
    aboutButton->setObjectName("aboutButton");

    connect(aboutButton, SIGNAL(clicked()), this, SLOT(on_action_about_triggered()));

    layout->addWidget(aboutButton);

    // Поле "Веб-камера"
    webcamLabel = new QLabel("Веб-камера: ", widget);
    webcamLabel->setObjectName("webcamLabel");

    layout->addWidget(webcamLabel);

    // Выпадающий список на виджете
    combobox = new QComboBox(widget);

    layout->addWidget(combobox);

    setMenuWidget(widget); // Установка меню

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
    }

    ui->lblWebCam->setPixmap(QPixmap::fromImage(qimgOriginal));
}

/*
* MainWindow::findEyes
* Функция поиска глаз
*/
void MainWindow::findEyes(Mat frame_gray, Rect face)
{
    findEye findeye;

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
    findeye.eyeCenter(faceROI, leftEyeRegion);
    findeye.eyeCenter(faceROI, rightEyeRegion);
}

/*
* MainWindow::detectAndDisplay
* Определение и отображение контура глаз/лица на фрейме
*/
void MainWindow::detectAndDisplay(Mat frame)
{
    vector<Rect> faces;
    vector<Rect> eyes;

    // Обнаружение лица
    face_cascade.detectMultiScale(frame, faces, 1.1, 2,
                                  0 |CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(150, 150));
    // Обнаружение глаз
    eye_cascade.detectMultiScale(frame, eyes, 1.1, 2,
                                 0 |CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(30, 30));

    if (faces.size() > 0)
    {
        findEyes(frame, faces[0]); // отображение контура области глаз

        croppedROI();
    }
}

void MainWindow::croppedROI()
{
    if (!QFile::exists("user.dat") ) {

        Mat crop;
        Mat gray;

        // Установка региона интереса
        cv::Rect roi_b;
        cv::Rect roi_c;

        size_t index_current = 0; // индекс текущего элемента
        int area_current = 0; // площадь текущего элемента

        size_t index_biggest_element = 0; // индекс большего элемента
        int area_biggest_element = 0; // площадь большего элемента

        // Создание папки cropped
        if(!QDir("cropped").exists())
        {
            QDir().mkdir("cropped");
        }

        // Сохранение найденной области в .png файл
        for (index_current = 0; index_current < eyes.size(); index_current++)
        {
            roi_c.x = eyes[index_current].x;
            roi_c.y = eyes[index_current].y;
            roi_c.width = (eyes[index_current].width);
            roi_c.height = (eyes[index_current].height);

            area_current = roi_c.width * roi_c.height; // Получить площадь текущего элемента

            roi_b.x = eyes[index_biggest_element].x;
            roi_b.y = eyes[index_biggest_element].y;
            roi_b.width = (eyes[index_biggest_element].width);
            roi_b.height = (eyes[index_biggest_element].height);

            area_biggest_element = roi_b.width * roi_b.height; // Получить площадь большего элемента

            if (area_current > area_biggest_element)
            {
                index_biggest_element = index_current;
                roi_b.x = eyes[index_biggest_element].x;
                roi_b.y = eyes[index_biggest_element].y;
                roi_b.width = (eyes[index_biggest_element].width);
                roi_b.height = (eyes[index_biggest_element].height);
            }

            /*-------- Сохранение ROI в папку cropped --------*/

            crop = frame(roi_b);
            cvtColor(crop, gray, CV_BGR2GRAY); // Конвертация вырезанного изображения в серый цвет

            filename = "";
            folderName = "cropped";

            // Чтение из файла
            QFile file("user.dat");
            file.open(QIODevice::ReadOnly);
            QDataStream in(&file);
            QString profileName;
            in >> profileName;

            filenumber++;

            // Остановить сохранение при 10 файлах
            if(filenumber > 10)
            {
                break;
            } else {
                stringstream ssfilename;
                ssfilename << folderName << "/" << profileName << "-" << filenumber << "_eye" << ".png";

                filename = ssfilename.str();

                imwrite(filename, gray);
            }
        }
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
