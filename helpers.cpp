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

#include "helpers.h"
#include "stdafx.h"

#include "constants.h"

using namespace std;
using namespace cv;

bool helpers::rectInImage(Rect rect, Mat image)
{
    return rect.x > 0 && rect.y > 0 && rect.x + rect.width < image.cols &&
            rect.y + rect.height < image.rows;
}

bool helpers::inMat(Point p, int rows, int cols)
{
    return p.x >= 0 && p.x < cols && p.y >= 0 && p.y < rows;
}

Mat helpers::matrixMagnitude(const Mat &matX, const Mat &matY)
{
    Mat mags(matX.rows, matX.cols, CV_64F);

    for (int y = 0; y < matX.rows; ++y)
    {
        const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);

        double *Mr = mags.ptr<double>(y);

        for (int x = 0; x < matX.cols; ++x)
        {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = sqrt((gX * gX) + (gY * gY));

            Mr[x] = magnitude;
        }
    }

    return mags;

}

double helpers::computeDynamicThreshold(const Mat &mat, double stdDevFactor)
{
    Scalar stdMagnGrad, meanMagnGrad;

    meanStdDev(mat, meanMagnGrad, stdMagnGrad);

    double stdDev = stdMagnGrad[0] / sqrt (mat.rows * mat.cols);

    return stdDevFactor * stdDev * meanMagnGrad[0];
}
