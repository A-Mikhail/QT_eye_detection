#include "optionswindow.h"
#include "ui_optionswindow.h"

optionsWindow::optionsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::optionsWindow)
{
    ui->setupUi(this);
}

optionsWindow::~optionsWindow()
{
    delete ui;
}
