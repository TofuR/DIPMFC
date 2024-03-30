#pragma once
#include "atlimage.h"

#include <complex>
#include <vector>
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
	//read raw picture
	void LoadRawFile(LPCTSTR lpszPathName);
	void CreateDisplayDib(uint16_t* pRawBuffer, int nWidth, int nHeight, int nBitCount);
	void CreateWhiteRect(int = 512, int = 512, int = 100, int = 100);

public:
	void Stretching();
	void Invert();
	void Invert_Palette();
	void BitPlane_Slicing(int = 7);
	void Histogram_Equalization();
	void Smoothing();
	void Sobel();
	void Laplace();
	void Gaussian1D(int = 19);	// һά�ɷ������˾����˹�˲�
	void Gaussian2D(int = 19);	// ��ά��˹�˲�
	void BoneWindow(double = 140, double = 70);	//��ʾ�β�CTͼ��Ǵ�
	void LungWindow(double = 64, double = 92);	//��ʾ�β�CTͼ��δ�
	// ��ͼ�����봴��һ���м��ɫ�ľ���8λ�Ҷ�ͼ��



public:
	long* GrayValueCount();	// ��ȡ�Ҷ�ֵͳ��
	//unsigned char* Padding(int, int);
	vector<vector<double>> Padding(int, int);	// ���
	vector<vector<double>> Conv2d(vector<vector<double>> const&, int = 1, int = 0, int = 0);	// 2D���
	void vector2uchar(vector<vector<unsigned char>> const&);	// ��vector<vector<unsigned char>>ת��Ϊunsigned char*
	void vector2uchar(vector<vector<double>> const&);	// ��vector<vector<double>>ת��Ϊunsigned char*
	void Window_1(double = 127.5, double = 255);

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
	unsigned char* m_pDibBits;	// ָ��λͼ���ݵ�ָ��

private:
	uint16_t* m_pRawBuffer;

private:
	long* m_pGrayValueCount;
};

