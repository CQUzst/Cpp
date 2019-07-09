#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <windows.h>

using namespace std;
using namespace cv;
int main(){
    ifstream infile;
    infile.open("D://teamWork//boundingBox_points.txt");//数据预处理，读入所有帧的points
    //allFramePoints[i][j].first.x,第一个下表代表帧数，第二个下表代表第i帧的第j个矩形
    //first代表左上角坐标,second代表右下角坐标
    vector<vector<pair<Point,Point>>> allFramePoints;

    while(!infile.eof()){
        string str,tmp;
        vector<int> vec;
        vector<pair<Point,Point>> oneFramePoints;
        getline(infile,str);
        for(size_t i=0;i<str.size();i++){
            if(isdigit(str[i])){
                tmp+=str[i];
            }
            else if(i>1&&str[i]==' '&&isdigit(str[i-1])){
                vec.emplace_back(atoi(tmp.c_str()));
                tmp.clear();
            }
            else if(str[i]==' ')
                continue;
            else{
                vec.emplace_back(atoi(tmp.c_str()));
                tmp.clear();
            }
        }
        Point upLedtPoint,dowmRightPoint;
        for(size_t i=0;i<vec.size();i+=4){
            upLedtPoint.x=vec[i];
            upLedtPoint.y=vec[i+1];
            dowmRightPoint.x=vec[i+2];
            dowmRightPoint.y=vec[i+3];
            oneFramePoints.push_back(make_pair(upLedtPoint,dowmRightPoint));
        }
        allFramePoints.push_back(oneFramePoints);
    }
    infile.close();

    long startTime = GetTickCount();
    vector<Point> trackPoints;//轨迹记录点集
    vector<Point> fixedTrackPoints;//轨迹记录点集
    //frame=2100;frame<2500
    for(int frame=3000;frame<3500;frame++){
        Mat srcImage;
        char numS[5];
        sprintf(numS,"%05d",frame);
        string numStr=numS;
        string frameStr="D://teamWork//originPicture//"+numStr+".jpg";//D://teamWork//originPicture//boundBOX
        srcImage=imread(frameStr);
        Mat showImage=srcImage.clone();

        vector<Point> oneFrameTrackPoints;
        vector<Mat> matVec;//ROIs matVec.size()==trackPoints.size()
        vector<Point> matVecPoint;//ROIs matVec.size()==trackPoints.size()
        //矩形中心画点
        for(size_t i=0;i<allFramePoints[frame].size();i++){
            int width=allFramePoints[frame][i].second.x-allFramePoints[frame][i].first.x;
            int height=allFramePoints[frame][i].second.y-allFramePoints[frame][i].first.y;
            Point center;
            center.x=allFramePoints[frame][i].first.x+width/2;
            center.y=allFramePoints[frame][i].first.y+height/2;
            oneFrameTrackPoints.push_back(center);
            trackPoints.push_back(center);
            int ROIwidth=width+width/5;
            int ROIheight=height+height/5;
            if(allFramePoints[frame][i].first.x+ROIwidth>=srcImage.cols)
                ROIwidth=srcImage.cols-allFramePoints[frame][i].first.x;
            if(allFramePoints[frame][i].first.y+ROIheight>=srcImage.rows)
                ROIheight=srcImage.rows-allFramePoints[frame][i].first.y;
            Mat imageROI=srcImage(Rect(max(0,allFramePoints[frame][i].first.x-width/10),
                                       max(0,allFramePoints[frame][i].first.y-height/10),
                                       ROIwidth,ROIheight));
            matVecPoint.push_back(Point(allFramePoints[frame][i].first.x,allFramePoints[frame][i].first.y));
            matVec.push_back(imageROI.clone());
            //cv::circle(srcImage, center, 3, cv::Scalar(0, 0, 255),-1);//在图像中画出特征点
        }

        ///针对所有ROI进行图像处理
        vector<Point>allContourPoint;
        for(size_t i=0;i<matVec.size();i++){
            //滤波函数
            //blur(matVec[i], matVec[i], Size(5, 5),Point(-1,-1));
            //medianBlur(matVec[i], matVec[i],5);
            Mat tmpMat=matVec[i].clone();
            bilateralFilter(matVec[i],tmpMat,5,100,5);//双边滤波保留轮廓
            cvtColor(tmpMat, tmpMat, COLOR_BGR2GRAY);//灰度化
            //threshold(matVec[i],matVec[i],120,255,THRESH_OTSU);
            Canny(tmpMat, tmpMat, 100, 200, 3);
            for(int x=0;x<tmpMat.cols;x++){
                for(int y=0;y<tmpMat.rows;y++){
                    if(x<tmpMat.cols/10||x>tmpMat.cols*9/10||y<tmpMat.rows/10||y>tmpMat.cols*9/10)
                        tmpMat.at<uchar>(y, x)=0;
                }
            }
            Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
            dilate(tmpMat, tmpMat, element);
            erode(tmpMat,tmpMat,element);

            vector<vector<Point>> contours;
            vector<Point> allContours;
            findContours(tmpMat, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

            for(int j=0;j<contours.size();j++)
                for(int k=0;k<contours[j].size();k++)
                    allContours.push_back(contours[j][k]);
            if(allContours.size()<6)
                continue;
            RotatedRect fitEllipseBox = fitEllipse(allContours);
            int fitX=matVecPoint[i].x+fitEllipseBox.center.x;
            int fitY=matVecPoint[i].y+fitEllipseBox.center.y;
            //cout<<"fitEllipseBox.x="<<fitEllipseBox.center.x<<"  "<<fitEllipseBox.center.y<<endl;
            //circle(showImage, Point(fitX,fitY), 2, cv::Scalar(0, 255, 255),-1);//在图像中画出特征点
            fitEllipseBox.center.x+=matVecPoint[i].x;
            fitEllipseBox.center.y+=matVecPoint[i].y;
            //ellipse(showImage, fitEllipseBox, Scalar(255,0,255),2);

            //cout<<"sumWhitePoint="<<sumWhitePoint<<"x="<<averageX<<"y="<<averageY<<endl;
            Point contourPoint;
            contourPoint.x=fitX;
            contourPoint.y=fitY;
            allContourPoint.push_back(contourPoint);
            //circle(showImage, contourPoint, 2, cv::Scalar(255, 0, 0),-1);//在图像中画出特征点
            //circle(mask, Point(averageX,averageY), 2, cv::Scalar(0, 255, 0),-1);//在图像中画出特征点
            //matVec[i].at<uchar>(averageY, averageX)=255;
            //显示roi图片
            string windowName="roi";
            windowName+=to_string(i);
            //imshow(windowName,tmpMat);
            //imshow(windowName,matVec[i]);
        }
        for(size_t i=0;i<matVec.size();i++){
            Point outPoint;
            int deltaX=allContourPoint[i].x-oneFrameTrackPoints[i].x;
            int deltaY=allContourPoint[i].y-oneFrameTrackPoints[i].y;
            outPoint.x=oneFrameTrackPoints[i].x+0.5*deltaX;
            outPoint.y=oneFrameTrackPoints[i].y+0.5*deltaY;
            fixedTrackPoints.push_back(outPoint);
            //circle(showImage, outPoint, 2, cv::Scalar(0, 255, 0),-1);//在图像中画出特征点
        }

        //画轨迹点
        for(size_t i=0;i<trackPoints.size();i++){
            circle(showImage, trackPoints[i], 2, cv::Scalar(0, 0, 255),-1);//在图像中画出特征点
            circle(showImage, fixedTrackPoints[i], 2, cv::Scalar(0, 255, 0),-1);
            //            srcImage.at<Vec3b>(trackPoints[i].y,trackPoints[i].x)[0] = 255;
            //            srcImage.at<Vec3b>(trackPoints[i].y,trackPoints[i].x)[1] = 250;
            //            srcImage.at<Vec3b>(trackPoints[i].y,trackPoints[i].x)[2] = 0;
        }

        imshow("show",showImage);
        waitKey(1);
    }
    long endTime = GetTickCount();
    cout<<"cost time="<<endTime-startTime<<"ms"<<endl;
    return 0;
}

