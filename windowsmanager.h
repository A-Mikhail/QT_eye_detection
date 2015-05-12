#ifndef WINDOWSMANAGER_H
#define WINDOWSMANAGER_H

#include <QObject>

#include "stdafx.h"

#include "mainwindow.h"
#include "selectwebcamwindow.h"

class windowsManager : public QObject
{
    Q_OBJECT
public:
    explicit windowsManager(QObject *parent = 0);
    ~windowsManager();

     static void deviceCount();

signals:

public slots:

private:
    selectWebcamWindow *selectwebcamwindow = 0;
};

#endif // WINDOWSMANAGER_H
