#pragma once
#include <complex>
#include <vector>
using namespace std;

#define Pi 3.1415926535897932354626
#define Ei 2.71828

double findmax(vector<vector<double>> const&);

vector<vector<complex<double>>> Double2Complex(vector<vector<double>> const&);

vector<vector<double>> Complex2Double(vector<vector<complex<double>>> const&);

vector<vector<complex<double>>> FFT(vector<vector<complex<double>>> const&);

vector<complex<double>> FFT_1D(vector<complex<double>> const& CTData);

vector<complex<double>> IFFT_1D(vector<complex<double>> const& CFData);

vector<vector<complex<double>>> FFT_2D(vector<vector<complex<double>>> const& CTData);

vector<vector<complex<double>>> IFFT_2D(vector<vector<complex<double>>> const& CFData);