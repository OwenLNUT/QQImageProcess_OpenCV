#define DLLAPI_EXPORTS

#include"GradientHistogram.h"

namespace QQ
{
	// 2017-8-3 17:16:16,by QQ
	void ComputeGradient(const Mat &srcImage, Mat &gradient, Mat &gradientAngle, const int numberOfBins, bool signedGradient)
	{

		CV_Assert(srcImage.depth() == CV_8U);

		// ����fx,fy
		Mat fx(srcImage.size(), CV_32SC1);
		Mat fy(srcImage.size(), CV_32SC1);

		gradient.create(srcImage.size(), CV_32FC1);
		gradientAngle.create(srcImage.size(), CV_32FC1);

		Mat enlargedImage;
		copyMakeBorder(srcImage, enlargedImage, 1, 1, 1, 1, cv::BORDER_REFLECT_101);

		float angleScale = signedGradient ?360 : 180;

		// ����fx,fy
		int stepOfEnlargedImage = enlargedImage.cols*enlargedImage.channels();
		int stepOfFx = fx.cols;
		int numberOfChannels = enlargedImage.channels();
		uchar *rowOfEnlargedImage = enlargedImage.data + stepOfEnlargedImage + enlargedImage.channels();
		int *rowOfFx = (int *)fx.data;
		int *rowOfFy = (int *)fy.data;
		float *rowOfGradient = (float *)gradient.data;
		float *rowOfGradientAngle = (float *)gradientAngle.data;
		int height = srcImage.rows;
		int width = srcImage.cols;

		// debug
		//std::ofstream outFile1("gradient.txt", ios::out);

		for (int y = 0; y <= height - 1; ++y, rowOfEnlargedImage += stepOfEnlargedImage, rowOfFx += stepOfFx, rowOfFy += stepOfFx,rowOfGradient+=stepOfFx,rowOfGradientAngle+=stepOfFx)
		{
			uchar *colOfEnlargedImage = rowOfEnlargedImage;
			int *colOfFx = rowOfFx;
			int *colOfFy = rowOfFy;
			float *colOfGradient = rowOfGradient;
			float *colOfGradientAngle = rowOfGradientAngle;
			for (int x = 0; x <= width - 1; ++x, colOfEnlargedImage += numberOfChannels, ++colOfFx, ++colOfFy,++colOfGradient,++colOfGradientAngle)
			{
				if (numberOfChannels == 1)
				{
					// ʹ��Sobelģ��
					// fx
					//colOfFx[0] = colOfEnlargedImage[1 - stepOfEnlargedImage] + 2 * colOfEnlargedImage[1] + colOfEnlargedImage[1 + stepOfEnlargedImage] -
					//	colOfEnlargedImage[-1 - stepOfEnlargedImage] - 2 * colOfEnlargedImage[-1] - colOfEnlargedImage[-1 + stepOfEnlargedImage];

					//// fy
					//colOfFy[0] = colOfEnlargedImage[stepOfEnlargedImage - 1] + 2 * colOfEnlargedImage[stepOfEnlargedImage] + colOfEnlargedImage[stepOfEnlargedImage + 1] -
					//	colOfEnlargedImage[-stepOfEnlargedImage - 1] - 2 * colOfEnlargedImage[-stepOfEnlargedImage] - colOfEnlargedImage[-stepOfEnlargedImage + 1];

					// ʹ����򵥵ĶԳ�ģ��[-1,0,1](HOG��ʹ��)
					colOfFx[0] = colOfEnlargedImage[1] - colOfEnlargedImage[-1];
					colOfFy[0] = colOfEnlargedImage[stepOfEnlargedImage] - colOfEnlargedImage[-stepOfEnlargedImage];

				}
				else
				{
					int maxGraident = -1;
					int maxFx = 0, maxFy = 0;
					for (int k = 0; k <= numberOfChannels - 1; ++k)
					{
						// ʹ��Sobelģ��
						// fx
						//int fx0 = colOfEnlargedImage[numberOfChannels - stepOfEnlargedImage + k] + 2 * colOfEnlargedImage[numberOfChannels + k] + colOfEnlargedImage[numberOfChannels + stepOfEnlargedImage + k] -
						//	colOfEnlargedImage[-numberOfChannels - stepOfEnlargedImage + k] - 2 * colOfEnlargedImage[-numberOfChannels + k] - colOfEnlargedImage[-numberOfChannels + stepOfEnlargedImage + k];

						//// fy
						//int fy0 = colOfEnlargedImage[stepOfEnlargedImage - numberOfChannels + k] + 2 * colOfEnlargedImage[stepOfEnlargedImage + k] + colOfEnlargedImage[stepOfEnlargedImage + numberOfChannels + k] -
						//	colOfEnlargedImage[-stepOfEnlargedImage - numberOfChannels + k] - 2 * colOfEnlargedImage[-stepOfEnlargedImage + k] - colOfEnlargedImage[-stepOfEnlargedImage + numberOfChannels + k];

						// ʹ����򵥵ĶԳ�ģ��[-1,0,1](HOG��ʹ��)
						int fx0 = colOfEnlargedImage[numberOfChannels+k] - colOfEnlargedImage[-numberOfChannels+k];
						int fy0 = colOfEnlargedImage[stepOfEnlargedImage+k] - colOfEnlargedImage[-stepOfEnlargedImage+k];

						int gradient = fx0*fx0 + fy0*fy0;

						// ȡ�ݶȴ�С�����Ǹ�ͨ��
						if (gradient > maxGraident)
						{
							maxGraident = gradient;
							maxFx = fx0;
							maxFy = fy0;
						}
					}

					colOfFx[0] = maxFx;
					colOfFy[0] = maxFy;

					

				}

				// �����ݶȴ�С���ݶȷ���
				colOfGradient[0] = sqrt(colOfFx[0] * colOfFx[0] + colOfFy[0] * colOfFy[0]);

				// �Ƿ��з���
				if (signedGradient)
				{
					// ת��Ϊ�Ƕ�
					colOfGradientAngle[0] = cv::fastAtan2(colOfFy[0], colOfFx[0]);
				}
				else
				{
					// ת��Ϊ�Ƕ�
					colOfGradientAngle[0] = cv::fastAtan2(colOfFy[0], colOfFx[0]) - 180.0*(colOfFy[0]<0);
				}

				// ת��Ϊbin�����
				colOfGradientAngle[0] = colOfGradientAngle[0] / angleScale * numberOfBins;
				if (colOfGradientAngle[0] >= numberOfBins)
				{
					colOfGradientAngle[0] -= numberOfBins;
				}
				
				// debug
				//outFile1 << colOfFx[0] << " " << colOfFy[0] << " " << colOfGradient[0] << " " << colOfGradientAngle[0] << endl;
			}
		}

		// debug
		//outFile1.close();
		
	}// ComputeGradient

