#ifndef FINDEYECORNER
#define FINDEYECORNER

#include "opencv2/imgproc/imgproc.hpp"

#define eyeLeft true;
#define eyeRight false;

class findEyeCorner
{
public slots:

    void createCornerKernels();

    void releaseCornerKernels();

    cv::Point2f eyeCorner(cv::Mat region, bool left, bool left2);

    cv::Point2f findSubPixelEyeCorner(cv::Mat region);
};

#endif // FINDEYECORNER

