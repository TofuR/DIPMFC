#include "pch.h"
#include "COpenCVProcess.h"
#include "Dib.h"


COpenCVProcess::COpenCVProcess(CDib* pDib)
{
	Dib2Mat(*pDib);
	cvimgbuffer = cvimg.clone();
}

COpenCVProcess::~COpenCVProcess(void)
{
	cvimg.release();
	cvimgbuffer.release();
}

void COpenCVProcess::OpenCVGaussianBlur()
{
	//Mat tmp;
	GaussianBlur(cvimg, cvimg, Size(7, 7), 0, 0, BORDER_DEFAULT);
	//cvimg = tmp;
}

void COpenCVProcess::OpenCVBinarization(int nThreshold)
{
	cvtColor(cvimg, cvimg, CV_BGR2GRAY);
	threshold(cvimg, cvimg, nThreshold, 255, CV_THRESH_BINARY);
}

void COpenCVProcess::OpenCVInvert()
{
	cvimg = 255 - cvimg;
}

void COpenCVProcess::OpenCVErode()
{
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(cvimg, cvimg, element);
}

void COpenCVProcess::OpenCVDilate()
{
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(cvimg, cvimg, element);
}

void COpenCVProcess::OpenCVEdge()
{
	// ��ԭͼ���ȥ��ʴͼ��õ���Ե
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat imgErode;
	erode(cvimg, imgErode, element);
	cvimg = cvimg - imgErode;

}

void COpenCVProcess::OpenCVFindContours()
{
	// ��ԭ��ɫͼ������ȡ����������ɫ����
	Mat gray;
	if (cvimg.channels() > 1) {
		cvtColor(cvimg, gray, COLOR_BGR2GRAY);
	}
	else {
		gray = cvimg.clone();
	}

	threshold(gray, gray, 218, 255, THRESH_BINARY);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(0, 255, 0);
		drawContours(cvimg, contours, (int)i, color, 2, 8, hierarchy, 0, Point());
	}
}

void COpenCVProcess::Dib2Mat(CDib& dib)
{
	if (true == dib.IsNull())
	{
		return;
	}
	int nChannels = dib.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = dib.GetWidth();
	int nHeight = dib.GetHeight();


	//�ؽ�mat
	if (1 == nChannels)
	{
		cvimg.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		cvimg.create(nHeight, nWidth, CV_8UC3);
	}

	//��������
	uchar* pucRow;									//ָ������������ָ��
	uchar* pucImage = (uchar*)dib.GetBits();		//ָ����������ָ��
	int nStep = dib.GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (cvimg.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
}

void COpenCVProcess::Mat2Dib(CDib& dib)
{
	if (0 == cvimg.total())
	{
		return;
	}
	int nChannels = cvimg.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = cvimg.cols;
	int nHeight = cvimg.rows;

	//�ؽ�CDib
	dib.Destroy();
	dib.Create(nWidth, nHeight, 8 * nChannels);

	//��������
	uchar* pucRow;									//ָ������������ָ��
	uchar* pucImage = (uchar*)dib.GetBits();		//ָ����������ָ��
	int nStep = dib.GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�
	dib.m_nWidth = nWidth;
	dib.m_nHeight = nHeight;
	dib.m_nWidthBytes = abs(nStep);
	dib.m_nBitCount = 8 * nChannels;
	dib.m_pDibBits = (unsigned char*)dib.GetBits() + (nHeight - 1) * dib.GetPitch();

	if (1 == nChannels)								//���ڵ�ͨ����ͼ����Ҫ��ʼ����ɫ��
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		dib.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		dib.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (cvimg.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
}