	void ComputeGradient2(const Mat &srcImage, Mat &gradient, Mat &gradientAngle, const int numberOfBins, bool signedGradient)
	{

		CV_Assert(srcImage.depth() == CV_8U);

		// ����fx,fy
		Mat fx(srcImage.size(), CV_32SC1);
		Mat fy(srcImage.size(), CV_32SC1);

		gradient.create(srcImage.size(), CV_32FC2);
		gradientAngle.create(srcImage.size(), CV_32FC2);

		Mat enlargedImage;
		copyMakeBorder(srcImage, enlargedImage, 1, 1, 1, 1, cv::BORDER_REFLECT_101);

		float angleScale = signedGradient ? (1.0*numberOfBins) / 360 : (1.0*numberOfBins)/180;

		// ����fx,fy
		int stepOfEnlargedImage = enlargedImage.cols*enlargedImage.channels();
		int stepOfFx = fx.cols;
		int stepOfGradient = 2 * stepOfFx;
		int numberOfChannels = enlargedImage.channels();
		uchar *rowOfEnlargedImage = enlargedImage.data + stepOfEnlargedImage + enlargedImage.channels();
		int *rowOfFx = (int *)fx.data;
		int *rowOfFy = (int *)fy.data;
		float *rowOfGradient = (float *)gradient.data;
		float *rowOfGradientAngle = (float *)gradientAngle.data;
		int height = srcImage.rows;
		int width = srcImage.cols;

		// debug
		//std::ofstream outFile1("gradient.txt", ios::out);

		for (int y = 0; y <= height - 1; ++y, rowOfEnlargedImage += stepOfEnlargedImage, rowOfFx += stepOfFx, rowOfFy += stepOfFx, rowOfGradient += stepOfGradient, rowOfGradientAngle += stepOfGradient)
		{
			uchar *colOfEnlargedImage = rowOfEnlargedImage;
			int *colOfFx = rowOfFx;
			int *colOfFy = rowOfFy;
			float *colOfGradient = rowOfGradient;
			float *colOfGradientAngle = rowOfGradientAngle;
			for (int x = 0; x <= width - 1; ++x, colOfEnlargedImage += numberOfChannels, ++colOfFx, ++colOfFy, colOfGradient+=2, colOfGradientAngle+=2)
			{
				if (numberOfChannels == 1)
				{
					// ʹ��Sobelģ��
					// fx
					//colOfFx[0] = colOfEnlargedImage[1 - stepOfEnlargedImage] + 2 * colOfEnlargedImage[1] + colOfEnlargedImage[1 + stepOfEnlargedImage] -
					//	colOfEnlargedImage[-1 - stepOfEnlargedImage] - 2 * colOfEnlargedImage[-1] - colOfEnlargedImage[-1 + stepOfEnlargedImage];

					//// fy
					//colOfFy[0] = colOfEnlargedImage[stepOfEnlargedImage - 1] + 2 * colOfEnlargedImage[stepOfEnlargedImage] + colOfEnlargedImage[stepOfEnlargedImage + 1] -
					//	colOfEnlargedImage[-stepOfEnlargedImage - 1] - 2 * colOfEnlargedImage[-stepOfEnlargedImage] - colOfEnlargedImage[-stepOfEnlargedImage + 1];

					// ʹ����򵥵ĶԳ�ģ��[-1,0,1](HOG��ʹ��)
					colOfFx[0] = colOfEnlargedImage[1] - colOfEnlargedImage[-1];
					colOfFy[0] = colOfEnlargedImage[stepOfEnlargedImage] - colOfEnlargedImage[-stepOfEnlargedImage];

				}
				else
				{
					int maxGraident = -1;
					int maxFx = 0, maxFy = 0;
					for (int k = 0; k <= numberOfChannels - 1; ++k)
					{
						// ʹ��Sobelģ��
						// fx
						//int fx0 = colOfEnlargedImage[numberOfChannels - stepOfEnlargedImage + k] + 2 * colOfEnlargedImage[numberOfChannels + k] + colOfEnlargedImage[numberOfChannels + stepOfEnlargedImage + k] -
						//	colOfEnlargedImage[-numberOfChannels - stepOfEnlargedImage + k] - 2 * colOfEnlargedImage[-numberOfChannels + k] - colOfEnlargedImage[-numberOfChannels + stepOfEnlargedImage + k];

						//// fy
						//int fy0 = colOfEnlargedImage[stepOfEnlargedImage - numberOfChannels + k] + 2 * colOfEnlargedImage[stepOfEnlargedImage + k] + colOfEnlargedImage[stepOfEnlargedImage + numberOfChannels + k] -
						//	colOfEnlargedImage[-stepOfEnlargedImage - numberOfChannels + k] - 2 * colOfEnlargedImage[-stepOfEnlargedImage + k] - colOfEnlargedImage[-stepOfEnlargedImage + numberOfChannels + k];

						// ʹ����򵥵ĶԳ�ģ��[-1,0,1](HOG��ʹ��)
						int fx0 = colOfEnlargedImage[numberOfChannels + k] - colOfEnlargedImage[-numberOfChannels + k];
						int fy0 = colOfEnlargedImage[stepOfEnlargedImage + k] - colOfEnlargedImage[-stepOfEnlargedImage + k];

						int gradient = fx0*fx0 + fy0*fy0;

						// ȡ�ݶȴ�С�����Ǹ�ͨ��
						if (gradient > maxGraident)
						{
							maxGraident = gradient;
							maxFx = fx0;
							maxFy = fy0;
						}
					}

					colOfFx[0] = maxFx;
					colOfFy[0] = maxFy;

				}

				// �����ݶȴ�С���ݶȷ���
				float magnitude= sqrt(colOfFx[0] * colOfFx[0] + colOfFy[0] * colOfFy[0]);

				// �Ƿ��з���
				float angle;
				if (signedGradient)
				{
					// ת��Ϊ�Ƕ�
					angle = cv::fastAtan2(colOfFy[0], colOfFx[0]);
				}
				else
				{
					// ת��Ϊ�Ƕ�
					angle = cv::fastAtan2(colOfFy[0], colOfFx[0]) - 180.0*(colOfFy[0]<0);
				}

				// ת��Ϊbin�����
				angle = angle*angleScale - 0.5;
				int indexOfFirstBin = cvFloor(angle);
				float weightOfSecondBin = angle - indexOfFirstBin;
				float weightOfFirstBin = (1 - weightOfSecondBin);
				colOfGradient[0] = magnitude*weightOfFirstBin;
				colOfGradient[1] = magnitude*weightOfSecondBin;
				if (indexOfFirstBin < 0)
				{
					indexOfFirstBin += numberOfBins;
				}
				if (indexOfFirstBin >= numberOfBins)
				{
					indexOfFirstBin -= numberOfBins;
				}
				colOfGradientAngle[0] = indexOfFirstBin;
				int indexOfSecondBin = indexOfFirstBin + 1;
				if (indexOfSecondBin >= numberOfBins)
				{
					indexOfSecondBin = 0;
				}
				colOfGradientAngle[1] = indexOfSecondBin;

				// debug
				//outFile1 << colOfFx[0] << " " << colOfFy[0] << " " << colOfGradient[0] << " " << colOfGradient[1] << " " << colOfGradientAngle[0] << " " << colOfGradientAngle[1] << endl;
			}
		}

		// debug
		//outFile1.close();

	}// ComputeGradient2


