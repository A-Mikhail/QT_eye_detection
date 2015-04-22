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

#include "findeyecenter.h"
#include "stdafx.h"

#include "constants.h"
#include "helpers.h"

using namespace std;
using namespace cv;

Mat floodKillEdges(Mat &mat);

#pragma mark Helpers

Point unscalePoint(Point p, Rect origSize)
{
    float ratio = (((float)fastEyeWidth)/origSize.width);

    int x = round(p.x / ratio);
    int y = round(p.y / ratio);

    return Point(x,y);
}

void scaleToFastSize(const Mat &src, Mat &dst)
{
    resize(src, dst, Size(fastEyeWidth,(((float)fastEyeWidth) / src.cols ) * src.rows));
}

Mat computeMatXGradient(const Mat *mat)
{
    Mat out(mat.rows, mat.cols, CV_64F);

    for (int y = 0; y < mat.rows; ++y)
    {
        const uchar *Mr = mar.ptr<uchar>(y);
        double *Or = out.ptr<double>(y);

        Or[0] = Mr[1] - Mr[0];

        for (int x = 1; x < mat.cols - 1; ++x)
        {
            Or[x] = (Mr[x+1] - Mr[x-1]) / 2.0;
        }

        Or[mat.cols - 1] = Mr[mat.cols - 1] - Mr[mat.cols - 2];

    }

    return out;
}

#pragma mark Main Algorithm

void testPossibleCentersFormula(int x, int y, const Mat &weight, double gx, double gy, Mat &out)
{
    for (int cy = 0; cy < out.rows; ++cy)
    {
        double *Or = out.ptr<double>(cy);

        const unsigned char *Wr = weight.ptr<unsigned char>(cy);

        for (int cx = 0; cx < out.cols; ++cx)
        {
            if (x == cx && y == cy)
            {
                continue;
            }

            double dx = x - cx;
            double dy = y - cy;

            double magnitude = sqrt((dx * dx) + (dy * dy));

            dx = dx / magnitude;
            dy = dy / magnitude;

            double dotProduct = dx * gx + dy * gy;

            dotProduct = max(0.0, dotProduct);

            if(enableWeight)
            {
                Or[cx] += dotProduct * dotProduct * (Wr[cx] / WeightDivisor);
            } else {
                Or[cx] += dotProduct * dotProduct;
            }
        }
    }
}

Point findEyeCenter(Mat face, Rect eye)
{
    Mat eyeROIUnscaled = face(eye);
    Mat eyeROI;

    scaleToFastSize(eyeROIUnscaled, eyeROI);

    // Draw eye region
    rectangle(face, eye, 1234);

    // Find the gradient
    Mat gradientX = computeMatXGradient(eyeROI);
    Mat gradientY = computeMatXGradient(eyeROI.t()).t();

    // Normilize and threshold the gradient
    // compute all the magnitudes
    Mat mags = matrixMagnitude(gradientX, gradientY);

    // compute the threshold
    double gradientThresh = computeDynamicThreshold(mags, gradientThreshold);

    for (int y = 0; y < eyeROI.rows; ++y)
    {
        double *Xr = gradientX.ptr<double>(y), *Yr = gradientX.ptr<double>(y);

        const double *Mr = mags.ptr<double>(y);

        for (int x = 0; x < eyeROI.cols; ++x)
        {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = Mr[x];

            if (magnitude > gradientThresh)
            {
                Xr[x] = gX / magnitude;
                Yr[x] = gY / magnitude;
            } else {
                Xr[x] = 0.0;
                Yr[x] = 0.0;
            }
        }
    }


    Mat weight;

    GaussianBlur(eyeROI, weight, Size(weightBlurSize, weightBlurSize), 0, 0);

    for (int y = 0; y < weight.rows; ++y)
    {
        unsigned char *row = weight.ptr<unsigned char>(y);

        for (int x = 0; x < weight.cols; ++x)
        {
            row[x] = (255 - row[x]);
        }
    }

    Mat outSum = Mat::zeros(eyeROI.rows, eyeROI.cols, CV_64F);

    for (int y = 0; y < weight.rows; ++y)
    {
        const double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);

        for (int x = 0; x < weight.cols; ++x)
        {
            double gX = Xr[x], gY = Yr[x];

            if (gX == 0.0 && gY == 0.0)
            {
                continue;
            }

            testPossibleCentersFormula(x, y, weight, gX, gY, outSum);

        }
    }

    double numGradients = (weight.rows * weight.cols);

    Mat out;

    outSum.convertTo(out, CV_32F, 1.0 / numGradients);

    Point maxP;

    double maxVal;

    minMaxLoc(out, NULL, &maxVal, NULL, &maxP);

    if(enablePostProcess)
    {
        Mat floodClone;

        double floodThresh = maxVal * PostProcessThreshold;

        threshold(out, floodClone, floodThresh, 0.0f, THRESH_TOZERO);

        if(plotVectorField)
        {
            imwrite("eyeFrame.png", eyeROIUnscaled);
        }

        Mat mask = floodKillEdges(floodClone);

        minMaxLoc(out, NULL, &maxVal, NULL, &maxP, mask);
    }

    return unscalePoint(maxP, eye);

}

#pragma mark Postprocessing

bool floodShouldPushPoint(const Point &np, const Mat &mat)
{
    return inMat(np, mat.rows, mat.cols);
}

// returns a mask
Mat floodKillEdges(Mat &mat)
{
    rectangle(mat, Rect(0, 0, mat.cols, mat.rows), 255);

    Mat mask(mat.rows, mat.cols, CV_8U, 255);

    queue<Point> toDo;

    toDo.push(Point(0, 0));

    while (!toDo.empty())
    {
        Point p = toDo.front();

        toDo.pop();

        if(mat.at<float>(p) == 0.0f)
        {
            contiune;
        }

        Point np(p.x + 1, p.y); // right
        if (floodShouldPushPoint(np, mat)) toDo.pish(np);

        np.x = p.x - 1; np.y = p.y; // left;
        if (floodShouldPushPoint(np, mat)) toDo.pish(np);

        np.x = p.x; np.y = p.y + 1; // down;
        if (floodShouldPushPoint(np, mat)) toDo.pish(np);

        np.x = p.x; np.y = p.y - 1; // up
        if (floodShouldPushPoint(np, mat)) toDo.pish(np);

        // kill
        mat.at<float>(p) = 0.0f;
        mask.at<uchar>(p) = 0;
    }

    return mask;

}


