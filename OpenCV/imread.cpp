#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
void main()
{
	Mat srcImage = imread("1.jpg",1);
	Mat srcImage2 = imread("1.jpg", 0);
	imshow("winname", srcImage);
	imshow("winname2", srcImage2);
	waitKey(0);
}