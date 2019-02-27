#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>


using namespace std;
using namespace cv;

enum EnemyColor { RED = 0, BLUE = 1 };
unsigned char enemy_color = 0;              // 0 for red, otherwise blue

unsigned char min_light_gray = 210;             // 板灯最小灰度值,用于对敌方颜色通道进行二值化
unsigned char min_light_height = 8;           // 板灯最小高度值
unsigned char avg_contrast_threshold = 110;     // 对比度检测中平均灰度差阈值,大于该阈值则为显著点
unsigned char light_slope_offset30;         // 允许灯柱偏离垂直线的最大偏移量,单位度
int  max_light_delta_h = 450;                   // 左右灯柱在水平位置上的最大差值,像素单位
unsigned char min_light_delta_h = 12;          // 左右灯柱在水平位置上的最小差值,像素单位
unsigned char max_light_delta_v = 50;          // 左右灯柱在垂直位置上的最大差值,像素单位
unsigned char max_light_delta_angle = 30;      // 左右灯柱在斜率最大差值,单位度
unsigned char avg_board_gray_threshold = 80; // 矩形区域平均灰度阈值,小于该阈值则选择梯度最小的矩阵
unsigned char avg_board_grad_threshold = 25; // 矩形区域平均梯度阈值,小于该阈值则选择梯度最小的矩阵
unsigned char grad_threshold = 25;             // 矩形区域梯度阈值,若大于该阈值,则累加 梯度/阈值
unsigned char br_threshold = 30;               // 红蓝通道相减后的阈值



cv::RotatedRect _res_last;      // last detect result
cv::Rect _dect_rect;            // detect roi of original image
cv::Mat _binary_template;       // armor template binary image
cv::Mat _binary_template_small; // small armor template binay image
cv::Mat _src;                   // source image
cv::Mat _g;                     // green component of source image
cv::Mat _ec;                    // enemy color
cv::Mat maskImage;
cv::Mat maskImage1;
cv::Size _size;
#define PI 3.1415926

bool makeRectSafe(cv::Rect & rect, cv::Size size)
{
	if (rect.x < 0)
		rect.x = 0;
	if (rect.x + rect.width > size.width)
		rect.width = size.width - rect.x;
	if (rect.y < 0)
		rect.y = 0;
	if (rect.y + rect.height > size.height)
		rect.height = size.height - rect.y;
	if (rect.width <= 0 || rect.height <= 0)
		return false;
	return true;
}

//设定图片，进行预处理,处理完的二值图在maskImage里
void setImage(const cv::Mat & src)
{
	_size = src.size();
	const cv::Point & last_result = _res_last.center;//定义last_result为上一帧矩形的中心
	if (last_result.x == 0 || last_result.y == 0)//如果上一帧记录不存在
	{
		_src = src;
		_dect_rect = Rect(0, 0, src.cols, src.rows);//定义当前搜索矩形为全屏
	}
	else
	{
		Rect rect = _res_last.boundingRect();//定义rect为上一帧矩形
											 //定义扩大的矩形搜索范围
		int max_half_w = max_light_delta_h * 1.3;
		int max_half_h = 300;
		double scale = src.rows == 480 ? 1.8 : 2.5;

		int exp_half_w = min(max_half_w / 2, int(rect.width * scale));
		int exp_half_h = min(max_half_h / 2, int(rect.height * scale));

		int w = std::min(max_half_w, exp_half_w);
		int h = std::min(max_half_h, exp_half_h);
		//定义center为上一帧矩形中心
		Point center = last_result;
		//定义扩大的矩形左上角坐标为lu,left-up
		int x = std::max(center.x - w, 0);
		int y = std::max(center.y - h, 0);
		Point lu = Point(x, y);
		//定义扩大的矩形右下角坐标为rd,right-down
		x = std::min(center.x + w, src.cols);
		y = std::min(center.y + h, src.rows);
		Point rd = Point(x, y);
		//定义当前搜索矩形
		_dect_rect = Rect(lu, rd);
		//如果搜索矩形范围不安全，搜索范围设定为全屏
		if (makeRectSafe(_dect_rect, src.size()) == false)
		{
			_res_last = cv::RotatedRect();
			_dect_rect = Rect(0, 0, src.cols, src.rows);
			_src = src;
		}
		//如果搜索矩形范围安全，搜索矩形拷贝给_src
		else
			src(_dect_rect).copyTo(_src);
	}
	//预处理（二值化）
	int total_pixel = _src.cols * _src.rows;
	const uchar * ptr_src = _src.data;
	const uchar * ptr_src_end = _src.data + total_pixel * 3;
	//创建单颜色通道空间_g,_ec,_max，遍历敌方目标颜色通道像素，并在_max_color中进行二值化赋值
	_g.create(_src.size(), CV_8UC1);
	_ec.create(_src.size(), CV_8UC1);
	maskImage = cv::Mat(_src.size(), CV_8UC1, cv::Scalar(0));
	uchar *ptr_g = _g.data, *ptr_ec = _ec.data, *ptrmaskImage = maskImage.data;
	if (enemy_color == RED)
	{
		for (; ptr_src != ptr_src_end; ++ptr_src, ++ptr_g, ++ptrmaskImage, ++ptr_ec) {
			uchar b = *ptr_src;
			uchar g = *(++ptr_src);
			uchar r = *(++ptr_src);
			*ptr_g = g;
			*ptr_ec = r;
			//*ptr_g = b;
			if (r > min_light_gray)
				*ptrmaskImage = 255;
			//            if (r - b > _para.br_threshold && r >= g)
			//                *ptr_max_color = 255;
		}
	}
	else {
		for (; ptr_src != ptr_src_end; ++ptr_src, ++ptr_g, ++ptrmaskImage, ++ptr_ec) {
			uchar b = *ptr_src;
			uchar g = *(++ptr_src);
			uchar r = *(++ptr_src);
			*ptr_g = g;
			*ptr_ec = b;
			//*ptr_g = r;
			if (b > min_light_gray)
				*ptrmaskImage = 255;
			//            if (b - r > _para.br_threshold && b >= g)
			//                *ptr_max_color = 255;
		}
	}
	//腐蚀膨胀
	Mat element1 = getStructuringElement(MORPH_RECT, Size(2, 3));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));
	erode(maskImage, maskImage, element2, cv::Point(-1, -1), 1);
	dilate(maskImage, maskImage, element1, cv::Point(-1, -1), 1);

}


