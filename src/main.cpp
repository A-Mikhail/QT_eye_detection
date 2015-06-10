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

#include <QApplication>

#include "mainwindow.h"
#include "popupwindow.h"


void myMessageOutput(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    const char * msg = str.toStdString().c_str();

    FILE *file; // указатель на файл, в который пишем
    file = fopen("file.log", "a"); // открываем файл на запись
        switch (type) {
        case QtDebugMsg:
            fprintf(file, "Debug: %s\n", msg);
            break;
        case QtWarningMsg:
            fprintf(file, "Warning: %s\n", msg);
            break;
        case QtCriticalMsg:
            fprintf(file, "Critical: %s\n", msg);
            break;
        case QtFatalMsg:
            fprintf(file, "Fatal: %s\n", msg);
            abort();
     }

    fclose(file); // закрываем файл
 }

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);

    QApplication a(argc, argv);

    popupWindow p;
    MainWindow w;

    w.show();

    if (!QFile::exists("user.dat"))
    {
        p.setModal(true);
        p.show();
    }

    return a.exec();
}
