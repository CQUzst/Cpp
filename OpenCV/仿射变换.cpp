#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace std;
using namespace cv;
#define window_name1 "原始窗口"
#define window_name2 "经过warp后的图像"
#define window_name3 "经过warp和rotate后的图像"
int main()
{
	system("color 2A");
	Point2f srcTriangle[3];
	Point2f dstTriangle[3];
	Mat rotMat(2, 3, CV_32FC1);
	Mat warpMat(2, 3, CV_32FC1);
	Mat srcImage, dstImage_warp, dstImage_warp_rotate;
	srcImage = imread("1.jpg");
	imshow(window_name1, srcImage);
	dstImage_warp = Mat::zeros(srcImage.rows, srcImage.cols, srcImage.type());
	srcTriangle[0] = Point2f(0, 0);
	srcTriangle[1] = Point2f(static_cast<float>(srcImage.cols - 1), 0);
	srcTriangle[2] = Point2f( 0,static_cast<float>(srcImage.rows - 1));
	dstTriangle[0] = Point2f(static_cast<float>(srcImage.cols-1), srcImage.rows - 1);
	dstTriangle[1] = Point2f(static_cast<float>( srcImage.rows-1),0);
	dstTriangle[2] = Point2f(0, static_cast<float>(srcImage.rows - 1));
	warpMat = getAffineTransform(srcTriangle, dstTriangle);
	warpAffine(srcImage, dstImage_warp, warpMat, dstImage_warp.size());
	imshow(window_name2, dstImage_warp);
	Point center = Point(dstImage_warp.cols / 2, dstImage_warp.rows / 2);
	double angle = 0;
	double scale = 0.5;
	rotMat = getRotationMatrix2D(center, angle, scale);
	warpAffine(dstImage_warp, dstImage_warp_rotate, rotMat, dstImage_warp.size());
	imshow(window_name3, dstImage_warp_rotate);
	waitKey(0);
	return 0;
}