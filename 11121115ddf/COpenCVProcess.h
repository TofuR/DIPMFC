#pragma once
//--------------------------------------------------------------
//COpenCVProcess��,����OpenCVͼ����
//--------------------------------------------------------------
using namespace cv;
class CDib;

class COpenCVProcess
{
public:
	COpenCVProcess(CDib* pDib);
	~COpenCVProcess(void);

public:
	Mat cvimg, cvimgbuffer;

public:
	void OpenCVGaussianBlur();

public:
	// ��̬ѧ����
	void OpenCVBinarization(int nThreshold = 218);
	void OpenCVInvert();
	void OpenCVErode();
	void OpenCVDilate();
	void OpenCVEdge();	// ��ԭͼ���ȥ��ʴͼ��õ���Ե
	void OpenCVFindContours();	// ��ԭ��ɫͼ������ȡ����������ɫ����



public:
	//transfering between CDib and cv::Mat
	void Dib2Mat(CDib& dib);
	void Mat2Dib(CDib& dib);
};