float dist(float ax, float ay, float bx, float by)
{
	float d = sqrt((ax - bx)*(ax - bx) + (ay - by)*(ay - by));
	return d;
}

float angle(float x1, float y1, float x2, float y2)
{
	float angle_temp;
	float xx, yy;

	xx = x2 - x1;
	yy = y2 - y1;

	if (xx == 0.0)
		angle_temp = 0;
	else
		angle_temp = atan(fabs(yy / xx));
	angle_temp = angle_temp * 180 / PI;
	if (angle_temp>45)
		angle_temp -= 90;
	return (angle_temp);
}

int main()
{
	int countFrame = 0;
	//读入视频
	VideoCapture capture("F://out.avi");
	//"/home/ubuntu/zst/out.avi"
	if (!capture.isOpened())
	{
		cout << "a" << endl;
		return 0;
	}
	Mat srcImage;

	while (capture.read(srcImage))
	{
		countFrame++;
		double start = (double)getTickCount();

		setImage(srcImage);

		vector<vector<Point> >contours, contours2, contours3, contours4;
		findContours(maskImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

		for (int i = 0; i < contours.size(); i++)
		{
			float d[4];
			float height, width;
			RotatedRect rect = minAreaRect(contours[i]);
			Rect rect1 = boundingRect(contours[i]);
			Point2f vertex[4];
			rect.points(vertex);
			//Point center=rect.center;

			if (rect1.height>rect1.width)
			{
				for (int i = 0; i<4; i++)
				{
					d[i] = dist(vertex[i].x, vertex[i].y, vertex[(i + 1) % 4].x, vertex[(i + 1) % 4].y);
				}

				float angle1 = angle(vertex[0].x, vertex[0].y, vertex[1].x, vertex[1].y);

				if (d[1]>d[0])
				{
					height = d[1];
					width = d[0];
				}
				else
				{
					height = d[0];
					width = d[1];
				}

				if (height>width*1.5
					&&angle1<30
					&& contours[i].size() > 10
					)
				{

					cout << "height:" << height << ",width:" << width << '\t';
					cout << "角度" << angle1 << endl;

					for (int i = 0; i<4; i++)
					{
						line(srcImage, vertex[i], vertex[(i + 1) % 4], Scalar(0, 255, 0), 2, LINE_AA);
					}
					contours2.push_back(contours[i]);
				}
			}
		}


		//得到所有轮廓的面积
		vector<double> area, length;
		area.resize(contours2.size());

		for (int i = 0; i<contours2.size(); i++)
			area[i] = contourArea(contours2[i]);

		float box_up_angle = 0, box_down_angle = 0, box_left_angle = 0, box_right_angle = 0;
		float delta_up_down_angle = 0;
		float delta_left_right_angle = 0;

		for (int i = 0; i < contours2.size(); i++)
		{
			RotatedRect rect_i = minAreaRect(contours2[i]);//寻找i最小矩形
			Point2f vertice_i[4];
			rect_i.points(vertice_i);

			for (int j = i + 1; j < contours2.size(); j++)
			{
				RotatedRect rect_j = minAreaRect(contours2[j]);//寻找j最小矩形
				Point2f vertice_j[4];
				rect_j.points(vertice_j);

				Point2f up_point_i = vertice_i[0], down_point_i = vertice_i[0],
					up_point_j = vertice_j[0], down_point_j = vertice_j[0];
				//得到两个矩形最高点与最低点
				for (int m = 0; m < 4; m++)
				{
					if (up_point_i.y > vertice_i[m].y)
						up_point_i = vertice_i[m];
					if (down_point_i.y < vertice_i[m].y)
						down_point_i = vertice_i[m];
				}
				for (int n = 0; n < 4; n++)
				{
					if (up_point_j.y > vertice_i[n].y)
						up_point_j = vertice_j[n];
					if (down_point_j.y < vertice_i[n].y)
						down_point_j = vertice_j[n];
				}

				//得到两个矩形的高
				double height_i = max(rect_i.size.height, rect_i.size.width);
				double height_j = max(rect_j.size.height, rect_j.size.width);

				//得到两个矩形中心距、平均高与其比值
				double distance_center = dist(rect_i.center.x, rect_i.center.y, rect_j.center.x, rect_j.center.y);
				double averageHeight = (height_i + height_j) / 2;
				double deltaY = distance_center / averageHeight;

				//得到上边沿斜率
				if ((up_point_i.x - up_point_j.x) == 0)
					box_up_angle = 0;
				else
					box_up_angle = angle(up_point_i.x, up_point_i.y, up_point_j.x, up_point_j.y);
				//得到下边沿斜率
				if ((down_point_i.x - down_point_j.x) == 0)
					box_down_angle = 0;
				else
					box_up_angle = angle(down_point_i.x, down_point_i.y, down_point_j.x, down_point_j.y);
				//得到左边沿斜率
				if (vertice_i[0].x - vertice_i[1].x == 0)
					box_left_angle = 0;
				else
					box_left_angle = angle(vertice_i[0].x, vertice_i[0].y, vertice_i[1].x, vertice_i[1].y);
				//得到右边沿斜率
				if (vertice_j[0].x - vertice_j[1].x == 0)
					box_right_angle = 0;
				else
					box_right_angle = angle(vertice_j[0].x, vertice_j[0].y, vertice_j[1].x, vertice_j[1].y);

				//得到斜率的差值
				delta_up_down_angle = abs(box_up_angle - box_down_angle);
				delta_left_right_angle = abs(box_left_angle - box_right_angle);

				//条件筛选
				if (box_right_angle <30
					&& deltaY <6
					&& delta_up_down_angle <7
					&& (delta_left_right_angle <5))
				{
					contours3.push_back(contours2[i]);
					contours3.push_back(contours2[j]);
				             //    line(srcImage,up_point_i,up_point_j,Scalar(255,0,0),2,LINE_AA);
								//line(srcImage,up_point_j,down_point_j,Scalar(255,0,0),2,LINE_AA);
				            //      line(srcImage,down_point_j,down_point_i,Scalar(255,0,0),2,LINE_AA);
				              //   line(srcImage,down_point_i,up_point_i,Scalar(255,0,0),2,LINE_AA);
				}
			}
		}

		vector<Point> contours_result;

		if (contours3.size()>1)
		{
			int contours_i_size = contours3[0].size();
			int contours_j_size = contours3[1].size();
			contours_result.resize(contours_i_size + contours_j_size);

			for (int m = 0; m < contours_i_size; m++)
				contours_result[m] = contours3[0][m];
			for (int n = 0; n < contours_j_size; n++)
				contours_result[n + contours_i_size] = contours3[1][n];

			RotatedRect rect_result = minAreaRect(contours_result);
			Rect standard_rect = boundingRect(contours_result);
			Point2f vertex_result[4];
			rect_result.points(vertex_result);
			if (standard_rect.width>standard_rect.height * 2
				&& standard_rect.height * 8>standard_rect.width)
			{
				for (int i = 0; i<4; i++)
				{
					line(srcImage, vertex_result[i], vertex_result[(i + 1) % 4], Scalar(255, 0, 0), 2, LINE_AA);
				}
			}
		}


		imshow("srcImage", srcImage);
		imshow("maskImage", maskImage);

		double end = (double)getTickCount();
		cout << "Frame: " << countFrame << "\tTime: ";
		cout << (end - start) * 1000 / (getTickFrequency()) << "ms" << endl;
		//waitKey();
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}
