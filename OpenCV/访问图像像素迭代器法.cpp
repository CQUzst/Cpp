#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;
void colorreduce(Mat &inputImage, Mat &outputImage, int div)//color reduceµü´úÆ÷·ÃÎÊ
{
	outputImage = inputImage.clone();
	Mat_<Vec3b>::iterator it = outputImage.begin<Vec3b>();
	Mat_<Vec3b>::iterator itend = outputImage.end<Vec3b>();
	for (; it != itend; ++it)
	{
		(*it)[0] = (*it)[0] / div*div;
		(*it)[1] = (*it)[1] / div*div;
		(*it)[2] = (*it)[2] / div*div;
	}
}
int main()
{
	Mat srcImage = imread("1.jpg");
	imshow("Original Image", srcImage);
	Mat dstImage;
	dstImage.create(srcImage.rows, srcImage.cols, srcImage.type());
	double time0 = static_cast<double>(getTickCount());
	colorreduce(srcImage, dstImage, 50);
	time0 = ((double)getTickCount() - time0) / getTickFrequency();
	cout << "This method runs for " << time0 << " seconds." << endl;
	imshow("Destination Image", dstImage);
	waitKey(0);
}