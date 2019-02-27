#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<cctype>
using namespace cv;
using namespace std;
RNG g_rng(12345);
int main()
{
	Mat srcImage = imread("1.jpg", 1);
	Mat outImage,maskImage;
	outImage = srcImage.clone();
	cout << srcImage.cols << "\t" << srcImage.rows << endl;
	int a, b, c;
	a = srcImage.at<Vec3b>(150, 350)[0];
	b = srcImage.at<Vec3b>(150, 350)[1];
	c = srcImage.at<Vec3b>(150, 350)[2];
	cout << a << "\t" << b << "\t" << c;
	for (int i = 0; i < srcImage.rows; i++)
	{
		for (int j = 0; j < srcImage.cols; j++)
		{
			if ((outImage.at<Vec3b>(i, j)[0] >180 && outImage.at<Vec3b>(i, j)[0]<255)
				&& (outImage.at<Vec3b>(i, j)[1]<200 && outImage.at<Vec3b>(i, j)[1]>150)
				&& (outImage.at<Vec3b>(i, j)[2] >40 && outImage.at<Vec3b>(i, j)[2] <80))
			{
				outImage.at<Vec3b>(i, j)[0] = 211;
				outImage.at<Vec3b>(i, j)[1] = 178;
				outImage.at<Vec3b>(i, j)[2] = 61;
			}
			else
			{
				outImage.at<Vec3b>(i, j)[0] = 0;
				outImage.at<Vec3b>(i, j)[1] = 0;
				outImage.at<Vec3b>(i, j)[2] = 0;
			}

		}
	}
	Mat element = getStructuringElement(MORPH_RECT, Size(20, 20));
	dilate(outImage, outImage, element);
	erode(outImage, outImage, element);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(outImage, outImage, element1);
	cvtColor(outImage, maskImage, CV_RGB2GRAY);
	threshold(maskImage, maskImage, 0, 222, CV_THRESH_BINARY);//二值化 
	vector<vector<Point>>contours;
	findContours(maskImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); //提取出最外面的轮廓
		double area, maxArea = 100;//过滤掉连通区域过小的连通区域
		for (int i = 0; i<contours.size(); i++)
		{
			Rect rect = boundingRect(contours[i]);
			printf("\nx=%d,y=%d,width=%d,height=%d", rect.x, rect.y, rect.width, rect.height);
			if (rect.width > 20 && rect.height > 20)
			{
				rectangle(outImage, Point(rect.x, rect.y),
					Point(rect.x + rect.width, rect.y + rect.height),
					cvScalar(0, 0, 255), 2, 8, 0);
				rectangle(srcImage, Point(rect.x, rect.y),
					Point(rect.x + rect.width, rect.y + rect.height),
					cvScalar(0, 0, 255), 2, 8, 0);
			}
		}
	imshow("原图", srcImage);
	imshow("分离图", outImage);
	imshow("掩膜图", maskImage);
	waitKey(0);
	return 0;
}

