#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
using namespace cv;
#define window_name "滑动条图例"
const int Max = 100;
int n;
double a, b;
Mat srcImage1;
Mat srcImage2;
Mat dstImage;
//-------------------------------on_Trackbar()函数--------------------------------
void on_Trackbar(int, void*)
{
	a = (double)n / Max;
	b = 1.0 - a;
	addWeighted(srcImage1, a, srcImage2, b, 0.0, dstImage);
	imshow(window_name, dstImage);
}
int main()
{
	srcImage1 = imread("1.jpg");
	srcImage2 = imread("2.jpg");
	n = 90;
	namedWindow ( window_name,1);
	char TrackbarName[40];
	sprintf(TrackbarName, "透明值%d",Max);
	createTrackbar(TrackbarName, window_name, &n, Max, on_Trackbar);
	on_Trackbar(n, 0);
	waitKey(0);
	return 0;
}
