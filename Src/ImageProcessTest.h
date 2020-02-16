//////////////////////////////////////////////////////////////////////////
// ������ ImageProcessTest.h 

//////////////////////////////////////////////////////////////////////////

#ifndef __QQ_IMAGEPROCESS_TEST_H__
#define __QQ_IMAGEPROCESS_TEST_H__

#include "opencv2/opencv.hpp"
#include"All.h"
#include<stdlib.h>
#include <fstream> 
#include <string> 
using namespace cv;
using namespace std;


////////////////////////////ͼ��·��//////////////////////////////////////////////
#ifdef _WIN32
#define LENA_GRAY		"../../../../Resource/Image/Gray/Lena512.bmp"
#define LENA_COLOR		"../../../../Resource/Image/Color/Lena800.bmp"
#define BEAUTY_COLOR	"../../../../Resource/Image/Color/Beauty.bmp"
#define BEAUTY_GRAY		"../../../../Resource/Image/Gray/Beauty.bmp"
#define CAMERA_GRAY		"../../../../Resource/Image/Gray/Camera256.bmp"
#define CAMERA_COLOR	"../../../../Resource/Image/Color/Camera256.bmp"
#else
#define LENA_GRAY		"../../../Resource/Image/Gray/Lena512.bmp"
#define LENA_COLOR		"../../../Resource/Image/Color/Lena800.bmp"
#define BEAUTY_COLOR	"../../../Resource/Image/Color/Beauty.bmp"
#define BEAUTY_GRAY		"../../../Resource/Image/Gray/Beauty.bmp"
#define CAMERA_GRAY		"../../../Resource/Image/Gray/Camera256.bmp"
#define CAMERA_COLOR	"../../../Resource/Image/Color/Camera256.bmp"
#endif

//Դͼ��·��
#define GRAY	"D:/Image/Gray/"
#define COLOR	"D:/Image/Color/"

//Ŀ��ͼ��·��
#define RESULT			"D:/Image/Temp/"

//�Ƕ�ת��
#define DEGREE2RADIAN(x) (x*CV_PI/180)//�Ƕ�ת����
#define RADIAN2DEGREE(x) (x*180/CV_PI)//����ת�Ƕ�

using namespace cv;

class ImageProcessTest
{
public:
	// �򵥵Ĳ����㷨��ƽ��ʱ��
	void TestMeanTime();


	void TestAccessImage();//���Է���ͼ��ĸ��ַ���
	void TestConnectImage();//����ƴ��ͼ��
	void TestProject();//����ͶӰ
	void TestMat_IplImage4ALigned();//����Mat��IplImage�Ƿ�4�ֽڶ���
	void TestDrawHist();//���Ի�ֱ��ͼ
	void TestImageMedianMinMax();//����ͼ���������ֵ�����ֵ����Сֵ
	void TestSmooth();//�����Լ����˲���
	void TestConvolution();// ���Ծ��
	void TestEdge();//���Ա�Ե���
	void TestCanny();
	void TestGeometryTransformation();//���Լ��α任
	void TestRotate();
	void TestCvtColor(); // ���ԻҶȱ任
	void TestEqualiz();
	void TestThin();
	void TestGaussianBlur();
	void TestGradientHist();

	// LBP�Ĳ���
	void LBP_SVM();
	void LBP_SVM_Rotation();
	void TestLBP(); // ����LBP����

};

#endif



