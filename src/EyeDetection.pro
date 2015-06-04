#-------------------------------------------------
#
# Project created by QtCreator 2015-04-08T13:26:07
#
#-------------------------------------------------

#-------------------------------------------------
#   “Copyright 2015 Адаменко Михаил”
#
#   This file is part of EyeDetection.

#   Foobar is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.

#   Foobar is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with EyeDetection.  If not, see <http://www.gnu.org/licenses/>.
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EyeDetection
TEMPLATE = app

INCLUDEPATH += D:\OpenCV_withQt\debug\install\include
INCLUDEPATH += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\include

LIBS += D:\OpenCV_withQt\release\bin\libopencv_core2410.dll
LIBS += D:\OpenCV_withQt\release\bin\libopencv_contrib2410.dll
LIBS += D:\OpenCV_withQt\release\bin\libopencv_highgui2410.dll
LIBS += D:\OpenCV_withQt\release\bin\libopencv_imgproc2410.dll
LIBS += D:\OpenCV_withQt\release\bin\libopencv_calib3d2410.dll
LIBS += D:\OpenCV_withQt\release\bin\libopencv_objdetect2410.dll

LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\libvideoInput.a
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\ddraw.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\dxguid.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\glfw.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\ole32.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\oleaut32.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\strmbasd.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\strmbase.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\strmiids.lib
LIBS += C:\Users\micha_000\Desktop\Pr.Alizee\EyeDetection\src\videoInput\uuid.lib

QMAKE_CXXFLAGS += -std=c++11
CONFIG += precompile_header \

PRECOMPILED_HEADER = stdafx.h

SOURCES  +=\
            mainwindow.cpp \
            aboutwindow.cpp \
    main.cpp \
    optionswindow.cpp \
    helpers.cpp \
    findeye.cpp \
    popupwindow.cpp

HEADERS  += mainwindow.h \
            aboutwindow.h \
            stdafx.h \
    constants.h \
    version.h \
    optionswindow.h \
    helpers.h \
    findeye.h \
    popupwindow.h

FORMS    += mainwindow.ui \
            aboutwindow.ui \
    optionswindow.ui \
    popupwindow.ui

RESOURCES += \
    resources.qrc \

RC_FILE = EyeDetection.rc

DISTFILES += \
    videoInput/include/videoInput.h \
    videoInput/ddraw.lib \
    videoInput/dxguid.lib \
    videoInput/glfw.lib \
    videoInput/libvideoInput.a \
    videoInput/ole32.lib \
    videoInput/oleaut32.lib \
    videoInput/strmbasd.lib \
    videoInput/strmbase.lib \
    videoInput/strmiids.lib \
    videoInput/uuid.lib \
    haarcascade/haarcascade_eye.xml \
    haarcascade/haarcascade_frontalface_default.xml
