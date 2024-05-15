#pragma once
//--------------------------------------------------------------
//COpenCVProcess类,用于OpenCV图像处理
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
	// 形态学操作
	void OpenCVBinarization(int nThreshold = 218);
	void OpenCVInvert();
	void OpenCVErode();
	void OpenCVDilate();
	void OpenCVEdge();	// 用原图像减去腐蚀图像得到边缘
	void OpenCVFindContours();	// 将原彩色图像中提取的轮廓用绿色画出



public:
	//transfering between CDib and cv::Mat
	void Dib2Mat(CDib& dib);
	void Mat2Dib(CDib& dib);
};

