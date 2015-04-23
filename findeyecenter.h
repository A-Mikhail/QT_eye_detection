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

#ifndef FINDEYECENTER_H
#define FINDEYECENTER_H

#include "opencv2/imgproc/imgproc.hpp"

class findEyeCenter
{
public slots:
    cv::Point unscalePoint(cv::Point p, cv::Rect origSize);

    void scaleToFastSize(const cv::Mat &src, cv::Mat &dst);

    cv::Mat computeMatXGradient(const cv::Mat &mat);

    void testPossibleCentersFormula(int x, int y, const cv::Mat &weight, double gx, double gy, cv::Mat &out);

    cv::Point eyeCenter(cv::Mat face, cv::Rect eye, std::string debugWindow);

    bool floodShouldPushPoint(const cv::Point &np, const cv::Mat &mat);
    cv::Mat floodKillEdges(cv::Mat &mat);
};

#endif // FINDEYECENTER_H
