#include "optionswindow.h"
#include "ui_optionswindow.h"

#include <QDebug>

optionsWindow::optionsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::optionsWindow)
{
    ui->setupUi(this);
}

optionsWindow::~optionsWindow()
{
    delete ui;
}
