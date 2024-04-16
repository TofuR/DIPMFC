#pragma once
#include <complex>
#include <vector>

template <typename Pixel>
using Matrix = std::vector<std::vector<Pixel>>;

using ComplexMatrix = Matrix<std::complex<double>>;
using RealMatrix = Matrix<double>;
using ImageSet = std::vector<RealMatrix>;