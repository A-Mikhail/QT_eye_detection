# eyeDetection
Eye detection using haarcascade, written in C++ with Qt and OpenCV

Function:
findeyecenter.cpp
	- Mat floodKillEdges(Mat &mat);
	- Point unscalePoint(Point p, Rect origSize);
	- void scaleToFastSize(const Mat &src, Mat &dst);
	- Mat computeMatXGradient(const Mat *mat)
	- void testPossibleCentersFormula(int x, int y, const Mat &weight, double gx, double gy, Mat &out);
	- Point findEyeCenter(Mat face, Rect eye)
	- bool floodShouldPushPoint(const Point &np, const Mat &mat);

