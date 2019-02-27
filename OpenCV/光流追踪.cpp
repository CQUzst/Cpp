#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <cstdio>
using namespace std;
using namespace cv;
//  ����������ȫ�ֺ���
void tracking(Mat &frame, Mat &output);
bool addNewPoints();
bool acceptTrackedPoint(int i);
//  ����������ȫ�ֱ���
string window_name = "optical flow tracking";
Mat gray;   // ��ǰͼƬ
Mat gray_prev;  // Ԥ��ͼƬ
vector<Point2f> points[2];  // point0Ϊ�������ԭ��λ�ã�point1Ϊ���������λ��
vector<Point2f> initial;    // ��ʼ�����ٵ��λ��
vector<Point2f> features;   // ��������
int maxCount = 500; // �������������
double qLevel = 0.01;   // �������ĵȼ�
double minDist = 10.0;  // ��������֮�����С����
vector<uchar> status;   // ����������״̬��������������Ϊ1������Ϊ0
vector<float> err;
//�����Ӧ��Ϣ��OpenCV�汾-----
static void helpinformation()
{
	cout << "\n\n\t\t\t �����������˶�Ŀ����\n"
		<< "\n\n\t\t\t ��ǰʹ�õ�OpenCV�汾Ϊ��" << CV_VERSION
		<< "\n\n";
}

//main( )�������������
int main()
{
	Mat frame;
	Mat result;
	//����ʹ�õ���Ƶ�ļ���������Ŀ���������ļ���
	VideoCapture capture("F:\\outColor.avi");
	//��ʾ��Ϣ����
	helpinformation();
	// ����ͷ��ȡ�ļ�����
	if (capture.isOpened())
	{
		while (true)
		{
			capture >> frame;

			if (!frame.empty())
			{
				tracking(frame, result);
			}
			else
			{
				printf(" --(!) No captured frame -- Break!");
				break;
			}
			int c = waitKey(50);
			if ((char)c == 27)
			{
				break;
			}
		}
	}
	return 0;
}

// parameter: frame �������Ƶ֡
//            output �и��ٽ������Ƶ֡
void tracking(Mat &frame, Mat &output)
{
	cvtColor(frame, gray, CV_BGR2GRAY);
	frame.copyTo(output);
	// ���������
	if (addNewPoints())
	{
		goodFeaturesToTrack(gray, features, maxCount, qLevel, minDist);
		points[0].insert(points[0].end(), features.begin(), features.end());
		initial.insert(initial.end(), features.begin(), features.end());
	}

	if (gray_prev.empty())
	{
		gray.copyTo(gray_prev);
	}
	// l-k�������˶�����
	calcOpticalFlowPyrLK(gray_prev, gray, points[0], points[1], status, err);
	// ȥ��һЩ���õ�������
	int k = 0;
	for (size_t i = 0; i<points[1].size(); i++)
	{
		if (acceptTrackedPoint(i))
		{
			initial[k] = initial[i];
			points[1][k++] = points[1][i];
		}
	}
	points[1].resize(k);
	initial.resize(k);
	// ��ʾ��������˶��켣
	for (size_t i = 0; i<points[1].size(); i++)
	{
		line(output, initial[i], points[1][i], Scalar(0, 0, 255));
		circle(output, points[1][i], 3, Scalar(0, 255, 0), -1);
	}

	// �ѵ�ǰ���ٽ����Ϊ��һ�˲ο�
	swap(points[1], points[0]);
	swap(gray_prev, gray);
	imshow(window_name, output);
}

//  ����µ��Ƿ�Ӧ�ñ����
// return: �Ƿ���ӱ�־
bool addNewPoints()
{
	return points[0].size() <= 10;
}

//������Щ���ٵ㱻����
bool acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
}