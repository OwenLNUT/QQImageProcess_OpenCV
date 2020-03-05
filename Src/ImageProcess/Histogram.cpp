#define DLLAPI_EXPORTS
#include "Histogram.h"

namespace QQ
{
//��֧��ROI
void GetHistogram(const Mat &image, int *histogram)
{
	memset(histogram, 0, 256 * sizeof(int));

	//����ֱ��ͼ
	int pixelCount = image.cols*image.rows;
	uchar *imageData = image.data;
	for (int i = 0; i <= pixelCount - 1; ++i)
	{
		int gray = imageData[i];
		histogram[gray]++;
	}
}
void GetMedianValue(const Mat &image, int &medianValue)
{
	//��ȡֱ��ͼ
	int histogram[256];
	GetHistogram(image, histogram);

	int sum=0;
	int pixelCount = image.cols*image.rows;
	for (int i=0;i<=255;++i)
	{
		//
		sum += histogram[i];
		if (2 * sum>pixelCount)//���ó���(sum>pixelCount/2)
		{
			medianValue = i;
			break;
		}
	}

}
void GetMedianValue_AndPixelCountLowerMedian(const Mat &image, int &medianValue, int &pixelCountLowerMedian)
{
	//��ȡֱ��ͼ
	int histogram[256];
	GetHistogram(image, histogram);

	pixelCountLowerMedian = 0;
	int pixelCount = image.cols*image.rows;
	for (int i = 0; i <= 255; ++i)
	{
		//
		pixelCountLowerMedian += histogram[i];
		if (2 * pixelCountLowerMedian>pixelCount)//���ó���(sum>pixelCount/2)
		{
			medianValue = i;
			break;
		}
	}

}

//����ͼ��������С����
void GetMaxMinValue(const Mat &image, int &maxValue, int &minValue)//������������Ⱥ���С����
{
	//��ȡֱ��ͼ
	int histogram[256];
	GetHistogram(image, histogram);

	//�������С
	for (int i=0;i<=255;++i)
	{
		if (histogram[i] != 0)
		{
			minValue=i;
			break;
		}
	}
	for (int i=255;i>=0;--i)
	{
		if (histogram[i] != 0)
		{
			maxValue=i;
			break;
		}
	}
}



//ע�⣺image_Histogram�Ŀ��Ҫ>=256
void DrawHistogram(const Mat &histogram, Mat &image_Historam)
{
	//
	image_Historam.setTo(Scalar(255, 255, 255));

	//��ȡͼ��Ĳ���
	int width = image_Historam.cols;
	int height = image_Historam.rows;

	//ֱ��ͼ��һ��
	Mat hist_Normalize(histogram);//��������
	normalize(histogram, hist_Normalize, 0, height, NORM_MINMAX, -1);

	//��ʼ��ͼ
	float *data = (float *)hist_Normalize.data;
	int step = width / 256; //ÿ���Ҷȼ��Ŀ��
	for (int i = 0; i < 255; i++)
	{
		rectangle(image_Historam,
			Point(i*step, height - 1),
			Point((i + 1)*step, height - cvRound(hist_Normalize.at<float>(i))),
			Scalar::all(0), -1, 8, 0);
	}

}







}//namespace QQ
