#pragma once
#include "atlimage.h"

#include <complex>
#include <vector>
#include <string>
#include "Matrix.h"
using namespace std;

#define  MAX_SIZE 1000
const int Smoth_Kernel[9] = { 1,1,1,1,1,1,1,1,1 };
#define Pi 3.1415926535897932354626
#define Ei 2.71828

class CDib :
	public CImage
{
public:
	CDib(void);
	CDib(CDib& Dib);
	~CDib(void);

public:
	enum chan_color { chan_red = 1, chan_green, chan_blue, chan_black };
	enum HSIChannel { HUE = 0, SATURATION, INTENSITY };

public:
	void LoadFile(LPCTSTR lpszPathName);
	//void LoadRawFile(LPCTSTR lpszPathName);
	void LoadRawFile(istream& data);
	void LoadRawFile(LPCTSTR lpszPathName);
	void CreateDisplayDib(uint16_t* pRawBuffer, int nWidth, int nHeight, int nBitCount);
	void CreateWhiteRect(double = 512, double = 512, double = 100, double = 100);
	void CreateColorRing(double = 512, double = 512);


public:
	void Stretching();
	void Invert();
	void Invert_Palette();
	void BitPlane_Slicing(int = 7);
	void Histogram_Equalization();
	void Smoothing();
	void Sobel();
	void Laplace();
	void Gaussian1D(int = 19);	// 一维可分离卷积核卷积高斯滤波
	void Gaussian2D(int = 19);	// 二维高斯滤波

public:
	void BoneWindow(double = 140, double = 70);	//显示肺部CT图像骨窗
	void LungWindow(double = 64, double = 92);	//显示肺部CT图像肺窗
	void Show(ImageMatrix const& image);
	//RealMatrix Transverse(int num);	// 横截面
	//RealMatrix Coronal(int num);	// 冠状面
	//RealMatrix Sagittal(int num);	// 矢状面

public:
	Matrix<double> Amplitude();
	Matrix<double> Phase();
	Matrix<double> Filter(CString filter, CString type, double D0, int n);

public:
	Matrix<double> HufnagelDegration(double k);
	Matrix<double> InverseFilter(double D0, double k);
	Matrix<double> WienerFilter(double k);	// 维纳滤波
	Matrix<double> MedianFilter(int = 3);	// 中值滤波
	Matrix<double> AdaptiveMedianFilter(int = 19);	// 均值滤波
	Matrix<double> BilateralFilter(int diameter, double sigmaDistance, double sigmaIntensity);

public:
	// 通过RGB通道获取HSI通道
	Matrix<double> GetHSIChannel(HSIChannel channel);
	// 显示HSI的某一通道
	void ShowHSIChannel(HSIChannel channel);
	// 将HSI通道通过RGB通道显示
	void RGB2HSI();
	void HSI2RGB();

public:
	long* GrayValueCount();	// 获取灰度值统计
	//unsigned char* Padding(int, int);
	Matrix<double> Padding(int, int);	// 填充
	Matrix<double> Conv2d(Matrix<double> const&, int = 1, int = 0, int = 0);	// 2D卷积
	Matrix<double> Tovector();	// 将m_pDibBits读取到vector<vector<double>>
	ImageSet TovectorMatrix();			// 将m_pRawBuffers读取到vector<Matrix<double>>
	void Read(vector<vector<unsigned char>> const&);	// 将vector<vector<unsigned char>>读取到m_pDibBits
	void Read(vector<vector<double>> const&);			// 将vector<vector<double>>读取到m_pDibBits
	void Window_1(double = 127.5, double = 255);
	double PSNR(Matrix<double> const& OriImage);

public:
	BOOL FFT(unsigned char*, long, long);
	void FFT_2D(complex<double>*, int, int, complex<double>*);
	void FFT_1D(complex<double>*, complex<double>*, int);
	void IFFT_2D(complex<double>*, complex<double>*, int, int);
	void IFFT_1D(complex<double>*, complex<double>*, int);

private:
	long m_nWidth;
	long m_nHeight;
	int m_nWidthBytes;
	int m_nBitCount;
	unsigned char* m_pDibBits;	// 指向位图数据的指针

private:
	uint16_t* m_pRawBuffer;		// 指向16位数据的指针
	vector<uint16_t*> m_pRawBuffers;	// 指向多张16位数据的指针
	int m_numImages;	// 图像数量

private:
	long* m_pGrayValueCount;
};

