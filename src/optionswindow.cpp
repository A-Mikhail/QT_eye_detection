#include "optionswindow.h"
#include "ui_optionswindow.h"

#include "stdafx.h"

optionsWindow::optionsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::optionsWindow)
{
    ui->setupUi(this);

    // Чтение из файла
    QFile file("user.dat");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    QString profileName;
    in >> profileName;

    ui->label_3->setText(profileName); // Текущий пользователь
}

optionsWindow::~optionsWindow()
{
    delete ui;
}


/*
* optionsWindow::on_toolButton_clicked()
* Выход из окна настроек
*/
void optionsWindow::on_pushButton_clicked()
{
    this->close();
}

/*
* optionsWindow::on_toolButton_clicked()
* Удаление пользователя
* По нажатию кнопки Yes, удалить файл с пользователем user.dat
*/
void optionsWindow::on_toolButton_clicked()
{
    QMessageBox::StandardButton messageBox;

    messageBox = QMessageBox::question(this, "Удаление пользователя", "Вы действительно хотите удалить пользователя?",
                                  QMessageBox::Yes|QMessageBox::No);

    if(messageBox == QMessageBox::Yes)
    {
        QFile::remove("user.dat"); // Удаление файла

        QDir("cropped").removeRecursively(); // Удаление папки "cropped"

        // Проверка на отсутствие папки и файла
        if(!QFile::exists("user.dat") || !QDir("cropped").exists())
        {
            QMessageBox::information(this, "Успешно!", "Пользователь успешно удалён!", QMessageBox::Ok);
        } else {
            QMessageBox::critical(this, "Ошибка!", "Ошибка удаления пользователя!", QMessageBox::Ok);
        }
    }


    if(messageBox == QMessageBox::No)
    {
        this->close();
    }
}
