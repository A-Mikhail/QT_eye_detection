#-------------------------------------------------
#
# Project created by QtCreator 2015-04-08T13:26:07
#
#-------------------------------------------------

#-------------------------------------------------
#   “Copyright 2015 Адаменко Михаил”
#
#   This file is part of Alizee.

#   Foobar is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.

#   Foobar is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with Alizee.  If not, see <http://www.gnu.org/licenses/>.
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AlizeeQt
TEMPLATE = app

INCLUDEPATH += D:\OpenCV_withQt\debug\install\include

LIBS += D:\OpenCV_withQt\debug\bin\libopencv_core2410d.dll
LIBS += D:\OpenCV_withQt\debug\bin\libopencv_contrib2410d.dll
LIBS += D:\OpenCV_withQt\debug\bin\libopencv_highgui2410d.dll
LIBS += D:\OpenCV_withQt\debug\bin\libopencv_imgproc2410d.dll
LIBS += D:\OpenCV_withQt\debug\bin\libopencv_calib3d2410d.dll
LIBS += D:\OpenCV_withQt\debug\bin\libopencv_objdetect2410d.dll

CONFIG += precompile_header

PRECOMPILED_HEADER = stdafx.h

SOURCES  += main.cpp\
            mainwindow.cpp \
            aboutwindow.cpp \
    findeyecorner.cpp \
    findeyecenter.cpp \
    helpers.cpp

HEADERS  += mainwindow.h \
            aboutwindow.h \
            stdafx.h \
    findeyecorner.h \
    findeyecenter.h \
    constants.h \
    helpers.h

FORMS    += mainwindow.ui \
            aboutwindow.ui

RESOURCES += \
    resources.qrc \

RC_FILE = alizee.rc

