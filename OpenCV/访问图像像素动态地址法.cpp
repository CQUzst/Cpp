#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;
void colorreduce(Mat &inputImage, Mat &outputImage, int div)//color reduceµü´úÆ÷·ÃÎÊ
{
	outputImage = inputImage.clone();
	int rownumber = outputImage.rows;
	int colnumber = outputImage.cols;
	for (int i=0; i< rownumber; i++)
	{
		for (int j = 0; j< colnumber; j++)
		{
			outputImage.at<Vec3b>(i, j)[0] = outputImage.at<Vec3b>(i, j)[0] / div*div;
			outputImage.at<Vec3b>(i, j)[1] = outputImage.at<Vec3b>(i, j)[1] / div*div;
			outputImage.at<Vec3b>(i, j)[2] = outputImage.at<Vec3b>(i, j)[2] / div*div;
		}
	}
}
int main()
{
	Mat srcImage = imread("1.jpg");
	imshow("Original Image", srcImage);
	Mat dstImage;
	dstImage.create(srcImage.rows, srcImage.cols, srcImage.type());
	double time0 = static_cast<double>(getTickCount());
	colorreduce(srcImage, dstImage, 32);
	time0 = ((double)getTickCount() - time0) / getTickFrequency();
	cout << "This method runs for " << time0 << " seconds." << endl;
	imshow("Destination Image", dstImage);
	waitKey(0);
}