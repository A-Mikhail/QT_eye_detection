#include "aboutwindow.h"
#include "ui_aboutwindow.h"

aboutwindow::aboutwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::aboutwindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: #333333;");
}

aboutwindow::~aboutwindow()
{
    delete ui;
}
