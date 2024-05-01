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
	void Gaussian1D(int = 19);	// һά�ɷ������˾����˹�˲�
	void Gaussian2D(int = 19);	// ��ά��˹�˲�

public:
	void BoneWindow(double = 140, double = 70);	//��ʾ�β�CTͼ��Ǵ�
	void LungWindow(double = 64, double = 92);	//��ʾ�β�CTͼ��δ�
	void Show(ImageMatrix const& image);
	//RealMatrix Transverse(int num);	// �����
	//RealMatrix Coronal(int num);	// ��״��
	//RealMatrix Sagittal(int num);	// ʸ״��

public:
	Matrix<double> Amplitude();
	Matrix<double> Phase();
	Matrix<double> Filter(CString filter, CString type, double D0, int n);

public:
	Matrix<double> HufnagelDegration(double k);
	Matrix<double> InverseFilter(double D0, double k);
	Matrix<double> WienerFilter(double k);	// ά���˲�
	Matrix<double> MedianFilter(int = 3);	// ��ֵ�˲�
	Matrix<double> AdaptiveMedianFilter(int = 19);	// ��ֵ�˲�
	Matrix<double> BilateralFilter(int diameter, double sigmaDistance, double sigmaIntensity);

public:
	// ͨ��RGBͨ����ȡHSIͨ��
	Matrix<double> GetHSIChannel(HSIChannel channel);
	// ��ʾHSI��ĳһͨ��
	void ShowHSIChannel(HSIChannel channel);
	// ��HSIͨ��ͨ��RGBͨ����ʾ
	void RGB2HSI();
	void HSI2RGB();

public:
	long* GrayValueCount();	// ��ȡ�Ҷ�ֵͳ��
	//unsigned char* Padding(int, int);
	Matrix<double> Padding(int, int);	// ���
	Matrix<double> Conv2d(Matrix<double> const&, int = 1, int = 0, int = 0);	// 2D���
	Matrix<double> Tovector();	// ��m_pDibBits��ȡ��vector<vector<double>>
	ImageSet TovectorMatrix();			// ��m_pRawBuffers��ȡ��vector<Matrix<double>>
	void Read(vector<vector<unsigned char>> const&);	// ��vector<vector<unsigned char>>��ȡ��m_pDibBits
	void Read(vector<vector<double>> const&);			// ��vector<vector<double>>��ȡ��m_pDibBits
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
	unsigned char* m_pDibBits;	// ָ��λͼ���ݵ�ָ��

private:
	uint16_t* m_pRawBuffer;		// ָ��16λ���ݵ�ָ��
	vector<uint16_t*> m_pRawBuffers;	// ָ�����16λ���ݵ�ָ��
	int m_numImages;	// ͼ������

private:
	long* m_pGrayValueCount;
};

