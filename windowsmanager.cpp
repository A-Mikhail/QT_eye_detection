#include "windowsmanager.h"

#include "videoInput.h"

MainWindow *mainwindow = 0;

windowsManager::windowsManager(QObject *parent) : QObject(parent)
{
    mainwindow = new MainWindow();
}

void windowsManager::deviceCount()
{
    // Список подключенных веб-камер
    int numDevices = videoInput::listDevices();

    if(numDevices > 0)
    {

/*
        if (selectwebcamwindow == 0){
            // Открытие окна выбора веб-камеры
            selectwebcamwindow = new selectWebcamWindow();

            connect(selectwebcamwindow, SIGNAL(signalWebcamIndex(QString)), mainwindow,
                    SLOT(recieveWebcamIndex(QString)));
        }

        selectwebcamwindow->show();
*/
        qDebug() << "hi";
    } else {
        mainwindow->show();
    }
}

windowsManager::~windowsManager()
{

}

