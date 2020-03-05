#define DLLAPI_EXPORTS

#include "GeometryTransformation.h"

using namespace std;

namespace QQ
{

//2015-3-5 15:33:20,by QQ
//����ڲ�ֵ
void NearestInterpolation(const Mat &image_Src,Mat &image_Dst,Size size_Dst)
{
	//���·���
	image_Dst.create(size_Dst,CV_8UC1);
	
	//
	int height=size_Dst.height;
	int width=size_Dst.width;

	//���ŵı���
	//1.��Ҫ���ˣ��������������
	float ratioX=1.*image_Src.cols/image_Dst.cols;
	float ratioY=1.*image_Src.rows/image_Dst.rows;

	//
	for(int y=0;y<=height-1;++y)
	{
		//��Ӧԭͼ����(��������)
		float y1=y*ratioY;
		
		//ȡ���õ����������
		int y2=(int)y1;
		
		for (int x=0;x<=width-1;++x)
		{
			//��Ӧ��ԭͼ����
			//�������˫���Ե�������ǣ�˫���Բ�ֵ�ڻ��ԭͼ�ĸ��������������Χ
			//4�������꣬�������ֱ�ӰѸ���������ȡ����������������
			float x1=x*ratioX;
			
			//ȡ���õ����������
			int x2=(int)x1;
			image_Dst.at<uchar>(y,x)=image_Src.at<uchar>(y2,x2);

		}
	}

}


//2015-3-5 13:27:24,by QQ
// ˫���Բ�ֵ
// size_Dst:Ŀ��ͼ��Ĵ�С
void BilinearInterpolation(const Mat &image_Src,Mat &image_Dst,Size size_Dst)
{
	// ���·���
	image_Dst.create(size_Dst,CV_8UC1);

	// ԭͼ�Ŵ󣬱��ڴ���߽�
	Mat image_Extend;
	copyMakeBorder(image_Src,image_Extend,1,1,1,1,BORDER_DEFAULT);

	int height=size_Dst.height;
	int width=size_Dst.width;

	//���ŵı���
	//1.��Ҫ����,�������������
	float ratioX=1.*image_Src.cols/image_Dst.cols;//����ں�˫���Զ���ͨ�����ŵı���
	float ratioY=1.*image_Src.rows/image_Dst.rows;
	
	//
	for(int y=0;y<=height-1;++y)
	{
		//��Ӧԭͼ������(����������)
		float y1=y*ratioY;

		// ������ͼ�е�����
		y1++;

		//ȡ��,Ҳ��������ڵ�����
		//�������˫���Ե�������ǣ�˫���Բ�ֵ�ڻ��ԭͼ�ĸ��������������Χ
		//4�������꣬�������ֱ�ӰѸ���������ȡ����������������
		int y2=(int)y1;

		// ��ֵ
		int dy=(y1-y2)*(1 << SHIFT);
		for (int x=0;x<=width-1;++x)
		{
			//��Ӧ��ԭͼ����
			float x1 = x*ratioX;

			// ������ͼ�е����� 
			x1++;

			//ȡ��,Ҳ��������ڵ�����
			int x2=(int)x1;

			// ��ֵ
			int dx=(x1-x2)*(1 << SHIFT);

			//��Χ4����
			//a�������������
			//a   b
			//  p
			//c   d
			uchar a=image_Extend.at<uchar>(y2,x2);
			uchar b=image_Extend.at<uchar>(y2,x2+1);
			uchar c=image_Extend.at<uchar>(y2+1,x2);
			uchar d=image_Extend.at<uchar>(y2+1,x2+1);
			
			// ˫���Բ�ֵ
			// ��Ҫ������
			//int p = (a*((1 << SHIFT) - dx)*((1 << SHIFT) - dy) + b*dx*((1 << SHIFT) - dy) + c*((1 << SHIFT) - dx)*dy + d*dx*dy)/(1<<(2*SHIFT));
			int p=a*((1 << SHIFT) -dx)*((1 << SHIFT) -dy)+b*dx*((1 << SHIFT) -dy)+c*((1 << SHIFT)-dx)*dy+d*dx*dy;
			p = DESCALE(p, 2 * SHIFT);
			image_Dst.at<uchar>(y,x)=(uchar)p;

		}
	}
}

// 2015-5-16 22:21:28,by QQ
// Updated:2016-5-17 11:38:14,by QQ
void VerticalFlip(const Mat &srcImage, Mat &dstImage)
{
	// �����ڴ�
	CV_Assert(srcImage.depth() == CV_8U);
	dstImage.create(srcImage.size(), srcImage.type());

	// ��ֱ��ת
	int width = srcImage.cols;
	int height = srcImage.rows;
	int widthStep = srcImage.cols*srcImage.channels();

	uchar *dataOfSrc = srcImage.data;
	uchar *dataOfDst = dstImage.data + widthStep*(dstImage.rows - 1);
	int limit = (height - 1) / 2;
	for (int y = 0; y <= height - 1; ++y,dataOfSrc+=widthStep,dataOfDst-=widthStep)
	{
		// ����һ��
		memcpy(dataOfDst, dataOfSrc, widthStep);
	}
}

// 2016-5-17 11:40:18,by QQ
void HorizontalFlip(const Mat &srcImage, Mat &dstImage)
{
	// �����ڴ�
	CV_Assert(srcImage.depth() == CV_8U);
	dstImage.create(srcImage.size(), srcImage.type());

	// ÿһ�н������ұ�(��ÿһ��ͨ����������)
	int width = srcImage.cols;
	int channelCount = srcImage.channels();
	int widthStep = width*channelCount;
	int *table = new int[widthStep];
	for (int i = 0; i <= width - 1; ++i)
	{
		int index1 = i*channelCount;
		int index2 = (width - 1 - i)*channelCount;
		for (int j = 0; j <= channelCount - 1; ++j)
			table[index1 + j] = index2 + j;
	}

	
	uchar *dataOfSrc = srcImage.data;
	uchar *dataOfDst = dstImage.data;
	int limit = ((width + 1) >>1)*channelCount; // ע����������ȼ�,Ϊʲô((width - 1) >>1)*channelCount ���ԣ���ʾ:��Ϊ��ͨ����ԭ�򣬶԰�ʹ��(x+1)/2
	for (int y = 0; y <= srcImage.rows - 1; ++y, dataOfSrc += widthStep, dataOfDst += widthStep)
	{
		for (int x = 0; x <= limit; ++x)
		{
			int index = table[x];
			dataOfDst[index] = dataOfSrc[x];
			dataOfDst[x] = dataOfSrc[index];
		}
	}

	delete[] table;

}

// 2016-8-11 15:49:14,by QQ
/*	center:ԭͼ�����ת����
	dstSize:��ת��ͼ��Ĵ�С
	theta:��ת�Ƕȣ���λ���ȣ�˳ʱ��Ϊ��
	scale:���ţ�scale>1��ʾ�Ŵ�  <1��ʾ��С
*/ 
void Rotate_Nearest(const Mat &srcImage, Point center, double theta, double scale, Size dstSize, Mat &dstImage)
{
	CV_Assert(srcImage.depth() == CV_8U);
	dstImage.create(dstSize, srcImage.type());

	// ԭͼ�����ת����
	int x0 = center.x;
	int y0 = center.y;
	theta = DEGREE2RADIAN(theta);

	// dx,dy����dstImage���ĵ���srcImage����ת���ĵľ��룬Ҳ����ƽ�Ƶľ���
	int dx = dstImage.cols / 2 - x0;
	int dy = dstImage.rows / 2 - y0;
	int numberOfChannels = srcImage.channels();

	int widthOfDst = dstImage.cols;
	int heightOfDst = dstImage.rows;
	
	for (int y = 0; y <= heightOfDst - 1; ++y)
	{
		for (int x = 0; x <= widthOfDst - 1; ++x)
		{
			float srcX = ((x - x0 - dx)*cos(theta) + (y - y0 - dy)*sin(theta))/scale + x0;
			float srcY = ((x - x0 - dx)*(-sin(theta)) + (y - y0 - dy)*cos(theta))/scale + y0;

			// get the nearest coordinate of src
			int x1 = (int)srcX;
			int y1 = (int)srcY;
			if (numberOfChannels == 1)
			{
				if ((x1 >= 0 && x1 <= srcImage.cols - 1) && (y1 >= 0 && y1 <= srcImage.rows - 1))
				{
					dstImage.at<uchar>(y, x) = srcImage.at<uchar>(y1, x1);
				}
				else
				{
					// Խ�縳ֵ0
					dstImage.at<uchar>(y, x) = 0;
				}
			}
			else
			{
				if ((x1 >= 0 && x1 <= srcImage.cols - 1) && (y1 >= 0 && y1 <= srcImage.rows - 1))
				{
					dstImage.at<cv::Vec3b>(y, x) = srcImage.at<cv::Vec3b>(y1, x1);
				}
				else
				{
					dstImage.at<cv::Vec3b>(y, x) = cv::Vec3b(0,0,0);
				}

			}
		}
	}

}

void Rotate_Bilinear(const Mat &srcImage, Point center, double theta, double scale, Size dstSize, Mat &dstImage)
{
	CV_Assert(srcImage.depth() == CV_8U);
	dstImage.create(dstSize, srcImage.type());
	dstImage.setTo(Scalar(0, 0, 0));

	// ԭͼ�����ת����
	int x0 = center.x;
	int y0 = center.y;
	theta = DEGREE2RADIAN(theta);

	// dx,dy����dstImage���ĵ���srcImage����ת���ĵľ��룬Ҳ����ƽ�Ƶľ���
	int dx = dstImage.cols / 2 - x0;
	int dy = dstImage.rows / 2 - y0;
	int numberOfChannels = srcImage.channels();

	int widthOfDst = dstImage.cols;
	int heightOfDst = dstImage.rows;

	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0, 0, 0)); // ʹ��0���߽�

	for (int y = 0; y <= heightOfDst - 1; ++y)
	{
		for (int x = 0; x <= widthOfDst - 1; ++x)
		{
			// ����ԭ���ķ�ʽ����ԭͼ����
			float srcX = ((x - x0 - dx)*cos(theta) + (y - y0 - dy)*sin(theta)) / scale + x0;
			float srcY = ((x0 + dx - x)*sin(theta) + (y - y0 - dy)*cos(theta)) / scale + y0;
			
			// ��1,�õ���extendedImage�е�����
			srcX++; 
			srcY++;
			 
			// get the nearest coordinate of src
			int x1 = (int)(srcX); 
			int y1 = (int)(srcY);

			// ����ת��Ϊ����
			int dx1 = (srcX - x1)*(1<< SHIFT);
			int dy1 = (srcY - y1)*(1<< SHIFT);

			if (numberOfChannels == 1)
			{
				// !����ע������ķ�Χ����extendedImage�У�ԭͼ�ķ�Χ����1~cols - 2��
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{	
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					uchar a = extendedImage.at<uchar>(y1, x1);
					uchar b = extendedImage.at<uchar>(y1, x1 + 1);
					uchar c = extendedImage.at<uchar>(y1 + 1, x1);
					uchar d = extendedImage.at<uchar>(y1 + 1, x1 + 1);

					// ��Ҫ������
					//int p = (a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1)/(1<<(2* SHIFT));
					
					int p = a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1;
					p = DESCALE(p, 2*SHIFT);

					dstImage.at<uchar>(y, x) = p;
				}
				else
				{
					// Խ�縳ֵ0
					dstImage.at<uchar>(y, x) = 0;
				}
			}
			else
			{
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					Vec3b a = extendedImage.at<Vec3b>(y1, x1);
					Vec3b b = extendedImage.at<Vec3b>(y1, x1 + 1);
					Vec3b c = extendedImage.at<Vec3b>(y1 + 1, x1);
					Vec3b d = extendedImage.at<Vec3b>(y1 + 1, x1 + 1);

					// ��Ҫ������
					/*int p1 = (a[0] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0] * dx1*((1 << SHIFT) - dy1) + c[0] * ((1 << SHIFT) - dx1)*dy1 + d[0] * dx1*dy1)/(1<<(2*SHIFT));
					int p2 = (a[1] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1] * dx1*((1 << SHIFT) - dy1) + c[1] * ((1 << SHIFT) - dx1)*dy1 + d[1] * dx1*dy1)/ (1 << (2 * SHIFT));
					int p3 = (a[2] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2] * dx1*((1 << SHIFT) - dy1) + c[2] * ((1 << SHIFT) - dx1)*dy1 + d[2] * dx1*dy1)/ (1 << (2 * SHIFT));*/

					int p1 = a[0]*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0]*dx1*((1 << SHIFT) - dy1) + c[0]*((1 << SHIFT) - dx1)*dy1 + d[0]*dx1*dy1;
					p1 = DESCALE(p1, 2 * SHIFT);
					int p2 = a[1]*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1]*dx1*((1 << SHIFT) - dy1) + c[1]*((1 << SHIFT) - dx1)*dy1 + d[1] *dx1*dy1;
					p2 = DESCALE(p2, 2 * SHIFT);
					int p3 = a[2]*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2]*dx1*((1 << SHIFT) - dy1) + c[2]*((1 << SHIFT) - dx1)*dy1 + d[2] *dx1*dy1;
					p3 = DESCALE(p3, 2 * SHIFT);


					dstImage.at<cv::Vec3b>(y, x) = Vec3b(p1,p2,p3);
				}
				else
				{
					dstImage.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
				}

			}
		}
	}
}

