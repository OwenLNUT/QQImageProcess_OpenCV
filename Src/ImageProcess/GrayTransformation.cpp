#define DLLAPI_EXPORTS

#include"GrayTransformation.h"
#include"Histogram.h"

namespace QQ
{

	void RGB2Gray(const Mat &srcImage, Mat &dstImage)
	{
		CV_Assert(srcImage.type()==CV_8UC3);
		dstImage.create(srcImage.size(), CV_8UC1);

		// �������ұ�
		int LUT_R[256],LUT_G[256],LUT_B[256];
		for (int i = 0; i <= 255; ++i)
		{
			LUT_R[i] = 0.299*(i << 16);
			LUT_G[i] = 0.587*(i << 16);
			LUT_B[i] = 0.114*(i << 16);
		}

		// �ҶȻ�
		int numberOfPixel = srcImage.rows*srcImage.cols;
		uchar *dataOfSrc = srcImage.data;
		uchar *dataOfDst = dstImage.data;
		for (int i = 0; i <= numberOfPixel - 1; ++i)
		{
			int b = dataOfSrc[0];
			int g = dataOfSrc[1]; 
			int r = dataOfSrc[2];
			dataOfDst[0] = (LUT_B[b] + LUT_G[g] + LUT_R[r]) >> 16;

			dataOfSrc += 3;
			dataOfDst += 1;
		}

	}

	void GammaCorrection(const Mat &srcImage, Mat &dstImage, float gamma)
	{
		CV_Assert(srcImage.type() == CV_8UC1);
		dstImage.create(srcImage.size(), CV_8UC1);

		// ����ת��
		Mat X;
		srcImage.convertTo(X, CV_32FC1);

		// GammaУ��
		Mat I;
		//float gamma = 1 / 2.2;
		pow(X, gamma, I); // ����gammaУ��

		// ��һ����0~255
		cv::normalize(I, dstImage, 0, 255, NORM_MINMAX, CV_8UC1);

	}

	// 2016-7-9 16:05:47,by QQ
	void EqualizeHistogram(const Mat &srcImage, Mat &dstImage)
	{
		CV_Assert(srcImage.type() == CV_8UC1);
		dstImage.create(srcImage.size(), srcImage.type());

		// ����ֱ��ͼ
		int histogram[256];
		GetHistogram(srcImage, histogram);

		// �����ۼƷֲ�ֱ��ͼ(Ҳ���Ǳ任����f(x))
		int numberOfPixel = srcImage.rows*srcImage.cols;
		int LUT[256];
		LUT[0] = 1.0*histogram[0] / numberOfPixel * 255;
		int sum = histogram[0];
		for (int i = 1; i <= 255; ++i)
		{
			sum += histogram[i];

			LUT[i] = 1.0*sum / numberOfPixel * 255;
		}

		// �Ҷȱ任
		uchar *dataOfSrc = srcImage.data;
		uchar *dataOfDst = dstImage.data;
		for (int i = 0; i <= numberOfPixel - 1; ++i)
			dataOfDst[i] = LUT[dataOfSrc[i]];
	}


}//namespace QQ