	void ComputeGradient_HOG(const Mat& img, Mat& grad, Mat& qangle, int nbins, bool signedGradient, bool gammaCorrection, Size paddingTL, Size paddingBR)
	{

		CV_Assert(img.type() == CV_8U || img.type() == CV_8UC3);

		Size gradsize(img.cols + paddingTL.width + paddingBR.width,
			img.rows + paddingTL.height + paddingBR.height);

		// ע����˫ͨ������Ҫ��¼��������bin�Լ���Ӧ��Ȩֵ
		grad.create(gradsize, CV_32FC2);  // <magnitude*(1-alpha), magnitude*alpha>
		qangle.create(gradsize, CV_8UC2); // [0..nbins-1] - quantized gradient orientation

		// ���img��ROI������ԭͼ��Ĵ�С�Լ�ROI�����
		Size wholeSize;
		Point roiofs;
		img.locateROI(wholeSize, roiofs);

		int i, x, y;
		int cn = img.channels();

		Mat_<float> _lut(1, 256);
		const float* lut = &_lut(0, 0);

		if (gammaCorrection)
		for (i = 0; i < 256; i++)
			_lut(0, i) = std::sqrt((float)i);
		else
		for (i = 0; i < 256; i++)
			_lut(0, i) = (float)i;

		AutoBuffer<int> mapbuf(gradsize.width + gradsize.height + 4);
		int* xmap = (int*)mapbuf + 1;
		int* ymap = xmap + gradsize.width + 2;

		/*borderTypeΪ�߽��������������ص�ķ�ʽ
		Various border types, image boundaries are denoted with '|'

		* BORDER_REPLICATE:     aaaaaa|abcdefgh|hhhhhhh
		* BORDER_REFLECT:       fedcba|abcdefgh|hgfedcb
		* BORDER_REFLECT_101:   gfedcb|abcdefgh|gfedcba
		* BORDER_WRAP:          cdefgh|abcdefgh|abcdefg
		* BORDER_CONSTANT:      iiiiii|abcdefgh|iiiiiii  with some specified 'i'
		*/
		const int borderType = (int)BORDER_REFLECT_101;

		/*int borderInterpolate(int p, int len, int borderType)
		����
		p������ͼ���������ԭͼ�е����ꣻ
		len����Ӧԭͼ���һ��������ĳ��ȣ�
		borderType����������
		���磬���Ҫ��������ͼ��x����ÿ�����ص������Ӧ��ԭͼ�е����꣬��ôp����������ͼ���ÿ��������ԭͼ�����꣬��len����ԭͼ�Ŀ�
		������������������Ǽ�������ͼ������ص���ԭͼ�е�����
		���img����ROI,��ôxmap��ymap����ԭͼ������
		*/
		for (x = -1; x < gradsize.width + 1; x++)
		{
			xmap[x] = borderInterpolate(x - paddingTL.width + roiofs.x,
				wholeSize.width, borderType) - roiofs.x;
			
			//printf("%d\n", xmap[x]);
		}

		//printf("\n");

		for (y = -1; y < gradsize.height + 1; y++)
		{
			ymap[y] = borderInterpolate(y - paddingTL.height + roiofs.y,
				wholeSize.height, borderType) - roiofs.y;
			
			// printf("%d\n", ymap[y]);

		}

		// x- & y- derivatives for the whole row
		int width = gradsize.width;
		AutoBuffer<float> _dbuf(width * 4);
		float* dbuf = _dbuf;

		// ÿһ�е�Dx,Dy,Mag,Angle.ע�⣬���ĸ������ڴ���������
		Mat Dx(1, width, CV_32F, dbuf);
		Mat Dy(1, width, CV_32F, dbuf + width);
		Mat Mag(1, width, CV_32F, dbuf + width * 2);
		Mat Angle(1, width, CV_32F, dbuf + width * 3);

		int _nbins = nbins;

		// ���µ�3.2Դ��,�����з��Ż����޷��ţ�����Ĭ��Ϊ�޷���
		// �з���Ӧ�ü���Ƕ�ռ��360�ı��������޷���Ӧ�ü���Ƕ�ռ��180�ı���
		float angleScale = signedGradient ? (float)(nbins/(2.0*CV_PI)) : (float)(nbins/CV_PI); 
		//float angleScale = (float)(_nbins / CV_PI);

		// debug
		//std::ofstream outFile11("gradient_HOG.txt", ios::out);

		// ����ÿ�У�ÿ�����ص��ݶȴ�С�ͷ���ʹ�õ�ģ����[-1,0,1](Ҳ��HOG�����в��õ�ģ��)
		for (y = 0; y < gradsize.height; y++)
		{
			const uchar* imgPtr = img.data + img.step*ymap[y];
			const uchar* prevPtr = img.data + img.step*ymap[y - 1];
			const uchar* nextPtr = img.data + img.step*ymap[y + 1];

			float* gradPtr = (float*)grad.ptr(y);
			uchar* qanglePtr = (uchar*)qangle.ptr(y);

			if (cn == 1)
			{
				for (x = 0; x < width; x++)
				{
					int x1 = xmap[x];

					dbuf[x] = (float)(lut[imgPtr[xmap[x + 1]]] - lut[imgPtr[xmap[x - 1]]]);
					dbuf[width + x] = (float)(lut[nextPtr[x1]] - lut[prevPtr[x1]]);

				}
			}
			else
			{
				for (x = 0; x < width; x++)
				{
					int x1 = xmap[x] * 3;
					float dx0, dy0, dx, dy, mag0, mag;

					const uchar* p2 = imgPtr + xmap[x + 1] * 3;
					const uchar* p0 = imgPtr + xmap[x - 1] * 3;

					dx0 = lut[p2[2]] - lut[p0[2]];
					dy0 = lut[nextPtr[x1 + 2]] - lut[prevPtr[x1 + 2]];
					mag0 = dx0*dx0 + dy0*dy0;

					dx = lut[p2[1]] - lut[p0[1]];
					dy = lut[nextPtr[x1 + 1]] - lut[prevPtr[x1 + 1]];
					mag = dx*dx + dy*dy;

					if (mag0 < mag)
					{
						dx0 = dx;
						dy0 = dy;
						mag0 = mag;
					}

					dx = lut[p2[0]] - lut[p0[0]];
					dy = lut[nextPtr[x1]] - lut[prevPtr[x1]];
					mag = dx*dx + dy*dy;

					if (mag0 < mag)
					{
						dx0 = dx;
						dy0 = dy;
						mag0 = mag;
					}

					dbuf[x] = dx0;
					dbuf[x + width] = dy0;
				}
			}

			cartToPolar(Dx, Dy, Mag, Angle, false);

			for (x = 0; x < width; x++)
			{

				/* ���ȼ����ݶȴ�С���ݶȷ����ϵĲ�ֵ���ݶȷ�������������bin���ĵľ��룩
				qangle��¼�õ��Ӧ��������bin,grad��¼�˸õ��Ӧ������bin��Ӧ��Ȩֵ(ÿ��binӦ�üӶ���Ȩֵ)
				��μ���������ݶȷ������ڵ�����bin�Ͷ�Ӧ��Ȩֵ��?
				������������bin���е�Ƚϣ�����������������һ�����ɣ�
				���Ƚ�ԭʼ�Ƕȼ�ȥ0.5���Ϳ��Եõ���һ��bin����ʱ�ýǶ����һ��bin�ľ�����ǵڶ���bin��Ȩֵ��Ҳ�Ϳ��Լ�����Ե�һ��bin��Ȩֵ(1-x)
				ͬʱ����Ҳ�͵õ��˶�Ӧ��bin������������Ҫע���������������
				���ڽǶȷ�ΧΪ[0,0.5]֮���ֵ����һ��bin�Ǹ�-1��˵����һ��bin�����һ��bin
				���ڽǶȷ�ΧΪ[8.5,9]֮���ֵ���ڶ���binԽ���ˣ�˵���ڶ���bin���ǵ�һ��bin

				Ϊʲô�ݶȷ���Ҫ��ȥ0.5�أ�
				��Ϊ�ݶȷ����ǰ��ոõ��ݶȷ���������bin�е������в�ֵ�����Ƕȼ�ȥ0.5
				Ϊ�˽��ýǶ��ƶ�����һ��bin�У��Ҵ�ʱ�ýǶ����һ��bin�ľ�����ǶԵڶ���bin��Ȩֵ

				����ʾ����
				����ԭʼ�Ƕ�1.3,��ȥ0.5��angle=0.8,hidx=0
				angle=0.8
				gradPtr[x*2]->(1-angle)=0.2
				gradPtr[x*2+1]->angle=0.8
				qanglePtr[x*2]->0
				qanglePtr[x*2+1]->1
				����ԭʼ�Ƕ�Ϊ1.8����ȥ0.5��angle=1.3,hidx=1
				angle=0.3
				gradPtr[x*2]->(1-angle)=0.7
				gradPtr[x*2+1]->angle=0.3
				qanglePtr[x*2]->1
				qanglePtr[x*2+1]->2
				!����ԭʼ�Ƕ�Ϊ0.3����ȥ0.5��angle=-0.2,hidx=-1
				angle=0.8
				gradPtr[x*2]->(1-angle)=0.2
				gradPtr[x*2+1]->angle=0.8
				qanglePtr[x*2]->8
				qanglePtr[x*2+1]->0
				!����ԭʼ�Ƕ�Ϊ8.8����ȥ0.5��angle=8.3,hidx=8
				angle=0.3
				gradPtr[x*2]->(1-angle)=0.7
				gradPtr[x*2+1]->angle=0.3
				qanglePtr[x*2]->8
				qanglePtr[x*2+1]->0
				����0.5,1.5�������ٽ�ֵ����ֻ��һ��bin�й��ף���һ��bin���׶�Ϊ0

				����Ҫע�⣺
				������޷��ŽǶȣ�����angle = dbuf[x + width * 3] * angleScale - 0.5f,����Ƕȴ���180���ᳬ��nbin,���Ժ�����Ҫ
				if (hidx >= _nbins)
					hidx -= _nbins
				�����Ҫangle�������������ʵ��ֵ��Ӧ��angle = (dbuf[x + width * 3] - CV_PI*(dbuf[x + width]<0))* angleScale - 0.5f
				*/
				float mag = dbuf[x + width * 2], angle = dbuf[x + width * 3] * angleScale - 0.5f;
				int hidx = cvFloor(angle); // cvFloor()���ز����ڲ������������
				angle -= hidx;
				gradPtr[x * 2] = mag*(1.f - angle);
				gradPtr[x * 2 + 1] = mag*angle;

				// ���ڽǶȷ�ΧΪ[0,0.5]֮���ֵ�������ض�Ӧ���ݶȷ���ĵ�һ��bin�������һ��bin
				// ���ڽǶȷ�ΧΪ[8.5,9]֮���ֵ�������ض�Ӧ���ݶȷ���ĵڶ���bin���ǵ�һ��bin
				// ���hindexΪ-1��hidx����Ϊ8
				if (hidx < 0)
					hidx += _nbins; // 8
				else if (hidx >= _nbins)
					hidx -= _nbins;
				assert((unsigned)hidx < (unsigned)_nbins);

				qanglePtr[x * 2] = (uchar)hidx; // 8
				hidx++; // 9

				//
				//-1�ڲ����еı�ʾΪ11111111,��-1����Ļ������Լ�������
				// 0�ڲ����еı�ʾΪ00000000,��0����Ľ������0��
				hidx &= hidx < _nbins ? -1 : 0; // 0
				qanglePtr[x * 2 + 1] = (uchar)hidx; // 0

				//// debug
				// dbuf[x + width * 3]��ʵ�ʵĽǶ�(����)
				// �����޷��ŽǶ�(dbuf[x + width * 3] - CV_PI*(dbuf[x + width]<0))* angleScale ����ʵ�ʵ�bin�����
				// �����з��ŽǶ� (dbuf[x + width * 3])* angleScale
				//outFile11 << (float)dbuf[x] << " " << (float)dbuf[x + width] << " " << (float)gradPtr[x * 2] << " " << (float)gradPtr[x * 2 + 1] << " " << (int)qanglePtr[x * 2] << " " << (int)qanglePtr[x * 2+1] << endl;
				
			}
		}

		// debug
		//outFile11.close();

	}// ComputeGradient2


}// namespace QQ