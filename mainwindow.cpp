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

int detectEye(Mat& im, Mat& tpl, Rect& rect)
{
    vector<Rect> faces, eyes;
    face_cascade.detectMultiScale(im, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

    for (size_t i = 0; i < faces.size(); i++)
    {
        Mat face = im(faces[i]);
        eye_cascade.detectMultiScale(face, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(20, 20));

        if (eyes.size())
        {
            rect = eyes[0] + Point(faces[i].x, faces[i].y);
            tpl = im(rect);
        }
    }
    return eyes.size();
}

void trackEye(Mat& im, Mat& tpl, Rect& rect)
{
    Size size(rect.width * 2, rect.height * 2);
    Rect window(rect + size - Point(size.width / 2, size.height / 2));

    window &= Rect(0, 0, im.cols, im.rows);

    Mat dst(window.width - tpl.rows + 1, window.height - tpl.cols + 1, CV_32FC1);
    matchTemplate(im(window), tpl, dst, CV_TM_SQDIFF_NORMED);

    double minval, maxval;
    Point minloc, maxloc;
    minMaxLoc(dst, &minval, &maxval, &minloc, &maxloc);

    if (minval <= 0.2)
    {
        rect.x = window.x + minloc.x;
        rect.y = window.y + minloc.y;
    }
    else
        rect.x = rect.y = rect.width = rect.height = 0;
}


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

    if( !face_cascade.load( face_cascade_name ) ) {QMessageBox::critical(this, "Ошибка!", "Ошибка загрузки face_cascade", QMessageBox::Ok); return; };
    if( !eye_cascade.load( eye_cascade_name ) ){QMessageBox::critical(this, "Ошибка!", "Ошибка загрузки eye_cascade", QMessageBox::Ok); return; };

    // Сообщение об отсутствии веб-камеры
    if (!capWebcam.isOpened())
    {
        QMessageBox::warning(this, "Ошибка!", "Камера не найдена");

        return; // выход
    }

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

    if (eye_bb.width == 0 && eye_bb.height == 0)
    {
        detectEye(gray, eye_tpl, eye_bb);

    } else {

        trackEye(gray, eye_tpl, eye_bb);

        rectangle(matOriginal, eye_bb, CV_RGB(0, 255, 0));

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
