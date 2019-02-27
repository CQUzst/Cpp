#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;
static void on_ContrastAndBright(int, void*);
int ContrastValue;
int BrightValue;
Mat srcImage, dstImage;
int main()
{
	srcImage = imread("1.jpg");
	if (!srcImage.data) { printf("error"); return false; }
	dstImage = Mat::zeros(srcImage.size(), srcImage.type());
	cout << "Please input ContractValue:";
	cin >> ContrastValue;
	cout <<endl<< "Please input BrightValue:";
	cin >> BrightValue;
	namedWindow("Design Sketch",1);
	createTrackbar("ContrastValue:", "Design Sketch", &ContrastValue, 300, on_ContrastAndBright);
	createTrackbar("BrightValue:", "Design Sketch", &ContrastValue, 200, on_ContrastAndBright);
	on_ContrastAndBright(ContrastValue, 0);
	on_ContrastAndBright(BrightValue, 0);
	while(char(waitKey(1)!='q')){}
	return 0;
}
static void on_ContrastAndBright(int, void*)
{
	namedWindow("Origin Image", 1);
	for (int y = 0; y < srcImage.rows; y++)
	{
		for (int x = 0; x < srcImage.cols; x++)
		{
			dstImage.at<Vec3b>(y, x)[0] = saturate_cast<uchar>((ContrastValue*0.01)*(srcImage.at<Vec3b>(y, x)[0] )+ BrightValue);
			dstImage.at<Vec3b>(y, x)[1] = saturate_cast<uchar>((ContrastValue*0.01)*(srcImage.at<Vec3b>(y, x)[1]) + BrightValue);
			dstImage.at<Vec3b>(y, x)[2] = saturate_cast<uchar>((ContrastValue*0.01)*(srcImage.at<Vec3b>(y, x)[2]) + BrightValue);
		}
	}
	imshow("Origin Image", srcImage);
	imshow("Design Sketch", dstImage);
}