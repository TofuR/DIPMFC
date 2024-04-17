#pragma once
#include <complex>
#include <vector>
#include "Matrix.h"
using namespace std;

#define Pi 3.1415926535897932354626
#define Ei 2.71828

// 功能函数

double findmax(vector<vector<double>> const&);

vector<vector<complex<double>>> Double2Complex(vector<vector<double>> const&);

vector<vector<double>> Complex2Double(vector<vector<complex<double>>> const&);

uint16_t* Matrix2Uint16(RealMatrix const&);

vector<vector<double>> ZeroPadding(vector<vector<double>> const&, int, int);

// 双线性插值

RealMatrix BilinearInterpolation(RealMatrix const&, int newWidth, int newHeight);

// FFT and IFFT

vector<vector<complex<double>>> FFT(vector<vector<complex<double>>> const&);

vector<vector<complex<double>>> IFFT(vector<vector<complex<double>>> const&);

vector<complex<double>> FFT_1D(vector<complex<double>> const& CTData);

vector<complex<double>> IFFT_1D(vector<complex<double>> const& CFData);

vector<vector<complex<double>>> FFT_2D(vector<vector<complex<double>>> const& CTData);

vector<vector<complex<double>>> IFFT_2D(vector<vector<complex<double>>> const& CFData);

// Filter

vector<vector<double>> IdealFilter(double D0, CString type, int nHeight, int nWidth);

vector<vector<double>> ButterworthFilter(double D0, CString type, int n, int nHeight, int nWidth);

vector<vector<double>> GaussianFilter(double D0, CString type, int nHeight, int nWidth);

vector<vector<complex<double>>> ApplyFilter(vector<vector<complex<double>>> const& CFData, vector<vector<double>> const& FilterData);

// Image Processing

vector<vector<double>> MedianFilter(vector<vector<double>> const& Data, int nHeight, int nWidth, int nSize);

vector<vector<double>> AdaptiveMedianFilter(vector<vector<double>> const& Data, int nHeight, int nWidth, int nSizeMax);

RealMatrix BilateralFilter(RealMatrix const& input, int diameter, double sigmaDistance, double sigmaIntensity);

// 对ImageSet显示某一个面

RealMatrix Transverse(ImageSet const& imageSet, int num);

RealMatrix Coronal(ImageSet const& imageSet, int num);

RealMatrix Sagittal(ImageSet const& imageSet, int num);

// 对ImageSet显示某一个面，规定长宽，不足则进行双线性插值

RealMatrix Transverse(ImageSet const& imageSet, int num, int newHeight, int newWidth);

RealMatrix Coronal(ImageSet const& imageSet, int num, int newHeight, int newWidth);

RealMatrix Sagittal(ImageSet const& imageSet, int num, int newHeight, int newWidth);