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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWidgetAction>

#include <QMainWindow>
#include <QComboBox>

#include "stdafx.h"

// include window file
#include "optionswindow.h"
#include "aboutwindow.h"

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

    // Нажатие кнопки "Выход"
    void on_action_exit_triggered();

    // Нажатие кнопки "О программе"
    void on_action_about_triggered();

    // Нажатие кнопки "Настройки"
    void on_action_settings_triggered();

public slots:   

    // Индекс выбранной веб-камеры
    void webcamIndex();

    // Обновление окна
    void processFrameAndUpdateGUI();

    // Поиск глаз
    void findEyes(cv::Mat frame_gray, cv::Rect face);

    // Определение области глаз и отображение в окне
    void detectAndDisplay(cv::Mat frame);

    // вырезание области интереса (глаза)
    void croppedROI(cv::Mat frame);

private:
    Ui::MainWindow *ui;

    aboutWindow         *aboutwindow        = 0;
    optionsWindow       *optionswindow      = 0;

    QHBoxLayout *layout;
    QWidget *widget;

    // Кнопки меню
    QPushButton* exitButton;
    QPushButton* settingsButton;
    QPushButton* aboutButton;

    QLabel* webcamLabel;
    QComboBox* combobox;

    QSpacerItem* horizontalSpacer;

    cv::Mat frame;

    cv::VideoCapture capWebCam;

    QImage qimgOriginal;

    QTimer* tmrTimer; 
};


#endif // MAINWINDOW_H
