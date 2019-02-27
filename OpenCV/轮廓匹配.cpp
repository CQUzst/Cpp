#include <iostream>

#include "opencv2/opencv.hpp"

using namespace std;

using namespace cv;

int main()

{

	Mat k = imread("1.jpg", 0);

	Mat f;

	Mat k1 = imread("3.jpg", 0);

	Mat f1;

	threshold(k, f, 50, 255, THRESH_BINARY);//对图像进行二值化

	threshold(k1, f1, 50, 255, THRESH_BINARY);

	Mat closerect = getStructuringElement(MORPH_RECT, Size(3, 3)); //进行结构算子生成

	morphologyEx(f, f, MORPH_OPEN, closerect);

	morphologyEx(f1, f1, MORPH_OPEN, closerect);//进行形态学开运算

	Mat dst = Mat::zeros(k.rows, k.cols, CV_8UC3);

	Mat dst1 = Mat::zeros(k1.rows, k1.cols, CV_8UC3);

	vector<vector<Point>> w, w1;

	vector<Vec4i> hierarchy, hierarchy1;

	findContours(f, w, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);//提取轮廓元素

	findContours(f1, w1, hierarchy1, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	FileStorage fs("f.dat", FileStorage::WRITE);

	fs << "f" << w1[0];

	int idx = 0;

	double ffff = matchShapes(w[0], w1[0], CV_CONTOURS_MATCH_I3, 1.0);//进行轮廓匹配

	std::cout << ffff << std::endl;

	system("pause");

	return 0;

}