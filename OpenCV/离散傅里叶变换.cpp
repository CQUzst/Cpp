#include"opencv2/core/core.hpp"
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include<iostream>
using namespace cv;

int main()
{
	//以灰度模式读取图像并显示
	Mat srcImage = imread("5.jpg",0);//通道为灰度
	if (!srcImage.data) 
	{
		printf("error"); 
		return false; 
	}
	imshow("Origin Image", srcImage);
	//将输入图像延展到最佳尺寸，边界用0补充
	int m = getOptimalDFTSize(srcImage.rows);
	int n = getOptimalDFTSize(srcImage.cols);
	//将添加的元素初始化为0
	Mat padded;
	copyMakeBorder(srcImage, padded, 0, m - srcImage.rows, 0, n - srcImage.cols, BORDER_CONSTANT, Scalar::all(0));
	//为傅里叶变换结果（实部和虚部）分配存储空间
	//将planes数组组合合并成一个多通道的数组complexI
	Mat planes[] = { Mat_<float>(padded),Mat::zeros(padded.size(),CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);//加一个额外通道存储复数部分
	//进行就地离散傅里叶变换
	dft(complexI, complexI);
	//将复数转化为幅值
	split(complexI, planes);//planes[0]=Re(DFT(I)),planes[1]=Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);//将幅值赋值给planes[0]
	Mat magnitudeImage = planes[0];
	//进行对数尺度缩放
	magnitudeImage = magnitudeImage + Scalar::all(1);
	log(magnitudeImage, magnitudeImage);
	//剪切和重分布幅度图象限
	//若有奇数行或者奇数列，进行频谱裁剪
	magnitudeImage = magnitudeImage(Rect(0, 0, magnitudeImage.cols&-2, magnitudeImage.rows&-2));
	//重新排列傅里叶图像中的象限，使得原点位于图像中心
	int cx = magnitudeImage.cols / 2;
	int cy = magnitudeImage.rows / 2;
	Mat q0(magnitudeImage, Rect(0, 0, cx, cy));//左上
	Mat q1(magnitudeImage, Rect(cx, 0, cx, cy));//右上
	Mat q2(magnitudeImage, Rect(0, cy, cx, cy));//左下
	Mat q3(magnitudeImage, Rect(cx, cy, cx, cy));//右下
	//交换象限
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	//归一
	normalize(magnitudeImage, magnitudeImage, 0, 1, NORM_MINMAX);
	imshow("频谱幅值图", magnitudeImage);
	waitKey();
	return 0;
}