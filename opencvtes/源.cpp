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
void CameraCalibrate(Size, string, Mat&, Mat&, Mat&, Mat&);//����궨
void DistanceSolve(Mat, Mat, Mat, Mat&, Mat&);//���
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
	vector<vector<Point3f> > objpoints;    // ��������ϵ�нǵ����ά����
	vector<vector<Point2f> > imgpoints;		// ͼ����������ϵ�нǵ�Ķ�ά����
	vector<Point3f> objp;   // Ϊ�ǵ㶨��������������ϵ�е�����
	vector<String> images;  // ��ȡ�洢�ڸ���Ŀ¼�еĵ���ͼ���·��//�ɸ���ͼ���·����ɵ�String����
	vector<Point2f> corner_pts;// �ǵ���ͼ����������ϵ�еĶ�άλ��
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
		}// ѭ����ȡͼ��
		cout << "the current image is " << i << "th" << endl;
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		success = findChessboardCorners(gray, patsize, corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
		cout << corner_pts << endl;
		cout << success << endl;
		if (success)
		{
			TermCriteria criteria(TermCriteria::EPS | TermCriteria::Type::MAX_ITER, 30, 0.001);// ʵ����TermCriteria�����criteria
			cornerSubPix(gray, corner_pts, Size(3, 3), Size(-1, -1), criteria);	// Ϊ�����Ķ�ά��ϸ����������				
			drawChessboardCorners(frame, patsize, corner_pts, success);// ����������ʾ��⵽�Ľǵ�
			objpoints.push_back(objp);//����·�����жัͼ�񣬾Ͷѵ��˶����ǵ㶨��õ���ά��������ϵ
			imgpoints.push_back(corner_pts);//����·�����жัͼ�񣬾Ͷѵ��˶��ǵ��Ӧ��ͼ����������
		}
		imshow("Image", frame);
		waitKey(0);
	}
	destroyAllWindows();
	calibrateCamera(objpoints, imgpoints, Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);
	a = cameraMatrix; b = distCoeffs; c = R; d = T;
	cout << "cameraMatrix : " << cameraMatrix << endl;
	cout << "distCoeffs : " << distCoeffs << endl; //͸������ϵ��
	cout << "Rotation vector : " << R << endl;
	cout << "Translation vector : " << T << endl;// ͨ��������֪3D�㣨objpoints����ֵ�ͼ�⵽�Ľǵ㣨imgpoints������Ӧ����������ִ�����У׼

}
void DistanceSolve(Mat frame, Mat cameraMatrix, Mat distCoeffs, Mat& R, Mat& T)
{
	Mat edge;  Mat edgesBlur; Mat srcBinary;
	vector<vector<Point>>contours;
	cvtColor(frame, edge, COLOR_BGR2GRAY);
	GaussianBlur(edge, edgesBlur, Size(7, 7), 1.5, 1.5);

	vector<Vec4i> hireachy;
	findContours(edgesBlur, contours, hireachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point> > balls;//��Ч��������
	vector<RotatedRect> ballsBox;//��Ч�߿򼯺�
	vector<vector<Point2f>> box_pointsSet;
	vector<Point2f>vertices(4);//����

	for (size_t i = 0; i < contours.size(); i++)
	{
		RotatedRect bBox;

		bBox = minAreaRect(contours[i]);//���������Ʊ߿�bBox
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
	}//����1��findcontours���ֻ�ҵ������������������ͻᱣ�棨�ڴ棩
	 //Ӧ�����������Ч��⵽����

	cout << "Balls found:" << ballsBox.size() << endl;//�������������Ŀ����

	for (size_t i = 0; i < balls.size(); i++)
	{
		drawContours(frame, balls, i, CV_RGB(20, 150, 20), 1);

		for (int j = 0; j < box_pointsSet.size(); j++)
		{
			//for (int k = 0; k < 4; j++)
			//{
				//line(frame, box_pointsSet[j][k], box_pointsSet[j][(k + 1) % 4], Scalar(0, 0, 255), 3);
			//}  ����2��line�������鱨��������̫���ԭ��
			//rectangle(frame, ballsBox[i], CV_RGB(0, 255, 0), 2);

			//Point center;
			//center.x = ballsBox[i].size.x + ballsBox[i].width / 2;
			//center.y = ballsBox[i].y + ballsBox[i].height / 2;
			circle(frame, ballsBox[i].center, 2, CV_RGB(20, 150, 20), -1);//��ɸѡ���Ŀ�������Բ

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
		sstr << "Distance:" << P.at<double>(2, 0);//����3��ע�������P��ֻ��һ��
		putText(frame, sstr.str(), Point(ballsBox[i].center), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 0, 255), 2);

	}
	imshow("dangqian", frame);
	waitKey(0);
	//drawContours(frame, contours, -1, Scalar(0,0,255), 2);
}