// 2016-8-14 16:16:20,by QQ
// �Ż���1. ��Rotate_Bilinearѭ���ڵĲ�������ȡ����
void Rotate_Bilinear1(const Mat &srcImage, Point center, double theta, double scale, Size dstSize, Mat &dstImage)
{
	CV_Assert(srcImage.depth() == CV_8U);
	dstImage.create(dstSize, srcImage.type());
	dstImage.setTo(Scalar(0, 0, 0));

	// ԭͼ�����ת����
	int x0 = center.x;
	int y0 = center.y;
	theta = DEGREE2RADIAN(theta);

	// dx,dy����dstImage���ĵ���srcImage����ת���ĵľ��룬Ҳ����ƽ�Ƶľ���
	int dx = dstImage.cols / 2 - x0;
	int dy = dstImage.rows / 2 - y0;
	int numberOfChannels = srcImage.channels();

	int widthOfDst = dstImage.cols;
	int heightOfDst = dstImage.rows;

	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0, 0, 0)); // ʹ��0���߽�

	////////////////////////////////// �Ż�����/////////////////////////////
	// ��ѭ���ڵĲ�������ȡ����
	double sinTheta = sin(theta);
	double cosTheta = cos(theta);
	scale = 1.0 / scale;
	
	for (int y = 0; y <= heightOfDst - 1; ++y)
	{
		// ���ڲ�ѭ����������ȡ����
		double temp1 = (y - y0 - dy)*sinTheta;
		double temp2 = (y - y0 - dy)*cosTheta;

		for (int x = 0; x <= widthOfDst - 1; ++x)
		{
			// ����ԭ���ķ�ʽ����ԭͼ����
			double srcX =  ((x - x0 - dx)*cosTheta + temp1)*scale + x0;
			double srcY = (-(x-x0 - dx)*sinTheta + temp2)*scale + y0;

			// ��1,�õ���extendedImage�е�����
			srcX++;
			srcY++;

			// get the nearest coordinate of src
			int x1 = (int)(srcX);
			int y1 = (int)(srcY);

			// ����ת��Ϊ������
			int dx1 = (srcX - x1)*(1 << SHIFT);
			int dy1 = (srcY - y1)*(1 << SHIFT);

			if (numberOfChannels == 1)
			{
				// !����ע������ķ�Χ����extendedImage�У�ԭͼ�ķ�Χ����1~cols - 2��
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					uchar a = extendedImage.at<uchar>(y1, x1);
					uchar b = extendedImage.at<uchar>(y1, x1 + 1);
					uchar c = extendedImage.at<uchar>(y1 + 1, x1);
					uchar d = extendedImage.at<uchar>(y1 + 1, x1 + 1);

					// ��Ҫ������
					//int p = (a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1)/(1<<(2* SHIFT));

					int p = a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1;
					p = DESCALE(p, 2 * SHIFT);

					dstImage.at<uchar>(y, x) = p;
				}
				else
				{
					// Խ�縳ֵ0
					dstImage.at<uchar>(y, x) = 0;
				}
			}
			else
			{
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					Vec3b a = extendedImage.at<Vec3b>(y1, x1);
					Vec3b b = extendedImage.at<Vec3b>(y1, x1 + 1);
					Vec3b c = extendedImage.at<Vec3b>(y1 + 1, x1);
					Vec3b d = extendedImage.at<Vec3b>(y1 + 1, x1 + 1);

					// ��Ҫ������
					/*int p1 = (a[0] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0] * dx1*((1 << SHIFT) - dy1) + c[0] * ((1 << SHIFT) - dx1)*dy1 + d[0] * dx1*dy1)/(1<<(2*SHIFT));
					int p2 = (a[1] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1] * dx1*((1 << SHIFT) - dy1) + c[1] * ((1 << SHIFT) - dx1)*dy1 + d[1] * dx1*dy1)/ (1 << (2 * SHIFT));
					int p3 = (a[2] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2] * dx1*((1 << SHIFT) - dy1) + c[2] * ((1 << SHIFT) - dx1)*dy1 + d[2] * dx1*dy1)/ (1 << (2 * SHIFT));*/

					int p1 = a[0] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0] * dx1*((1 << SHIFT) - dy1) + c[0] * ((1 << SHIFT) - dx1)*dy1 + d[0] * dx1*dy1;
					p1 = DESCALE(p1, 2 * SHIFT);
					int p2 = a[1] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1] * dx1*((1 << SHIFT) - dy1) + c[1] * ((1 << SHIFT) - dx1)*dy1 + d[1] * dx1*dy1;
					p2 = DESCALE(p2, 2 * SHIFT);
					int p3 = a[2] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2] * dx1*((1 << SHIFT) - dy1) + c[2] * ((1 << SHIFT) - dx1)*dy1 + d[2] * dx1*dy1;
					p3 = DESCALE(p3, 2 * SHIFT);


					dstImage.at<cv::Vec3b>(y, x) = Vec3b(p1, p2, p3);
				}
				else
				{
					dstImage.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
				}

			}
		}
	}
}
// 2016-8-14 16:41:51,by QQ
//  �Ż�����Rotate_Bilinear1�Ļ����ϣ���һ����ȡ���������ı�ѭ������������ʽ
void Rotate_Bilinear2(const Mat &srcImage, Point center, double theta, double scale, Size dstSize, Mat &dstImage)
{
	CV_Assert(srcImage.depth() == CV_8U);
	dstImage.create(dstSize, srcImage.type());
	dstImage.setTo(Scalar(0, 0, 0));

	// ԭͼ�����ת����
	int x0 = center.x;
	int y0 = center.y;
	theta = DEGREE2RADIAN(theta);

	// dx,dy����dstImage���ĵ���srcImage����ת���ĵľ��룬Ҳ����ƽ�Ƶľ���
	int dx = dstImage.cols / 2 - x0;
	int dy = dstImage.rows / 2 - y0;
	int numberOfChannels = srcImage.channels();

	int widthOfDst = dstImage.cols;
	int heightOfDst = dstImage.rows;

	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0, 0, 0)); // ʹ��0���߽�

	////////////////////////////////// �Ż�����/////////////////////////////
	// ��ѭ���ڵĲ�������ȡ����
	double sinTheta = sin(theta);
	double cosTheta = cos(theta);
	scale = 1.0 / scale;

	// �ı���ѭ���ڲ������ķ�ʽ
	double temp1= (0 - y0 - dy)*sinTheta;
	double temp2 = (0 - y0 - dy)*cosTheta;
	double dtemp1 = sinTheta;
	double dtemp2 = cosTheta;

	for (int y = 0; y <= heightOfDst - 1; ++y,temp1+=dtemp1,temp2+=dtemp2)
	{
		// �ı���ѭ���ڲ������ķ�ʽ
		double temp3= ((0 - x0 - dx)*cosTheta + temp1)*scale + x0;
		double temp4= (-(0 - x0 - dx)*sinTheta + temp2)*scale + y0;
		double dtemp3 = (cosTheta)*scale;
		double dtemp4= (-sinTheta)*scale;
		for (int x = 0; x <= widthOfDst - 1; ++x,temp3+=dtemp3,temp4+=dtemp4)
		{
			// ����ԭͼ����
			double srcX = temp3;
			double srcY = temp4;

			// ��1,�õ���extendedImage�е�����
			srcX++;
			srcY++;

			// get the nearest coordinate of src
			int x1 = (int)(srcX);
			int y1 = (int)(srcY);

			// ����ת��Ϊ����
			int dx1 = (srcX - x1)*(1 << SHIFT);
			int dy1 = (srcY - y1)*(1 << SHIFT);

			if (numberOfChannels == 1)
			{
				// !����ע������ķ�Χ����extendedImage�У�ԭͼ�ķ�Χ����1~cols - 2��
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					uchar a = extendedImage.at<uchar>(y1, x1);
					uchar b = extendedImage.at<uchar>(y1, x1 + 1);
					uchar c = extendedImage.at<uchar>(y1 + 1, x1);
					uchar d = extendedImage.at<uchar>(y1 + 1, x1 + 1);

					// ��Ҫ������
					//int p = (a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1)/(1<<(2* SHIFT));

					int p = a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1;
					p = DESCALE(p, 2 * SHIFT);

					dstImage.at<uchar>(y, x) = p;
				}
				else
				{
					// Խ�縳ֵ0
					dstImage.at<uchar>(y, x) = 0;
				}
			}
			else
			{
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					Vec3b a = extendedImage.at<Vec3b>(y1, x1);
					Vec3b b = extendedImage.at<Vec3b>(y1, x1 + 1);
					Vec3b c = extendedImage.at<Vec3b>(y1 + 1, x1);
					Vec3b d = extendedImage.at<Vec3b>(y1 + 1, x1 + 1);

					// ��Ҫ������
					/*int p1 = (a[0] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0] * dx1*((1 << SHIFT) - dy1) + c[0] * ((1 << SHIFT) - dx1)*dy1 + d[0] * dx1*dy1)/(1<<(2*SHIFT));
					int p2 = (a[1] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1] * dx1*((1 << SHIFT) - dy1) + c[1] * ((1 << SHIFT) - dx1)*dy1 + d[1] * dx1*dy1)/ (1 << (2 * SHIFT));
					int p3 = (a[2] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2] * dx1*((1 << SHIFT) - dy1) + c[2] * ((1 << SHIFT) - dx1)*dy1 + d[2] * dx1*dy1)/ (1 << (2 * SHIFT));*/

					int p1 = a[0] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0] * dx1*((1 << SHIFT) - dy1) + c[0] * ((1 << SHIFT) - dx1)*dy1 + d[0] * dx1*dy1;
					p1 = DESCALE(p1, 2 * SHIFT);
					int p2 = a[1] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1] * dx1*((1 << SHIFT) - dy1) + c[1] * ((1 << SHIFT) - dx1)*dy1 + d[1] * dx1*dy1;
					p2 = DESCALE(p2, 2 * SHIFT);
					int p3 = a[2] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2] * dx1*((1 << SHIFT) - dy1) + c[2] * ((1 << SHIFT) - dx1)*dy1 + d[2] * dx1*dy1;
					p3 = DESCALE(p3, 2 * SHIFT);


					dstImage.at<cv::Vec3b>(y, x) = Vec3b(p1, p2, p3);
				}
				else
				{
					dstImage.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
				}

			}
		}
	}
} // Rotate_Bilinear2

  // 2016-9-11 15:57:13,by QQ
  //  �Ż�����Rotate_Bilinear2�Ļ����ϣ���һ����ȡ������,����ѭ������ѭ�������������Ѿ�û�г˷�
