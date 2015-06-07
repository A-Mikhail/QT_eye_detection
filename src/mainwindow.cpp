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

// рассчитать хеш картинки
__int64 calcImageHash(IplImage* image, bool show_results = false);
// рассчёт расстояния Хэмминга
__int64 calcHammingDistance(__int64 x, __int64 y);

CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

string originalImage;
string temporaryImage;

string folderName;
string temporaryFolderName;

string folderCreateCommand;

Mat crop;
Mat gray;

int filenumber = 0;


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

    // Обнаружение лица
    face_cascade.detectMultiScale(frame, faces, 1.1, 2,
                                  0 |CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(150, 150));
    if (faces.size() > 0)
    {
        findEyes(frame, faces[0]); // отображение контура области глаз

        croppedROI(frame);
    }
}

/*
* MainWindow::croppedROI(Mat frame)
* Сохранение ROI в папке "cropped"
*/
void MainWindow::croppedROI(Mat frame)
{
    // проверка пользователя
    if (QFile::exists("user.dat") ) {

        // Создание папки "cropped"
        if(!QDir("cropped").exists())
        {
            QDir().mkdir("cropped");
        }

        // Создание папки "temporary"
        if(!QDir("temporary").exists())
        {
            QDir().mkdir("temporary");
        }

        vector<Rect> eyes;

        // Обнаружение глаз
        eye_cascade.detectMultiScale(frame, eyes, 1.1, 2,
                                     0 |CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(30, 30));

        // Установка региона интереса
        cv::Rect roi_b;
        cv::Rect roi_c;

        size_t index_current = 0; // индекс текущего элемента
        int area_current = 0; // площадь текущего элемента

        size_t index_biggest_element = 0; // индекс большего элемента
        int area_biggest_element = 0; // площадь большего элемента

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

            originalImage = "";
            folderName = "cropped";

            // Чтение из файла
            QFile file("user.dat");
            file.open(QIODevice::ReadOnly);
            QDataStream in(&file);
            QString profileName;
            in >> profileName;

            // Перевод из QString в String
            String profileName_utf8 = profileName.toUtf8().constData();

            stringstream ssoriginalImage;
            ssoriginalImage << folderName << "/" << profileName_utf8 << "-" << filenumber << "_eye" << ".png";

            originalImage = ssoriginalImage.str();

            filenumber++;

            // Остановить сохранение при 10 файлах
            if(filenumber > 10)
            {
                break;
            } else {
                imwrite(originalImage, gray);

                IplImage *original = 0;

                char* original_name[] = new char[10];

                char* newOriginal_name = new char[11];

                for(int i = 0; i < 10; i++) newOriginal_name[i] = original[i];
                // имя объекта задаётся первым параметром
                char* original_filename = original_name;

                // получаем картинку
                original = cvLoadImage(original_filename, 1);

                if(!original){
                        qDebug() << "[!] Error: cant load object image: %s\n" << original_filename;
                        return;
                }

                // построим хэш
                __int64 hashO = calcImageHash(original, true);

                // освобождаем ресурсы
                cvReleaseImage(&original);

                return;
            }
        }
    }
}

/*
void MainWindow::createTemporaryObject()
{
    qDebug() << "Создание папки и файла";

    temporaryImage = "";
    temporaryFolderName = "temporary";

    stringstream sstemporaryImage;
    sstemporaryImage << temporaryFolderName << "/" << "Temporary_eye" << ".png";

    temporaryImage = sstemporaryImage.str();

    imwrite(temporaryImage, gray);
}
*/


/*
 * __int64 calcHammingDistance(__int64 x, __int64 y)
 * рассчитать хеш картинки
*/
__int64 calcImageHash(IplImage* src, bool show_results)
{
        IplImage *res=0, *gray=0, *bin =0;

        res = cvCreateImage( cvSize(8, 8), src->depth, src->nChannels);
        gray = cvCreateImage( cvSize(8, 8), IPL_DEPTH_8U, 1);
        bin = cvCreateImage( cvSize(8, 8), IPL_DEPTH_8U, 1);

        // уменьшаем картинку
        cvResize(src, res);
        // переводим в градации серого
        cvCvtColor(res, gray, CV_BGR2GRAY);
        // вычисляем среднее
        CvScalar average = cvAvg(gray);

        qDebug() << "[i] average: " << average.val[0];

        // получим бинарное изображение относительно среднего
        // для этого воспользуемся пороговым преобразованием
        cvThreshold(gray, bin, average.val[0], 255, CV_THRESH_BINARY);

        // построим хэш
        __int64 hash = 0;

        int i=0;

        // пробегаемся по всем пикселям изображения
        for( int y=0; y<bin->height; y++ ) {
                uchar* ptr = (uchar*) (bin->imageData + y * bin->widthStep);
                for( int x=0; x<bin->width; x++ ) {
                        // 1 канал
                        if(ptr[x]){
                                // hash |= 1<<i;  // warning C4334: '<<' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
                                hash |= 1<<i;
                        }
                        i++;
                }
        }

        qDebug() << "hash: " << hash;

        return hash;
}

/*
 * __int64 calcHammingDistance(__int64 x, __int64 y)
 * рассчёт расстояния Хэмминга между двумя хэшами

__int64 calcHammingDistance(__int64 x, __int64 y)
{
        __int64 dist = 0, val = x ^ y;

        // Count the number of set bits
        while(val)
        {
                ++dist;
                val &= val - 1;
        }

        return dist;
}
*/

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
