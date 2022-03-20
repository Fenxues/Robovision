#include<iostream>
#include<opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/calib3d/calib3d.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#define MIN_H_BLUE 200
#define MAX_H_BLUE 300
using namespace std;
using namespace cv;
void CameraCalibrate(Size, string, Mat&, Mat&, Mat&, Mat&);//相机标定
void DistanceSolve(Mat, Mat, Mat, Mat&, Mat&);//测距
int main()
{

	Size pats;
	pats.height = 6;
	pats.width = 9;
	string path = "C:\\Users\\DELL\\Desktop\\R-C.bmp";
	Mat cameraMatrix, distCoeffs, R, T;
	CameraCalibrate(pats, path, cameraMatrix, distCoeffs, R, T);

	Mat frame = imread("C:\\Users\\DELL\\Desktop\\CCC.bmp");
	DistanceSolve(frame, cameraMatrix, distCoeffs, R, T);


}


void CameraCalibrate(Size patsize, string path, Mat& a, Mat& b, Mat& c, Mat& d)
{
	vector<vector<Point3f> > objpoints;    // 世界坐标系中角点的三维坐标
	vector<vector<Point2f> > imgpoints;		// 图像像素坐标系中角点的二维坐标
	vector<Point3f> objp;   // 为角点定义其在世界坐标系中的坐标
	vector<String> images;  // 提取存储在给定目录中的单个图像的路径//由各个图像的路径组成的String数组
	vector<Point2f> corner_pts;// 角点在图像像素坐标系中的二维位置
	Mat frame, gray;
	Mat cameraMatrix, distCoeffs, R, T;
	bool success;
	for (int i{ 0 }; i < patsize.height; i++)
	{
		for (int j{ 0 }; j < patsize.width; j++)
		{
			objp.push_back(Point3f(i, j, 0));
		}
	}
	glob(path, images);
	for (int i{ 0 }; i < images.size(); i++)
	{
		frame = imread(images[i]);
		if (frame.empty())
		{
			continue;
		}
		if (i == 10)
		{
			int b = 1;
		}// 循环读取图像
		cout << "the current image is " << i << "th" << endl;
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		success = findChessboardCorners(gray, patsize, corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
		cout << corner_pts << endl;
		cout << success << endl;
		if (success)
		{
			TermCriteria criteria(TermCriteria::EPS | TermCriteria::Type::MAX_ITER, 30, 0.001);// 实例化TermCriteria类对象criteria
			cornerSubPix(gray, corner_pts, Size(3, 3), Size(-1, -1), criteria);	// 为给定的二维点细化像素坐标				
			drawChessboardCorners(frame, patsize, corner_pts, success);// 在棋盘上显示检测到的角点
			objpoints.push_back(objp);//由于路径中有多副图像，就堆叠了多层给角点定义好的三维世界坐标系
			imgpoints.push_back(corner_pts);//由于路径中有多副图像，就堆叠了多层角点对应的图像像素坐标
		}
		imshow("Image", frame);
		waitKey(0);
	}
	destroyAllWindows();
	calibrateCamera(objpoints, imgpoints, Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);
	a = cameraMatrix; b = distCoeffs; c = R; d = T;
	cout << "cameraMatrix : " << cameraMatrix << endl;
	cout << "distCoeffs : " << distCoeffs << endl; //透镜畸变系数
	cout << "Rotation vector : " << R << endl;
	cout << "Translation vector : " << T << endl;// 通过传递已知3D点（objpoints）的值和检测到的角点（imgpoints）的相应像素坐标来执行相机校准

}
void DistanceSolve(Mat frame, Mat cameraMatrix, Mat distCoeffs, Mat& R, Mat& T)
{
	Mat edge;  Mat edgesBlur; Mat srcBinary;
	vector<vector<Point>>contours;
	cvtColor(frame, edge, COLOR_BGR2GRAY);
	GaussianBlur(edge, edgesBlur, Size(7, 7), 1.5, 1.5);

	vector<Vec4i> hireachy;
	findContours(edgesBlur, contours, hireachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point> > balls;//有效轮廓集合
	vector<RotatedRect> ballsBox;//有效边框集合
	vector<vector<Point2f>> box_pointsSet;
	vector<Point2f>vertices(4);//顶点

	for (size_t i = 0; i < contours.size(); i++)
	{
		RotatedRect bBox;

		bBox = minAreaRect(contours[i]);//对轮廓绘制边框bBox
		float ratio = (float)bBox.size.width / (float)bBox.size.height;
		if (ratio > 1.0f)
			ratio = 1.0f / ratio;
		if (contourArea(contours[i]) >= 200)
		{
			balls.push_back(contours[i]);
			ballsBox.push_back(bBox);
			bBox.points(vertices.data());
			box_pointsSet.push_back(vertices);
		}
	}//问题1：findcontours如果只找到了最外层轮廓，这里就会保存（内存）
	 //应当检查有无有效检测到轮廓

	cout << "Balls found:" << ballsBox.size() << endl;//输出满足条件的目标物

	for (size_t i = 0; i < balls.size(); i++)
	{
		drawContours(frame, balls, i, CV_RGB(20, 150, 20), 1);

		for (int j = 0; j < box_pointsSet.size(); j++)
		{
			//for (int k = 0; k < 4; j++)
			//{
				//line(frame, box_pointsSet[j][k], box_pointsSet[j][(k + 1) % 4], Scalar(0, 0, 255), 3);
			//}  问题2：line这里数组报错，还不是太清楚原因
			//rectangle(frame, ballsBox[i], CV_RGB(0, 255, 0), 2);

			//Point center;
			//center.x = ballsBox[i].size.x + ballsBox[i].width / 2;
			//center.y = ballsBox[i].y + ballsBox[i].height / 2;
			circle(frame, ballsBox[i].center, 2, CV_RGB(20, 150, 20), -1);//对筛选后的目标物绘制圆

			stringstream sstr;
			sstr << "(" << ballsBox[i].center << ")";
			putText(frame, sstr.str(), Point(ballsBox[i].center.x + 3, ballsBox[i].center.y - 3), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20, 150, 20), 2);
		}

		Mat objpoints;
		objpoints.push_back(Point3f(-5.0, -2.5, 0));
		objpoints.push_back(Point3f(+5.0, -2.5, 0));
		objpoints.push_back(Point3f(+5.0, +2.5, 0));
		objpoints.push_back(Point3f(-5.0, +2.5, 0));

		solvePnP(objpoints, box_pointsSet[i], cameraMatrix, distCoeffs, R, T);
		Mat rotM;
		Rodrigues(R, rotM);
		Mat P;
		P = (rotM.t()) * T;

		stringstream sstr;
		sstr << "Distance:" << P.at<double>(2, 0);//问题3：注意这里的P；只有一列
		putText(frame, sstr.str(), Point(ballsBox[i].center), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 0, 255), 2);

	}
	imshow("dangqian", frame);
	waitKey(0);
	//drawContours(frame, contours, -1, Scalar(0,0,255), 2);
}