void Rotate_Bilinear3(const Mat &srcImage, Point center, double theta, double scale, Size dstSize, Mat &dstImage)
{
	CV_Assert(srcImage.depth() == CV_8U);
	dstImage.create(dstSize, srcImage.type());
	dstImage.setTo(Scalar(0, 0, 0));

	// ԭͼ�����ת����
	int x0 = center.x;
	int y0 = center.y;
	theta = DEGREE2RADIAN(theta);

	// dx,dy����dstImage���ĵ���srcImage����ת���ĵľ��룬Ҳ����ƽ�Ƶľ���
	int dx = dstImage.cols / 2 - x0;
	int dy = dstImage.rows / 2 - y0;
	int numberOfChannels = srcImage.channels();

	int widthOfDst = dstImage.cols;
	int heightOfDst = dstImage.rows;

	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0, 0, 0)); // ʹ��0���߽�

	////////////////////////////////// �Ż�����/////////////////////////////
	// ��ѭ���ڵĲ�������ȡ����
	double sinTheta = sin(theta);
	double cosTheta = cos(theta);
	scale = 1.0 / scale;

	// �ı���ѭ���ڲ������ķ�ʽ
	double temp1 = (0 - y0 - dy)*sinTheta;
	double temp2 = (0 - y0 - dy)*cosTheta;
	double dtemp1 = sinTheta;
	double dtemp2 = cosTheta;

	double dtemp3 = (cosTheta)*scale;
	double dtemp4 = (-sinTheta)*scale;

	// �ı���ѭ���ڲ������ķ�ʽ
	double temp3 = ((0 - x0 - dx)*cosTheta + temp1)*scale + x0- (dtemp1*scale);
	double temp4 = (-(0 - x0 - dx)*sinTheta + temp2)*scale + y0- (dtemp2*scale);

	double dtemp5 = (dtemp1*scale);
	double dtemp6 = (dtemp2*scale);
	for (int y = 0; y <= heightOfDst - 1; ++y)
	{
		// �ı���ѭ���ڲ������ķ�ʽ
		//double temp3 = ((0 - x0 - dx)*cosTheta + temp1)*scale + x0;
		//double temp4 = (-(0 - x0 - dx)*sinTheta + temp2)*scale + y0;
		temp3 += dtemp5;
		temp4 += dtemp6;
		
		// ע������һ��Ҫ������ʱ���������ֱ��ʹ��temp3,temp4�ʹ����ˣ���Ϊtemp3��temp4����ѭ����ֵ���޸���
		double temp5 = temp3;
		double temp6 = temp4;
		for (int x = 0; x <= widthOfDst - 1; ++x, temp5 += dtemp3, temp6 += dtemp4)
		{
			// ����ԭͼ����
			double srcX = temp5;
			double srcY = temp6;

			// ��1,�õ���extendedImage�е�����
			srcX++;
			srcY++;

			// get the nearest coordinate of src
			int x1 = (int)(srcX);
			int y1 = (int)(srcY);

			// ����ת��Ϊ����
			int dx1 = (srcX - x1)*(1 << SHIFT);
			int dy1 = (srcY - y1)*(1 << SHIFT);

			if (numberOfChannels == 1)
			{
				// !����ע������ķ�Χ����extendedImage�У�ԭͼ�ķ�Χ����1~cols - 2��
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					uchar a = extendedImage.at<uchar>(y1, x1);
					uchar b = extendedImage.at<uchar>(y1, x1 + 1);
					uchar c = extendedImage.at<uchar>(y1 + 1, x1);
					uchar d = extendedImage.at<uchar>(y1 + 1, x1 + 1);

					// ��Ҫ������
					//int p = (a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1)/(1<<(2* SHIFT));

					int p = a*((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b*dx1*((1 << SHIFT) - dy1) + c*((1 << SHIFT) - dx1)*dy1 + d*dx1*dy1;
					p = DESCALE(p, 2 * SHIFT);

					dstImage.at<uchar>(y, x) = p;
				}
				else
				{
					// Խ�縳ֵ0
					dstImage.at<uchar>(y, x) = 0;
				}
			}
			else
			{
				if ((x1 >= 1 && x1 <= extendedImage.cols - 2) && (y1 >= 1 && y1 <= extendedImage.rows - 2))
				{
					//˫���Բ�ֵ
					//��Χ4����
					//a�������������
					//a   b
					//  p
					//c   d
					Vec3b a = extendedImage.at<Vec3b>(y1, x1);
					Vec3b b = extendedImage.at<Vec3b>(y1, x1 + 1);
					Vec3b c = extendedImage.at<Vec3b>(y1 + 1, x1);
					Vec3b d = extendedImage.at<Vec3b>(y1 + 1, x1 + 1);

					// ��Ҫ������
					/*int p1 = (a[0] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0] * dx1*((1 << SHIFT) - dy1) + c[0] * ((1 << SHIFT) - dx1)*dy1 + d[0] * dx1*dy1)/(1<<(2*SHIFT));
					int p2 = (a[1] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1] * dx1*((1 << SHIFT) - dy1) + c[1] * ((1 << SHIFT) - dx1)*dy1 + d[1] * dx1*dy1)/ (1 << (2 * SHIFT));
					int p3 = (a[2] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2] * dx1*((1 << SHIFT) - dy1) + c[2] * ((1 << SHIFT) - dx1)*dy1 + d[2] * dx1*dy1)/ (1 << (2 * SHIFT));*/

					int p1 = a[0] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[0] * dx1*((1 << SHIFT) - dy1) + c[0] * ((1 << SHIFT) - dx1)*dy1 + d[0] * dx1*dy1;
					p1 = DESCALE(p1, 2 * SHIFT);
					int p2 = a[1] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[1] * dx1*((1 << SHIFT) - dy1) + c[1] * ((1 << SHIFT) - dx1)*dy1 + d[1] * dx1*dy1;
					p2 = DESCALE(p2, 2 * SHIFT);
					int p3 = a[2] * ((1 << SHIFT) - dx1)*((1 << SHIFT) - dy1) + b[2] * dx1*((1 << SHIFT) - dy1) + c[2] * ((1 << SHIFT) - dx1)*dy1 + d[2] * dx1*dy1;
					p3 = DESCALE(p3, 2 * SHIFT);


					dstImage.at<cv::Vec3b>(y, x) = Vec3b(p1, p2, p3);
				}
				else
				{
					dstImage.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
				}

			}
		}
	}
}  // Rotate_Bilinear3




}//namespace QQ
