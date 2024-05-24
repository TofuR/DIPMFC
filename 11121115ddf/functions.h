#pragma once
#include <complex>
#include <vector>
#include "Matrix.h"
using namespace std;

#define Pi 3.1415926535897932354626
#define Ei 2.71828
#define min(a,b,c) (a<b?(a<c?a:c):(b<c?b:c))

// ���ܺ���

double findmax(Matrix<double> const&);

ComplexMatrix Double2Complex(Matrix<double> const&);

Matrix<double> Complex2Double(ComplexMatrix const&);

uint16_t* Matrix2Uint16(Matrix<double> const&);

Matrix<double> ZeroPadding(Matrix<double> const&, int, int);

Matrix<double> Con2d(Matrix<double> const&, Matrix<double> const&, int padding);

// ˫���Բ�ֵ

Matrix<double> BilinearInterpolation(Matrix<double> const&, int newWidth, int newHeight);

// FFT and IFFT

ComplexMatrix FFT(ComplexMatrix const&);

ComplexMatrix IFFT(ComplexMatrix const&);

vector<complex<double>> FFT_1D(vector<complex<double>> const& CTData);

vector<complex<double>> IFFT_1D(vector<complex<double>> const& CFData);

ComplexMatrix FFT_2D(ComplexMatrix const& CTData);

ComplexMatrix IFFT_2D(ComplexMatrix const& CFData);

// Filter

Matrix<double> IdealFilter(double D0, CString type, int nHeight, int nWidth);

Matrix<double> ButterworthFilter(double D0, CString type, int n, int nHeight, int nWidth);

Matrix<double> GaussianFilter(double D0, CString type, int nHeight, int nWidth);

ComplexMatrix ApplyFilter(ComplexMatrix const& CFData, Matrix<double> const& FilterData);

// Image Processing

Matrix<double> MedianFilter(Matrix<double> const& Data, int nHeight, int nWidth, int nSize);

Matrix<double> AdaptiveMedianFilter(Matrix<double> const& Data, int nHeight, int nWidth, int nSizeMax);

Matrix<double> BilateralFilter(Matrix<double> const& input, int diameter, double sigmaDistance, double sigmaIntensity);

// ��ImageSet��ʾĳһ����

Matrix<double> Transverse(ImageSet const& imageSet, int num);

Matrix<double> Coronal(ImageSet const& imageSet, int num);

Matrix<double> Sagittal(ImageSet const& imageSet, int num);

// ��ɫͼ����

Matrix<double> RGB2HSI(Matrix<double> const& Data);

Matrix<double> HSI2RGB(Matrix<double> const& Data);

Matrix<double> HSIHistogramEqualization(Matrix<double>& Data);

// ��ImageSet��ʾĳһ���棬�涨�������������˫���Բ�ֵ

Matrix<double> Transverse(ImageSet const& imageSet, int num, int newHeight, int newWidth);

Matrix<double> Coronal(ImageSet const& imageSet, int num, int newHeight, int newWidth);

Matrix<double> Sagittal(ImageSet const& imageSet, int num, int newHeight, int newWidth);

// ��̬ѧͼ����

Matrix<double> StructuringElement(int nSize, CString type);


// CANNY��Ե���

Matrix<double> GaussianKernel(double sigma);// ������˹��

Matrix<double> GaussianBlur(Matrix<double> const& Data, double sigma);// ��˹�˲�

void SobelOperator(Matrix<double> const& Data, int nSize, Matrix<double>& Amplitude, Matrix<double>& Angle);// Sobel���ӷ����ݶȺͷ���

Matrix<double> NonMaximumSuppression(Matrix<double> const& Amplitude, Matrix<double> const& Angle);// �Ǽ���ֵ����

Matrix<double> DoubleThreshold(Matrix<double> const& Data, double lowThreshold, double highThreshold);// ˫��ֵ����

Matrix<double> EdgeTracking(Matrix<double> const& DT);// ��Ե����

Matrix<double> Canny(Matrix<double> const& Data, double sigma, double lowThreshold, double highThreshold);// Canny��Ե���
