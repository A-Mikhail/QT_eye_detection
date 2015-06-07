#include "popupwindow.h"
#include "ui_popupwindow.h"

#include "stdafx.h"
#include "mainwindow.h"

popupWindow::popupWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::popupWindow)
{
    ui->setupUi(this);

    // Удалить кнопку "помощь"
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowFlags(Qt::WindowTitleHint);

    // ограничение на вводимые символы в lineEdit
    ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+"), this));
}


void popupWindow::on_pushButton_clicked()
{
    QString profileName = ui->lineEdit->text();

    // проверка на присутствие текста
    if(ui->lineEdit->text() == "")
    {
        QMessageBox::warning(this, "Ошибка!", "Имя пользователя не должно быть пустым");

        return;
    } else {
        // Создание файла user.dat с добавлением текста из lineEdit
        QFile file("user.dat");
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        out << QString(profileName);
    }

    this->close();
}


popupWindow::~popupWindow()
{
    delete ui;
}
