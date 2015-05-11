#include "selectwebcamwindow.h"
#include "ui_selectwebcamwindow.h"

#include "stdafx.h"
#include "videoInput.h"

using namespace std;

selectWebcamWindow::selectWebcamWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::selectWebcamWindow)
{
    ui->setupUi(this);

    // Список подключенных веб-камер
    int numDevices = videoInput::listDevices();

    // Вывод списка веб-камер в comboBox
    for(int i = 0; i < numDevices; i++){
        ui->comboBox->addItem(videoInput::getDeviceName(i));
    }
}


selectWebcamWindow::~selectWebcamWindow()
{
    delete ui;
}


void selectWebcamWindow::on_pushButton_clicked()
{
    QString webcamIndexText = QString::number(ui->comboBox->currentIndex());

    emit signalWebcamIndex(webcamIndexText);

    this->hide();

    printf("clicked");
}
