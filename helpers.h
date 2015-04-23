/*
   “Copyright 2015 Адаменко Михаил”

    This file is part of Alizee.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Alizee.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HELPERS_H
#define HELPERS_H

#include "opencv2/imgproc/imgproc.hpp"

class helpers
{
public:

public slots:
    bool rectInImage(cv::Rect rect, cv::Mat image);

    bool inMat(cv::Point p, int rows, int cols);

    cv::Mat matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY);

    double computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor);

};

#endif // HELPERS_H
