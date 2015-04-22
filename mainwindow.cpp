/*
   “Copyright 2015 Адаменко Михаил”

    This file is part of Alizee.

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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutwindow.h"

#include "constants.h"
#include "findeyecenter.h"
#include "findeyecorner.h"

using namespace std;
using namespace cv;

// Подключение haarcascade 
String face_cascade_name = "C:\\Users\\micha_000\\Desktop\\Pr.Alizee\\AlizeeQt\\haarcascade_face_alt2.xml";
String eye_cascade_name = "C:\\Users\\micha_000\\Desktop\\Pr.Alizee\\AlizeeQt\\haarcascade_eyes.xml";

CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

void detectAndDisplay(Mat frame);

RNG rng(12345);
Mat debugImage;
Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);


/*
* MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
* Главная функция файла mainwindow.cpp
* Открытие веб-камеры, сообщение об ошибки, инициализация таймера.
*/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Открытие первой веб-камеры
    capWebcam.open(0);

    // Сообщения о отсутствии face_cascade или eye_cascade
    if( !face_cascade.load( face_cascade_name ) ) {QMessageBox::critical(this, "Ошибка!", "Ошибка загрузки face_cascade", QMessageBox::Ok); return; };
    if( !eye_cascade.load( eye_cascade_name ) ){QMessageBox::critical(this, "Ошибка!", "Ошибка загрузки eye_cascade", QMessageBox::Ok); return; };

    // Сообщение об отсутствии веб-камеры
    if (!capWebcam.isOpened())
    {
        QMessageBox::warning(this, "Ошибка!", "Камера не найдена");

        return; // выход
    }

    createCornerKernels();

    ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2),
            43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);

    // Инициализация таймера с обновлением в 60мс.
    tmrTimer = new QTimer(this);
    connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
    tmrTimer->start(60);
}


/*
* MainWindow::processFrameAndUpdateGUI()
* Функция отображения изображения с веб-камеры в lblWebCam
*/
void MainWindow::processFrameAndUpdateGUI()
{
    capWebcam.read(matOriginal);

    if(matOriginal.empty() == true) return;

    QImage qimgOriginal((uchar*)matOriginal.data, matOriginal.cols, matOriginal.rows,
                        matOriginal.step, QImage::Format_RGB888);

    cvtColor(matOriginal, matOriginal, CV_BGR2RGB);

    displayAndUpdateRectangle();

    ui -> lblWebCam -> setPixmap(QPixmap::fromImage(qimgOriginal));
}

void MainWindow::displayAndUpdateRectangle()
{
    if(matOriginal.empty() == true)
    {
        QMessageBox::warning(this, "Ошибка!", "Нед доступа к frame");
        return;
    }

    flip(matOriginal, matOriginal, 1);

    Mat gray;

    cvtColor(matOriginal, gray, CV_BGR2RGB);

    releaseCornerKernels();
}

