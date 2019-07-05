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
    for(int frame=600;frame<900;frame++){
        Mat srcImage;
        char numS[5];
        sprintf(numS,"%05d",frame);
        string numStr=numS;
        string frameStr="D://teamWork//boundBOX//"+numStr+".jpg";//D://teamWork//originPicture//
        srcImage=imread(frameStr);
        //Mat showImage=srcImage.clone();

        //矩形中心画点
        for(size_t i=0;i<allFramePoints[frame].size();i++){
            int width=allFramePoints[frame][i].second.x-allFramePoints[frame][i].first.x;
            int height=allFramePoints[frame][i].second.y-allFramePoints[frame][i].first.y;
            Point center;
            center.x=allFramePoints[frame][i].first.x+width/2;
            center.y=allFramePoints[frame][i].first.y+height/2;
            trackPoints.push_back(center);
            //cv::circle(srcImage, center, 3, cv::Scalar(0, 0, 255),-1);//在图像中画出特征点
        }

        //画轨迹点
        for(int i=0;i<trackPoints.size();i++){
            circle(srcImage, trackPoints[i], 1, cv::Scalar(0, 0, 255),-1);//在图像中画出特征点
//            srcImage.at<Vec3b>(trackPoints[i].y,trackPoints[i].x)[0] = 255;
//            srcImage.at<Vec3b>(trackPoints[i].y,trackPoints[i].x)[1] = 250;
//            srcImage.at<Vec3b>(trackPoints[i].y,trackPoints[i].x)[2] = 0;
        }

        imshow("show",srcImage);
        waitKey(0);
    }
    long endTime = GetTickCount();
    cout<<"cost time="<<endTime-startTime<<"ms"<<endl;
    waitKey(0);
    return 0;
}
//    vector<Mat> matVec;
//    for(int i=0;i<allPoints.size();i+=2){
//        int width=allPoints[i+1].x-allPoints[i].x;
//        int height=allPoints[i+1].y-allPoints[i].y;
//        Point center;
//        center.x=allPoints[i].x+width/2;
//        center.y=allPoints[i].y+height/2;
//        //            cout<<"centerPoint.x="<<allPoints[i].x+width/2
//        //               <<","<<"centerPoint.y="<<allPoints[i].y+height/2<<endl;
//        Mat imageROI=srcImage(Rect(allPoints[i].x,allPoints[i].y,width,height));

//        cv::circle(showImage, center, 3, cv::Scalar(0, 0, 255),-1);//在图像中画出特征点
//        matVec.emplace_back(imageROI);
//    }
//    imshow("showImage",showImage);
//    for(int i=0;i<matVec.size()/2;i++){
//        //针对所有ROI进行图像处理
//        blur(matVec[i], matVec[i], Size(5, 5),Point(-1,-1));




//            //显示roi图片
//            string windowName="roi";
//            windowName+=to_string(i);
//            imshow(windowName,matVec[i]);
//        }
//        waitKey(0);
//        cout<<endl;
//    }
