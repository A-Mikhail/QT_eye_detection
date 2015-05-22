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

#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QWidget>

namespace Ui {
class optionsWindow;
}

class optionsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit optionsWindow(QWidget *parent = 0);
    ~optionsWindow();

private slots:

private:
    Ui::optionsWindow *ui;
};

#endif // OPTIONSWINDOW_H