void findEyes(Mat frame_gray, Rect face)
{
    Mat faceROI = framge_gray(face);

    if (smoothFaceImage)
    {
        double sigma = smoothFaceFactor * face.width;
        GaussianBlur(faceROI, faceROI, Size(0, 0), sigma);
    }

    // -- Поиск региона глаз и отрисовка
    int eye_region_width = face.width * (eyePercentHeight / 100.0);
    int eye_region_height = face.height * (eyePercentHeight / 100.0);
    int eye_region_top = face.height * (eyePercentTop / 100.0);

    Rect leftEyeRegion(face.width * (eyePercentSide / 100.0),
                       eye_region_top, eye_region_width, eye_region_height);

    Rect rightEyeRegion(face.width - eye_region_width - face.width * (eyePercentSide / 100.0),
                        eye_region_top, eye_region_width, eye_region_height);

    // -- Поиск центра глаза
    Point leftPupil = findEyeCenter(faceROI, leftEyeRegion, "Left Eye");
    Point rightPupil = findEyeCenter(faceROI, rightEyeRegion, "Right Eye");

    // -- Получаем регионы углов
    Rect leftRightCornerRegion(leftEyeRegion);
    leftRightCornerRegion.width -= leftPupil.x;
    leftRightCornerRegion.x += leftPupil.x;
    leftRightCornerRegion.height /= 2;
    leftRightCornerRegion.y += leftRightCornerRegion.height / 2;

    Rect leftLeftCornerRegion(leftEyeRegion);
    leftLeftCornerRegion.width = leftPupil.x;
    leftLeftCornerRegion.height /= 2;
    leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;

    Rect rightLeftCornerRegion(rightEyeRegion);
    rightLeftCornerRegion.width = rightPupil.x;
    rightLeftCornerRegion.height /= 2;
    rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;

    Rect rightRightCornerRegion(rightEyeRegion);
    rightRightCornerRegion.width -= rightPupil.x;
    rightRightCornerRegion.x += rightPupil.x;
    rightRightCornerRegion.height /= 2;
    rightRightCornerRegion.y += rightRightCornerRegion.height / 2;

    rectangle(debugFace,leftRightCornerRegion,200);
    rectangle(debugFace,leftLeftCornerRegion,200);
    rectangle(debugFace,rightLeftCornerRegion,200);
    rectangle(debugFace,rightRightCornerRegion,200);

    // -- Смена координат центра глаз относительно лица
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;

    // Отрисовка центра глаз
    circle(debugFace, rightPupil, 3, 1234);
    circle(debugFace, leftPupil, 3, 1234);

    // Поиск угла глаз
    if (enableEyeCorner)
    {
        Point2f leftRightCorner = fingEyeCorner(faceROI(leftRightCornerRegion), true, false);
        leftRightCorner.x += leftRightCornerRegion.x;
        leftRightCorner.y += leftRightCornerRegion.y;

        Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true);
        leftLeftCorner.x += leftLeftCornerRegion.x;
        leftLeftCorner.y += leftLeftCornerRegion.y;

        Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true);
        rightLeftCorner.x += rightLeftCornerRegion.x;
        rightLeftCorner.y += rightLeftCornerRegion.y;

        Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false);
        rightRightCorner.x += rightRightCornerRegion.x;
        rightRightCorner.y += rightRightCornerRegion.y;

        circle(faceROI, leftRightCorner, 3, 200);
        circle(faceROI, leftLeftCorner, 3, 200);
        circle(faceROI, rightLeftCorner, 3, 200);
        circle(faceROI, rightRightCorner, 3, 200);
    }

}

Mat findSkin (mat &frame)
{
    Mat input;
    Mat output = Mat(frame.rows, frame.cols, CV_8U);

    cvtColor(frame, input, CV_BGR2YCrCb);

    for (int y = 0; y < input.rows, ++y)
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
 *  @function detectAndDisplay
 */
void detectAndDisplay(Mat frame)
{
    vector<Rect> faces;

    vector<Mat> rgbChannels(3);
    split(frame, rgbChannels);
    Mat frame_gray = rgbChannels[2];

    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, Size(150, 150));

    for (int i = 0; i < faces.size(); i++)
    {
        rectangle(debugImage, faces[i], 1234);
    }

    if (faces.size() > 0)
    {
        findEyes(frame_gray, faces[0]);
    }
}

/*
* MainWindow::on_action_exit_triggered()
* Функция выхода по нажатию кнопки "Выход" в диалоговом меню "Меню"
*/
void MainWindow::on_action_exit_triggered()
{
    this->close(); // Закрытие текущего окна
}


/*
 * MainWindow::on_action_about_triggered()
 * Функция вывода окна about по нажатию кнопки "О программе" в диалоговом меню "Помощь"
 */
void MainWindow::on_action_about_triggered()
{
    aboutwindow *aboutWin = 0;

    aboutWin = new aboutwindow();
    aboutWin -> show(); // вывод окна aboutWin
}

MainWindow::~MainWindow()
{
    delete ui;
}
