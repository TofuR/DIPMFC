#include "pch.h"
#include "functions.h"
#include <algorithm>


double findmax(Matrix<double> const& matrix)
{
	vector<double> temp;
	for (const auto& row : matrix) {
		temp.insert(temp.end(), row.begin(), row.end());
	}
	return *max_element(temp.begin(), temp.end());
}

ComplexMatrix Double2Complex(Matrix<double> const& realData) {
	ComplexMatrix complexData;
	for (const auto& row : realData) {
		vector<complex<double>> complexRow;
		for (const auto& value : row) {
			complexRow.push_back(complex<double>(value, 0.0)); // ����������0���
		}
		complexData.push_back(complexRow);
	}
	return complexData;
}

Matrix<double> Complex2Double(ComplexMatrix const& input) {
	Matrix<double> output;
	// ����ԭʼ��ά����
	for (const auto& row : input) {
		vector<double> doubleRow;
		// ��ÿ�������ķ���ֵ��ӵ��µĶ�ά������
		for (const auto& complexNumber : row) {
			doubleRow.push_back(abs(complexNumber)); // ��ȡ�����ķ���ֵ
		}
		output.push_back(doubleRow);
	}
	return output;
}

uint16_t* Matrix2Uint16(Matrix<double> const& matrix)
{
	int rows = matrix.size();
	int cols = matrix[0].size();
	uint16_t* result = new uint16_t[rows * cols];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			result[i * cols + j] = matrix[i][j];
		}
	}
	return result;
}

Matrix<double> ZeroPadding(Matrix<double> const& Data, int a, int b)
{
	int rows = Data.size();
	int cols = Data[0].size();
	Matrix<double> result(rows + a * 2, vector<double>(cols + b * 2, 0));
	for (int i = a; i < rows + a; i++) {
		for (int j = b; j < cols + b; j++) {
			result[i][j] = Data[i - a][j - b];
		}
	}
	return result;
}

Matrix<double> Con2d(Matrix<double> const& mat, Matrix<double> const& kernel, int padding)
{
	// ����padding�ľ��
	int rows = mat.size();
	int cols = mat[0].size();
	int kernelRows = kernel.size();
	int kernelCols = kernel[0].size();
	// ����
	Matrix<double> paddedMat = ZeroPadding(mat, padding, padding);
	// ����һ���µ�ͼƬ�������ڴ洢�����Ľ��
	Matrix<double> result(rows, vector<double>(cols, 0.0));
	// ��ԭʼͼƬ��ÿ�����ؽ��о��
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			double sum = 0;
			for (int m = 0; m < kernelRows; m++) {
				for (int n = 0; n < kernelCols; n++) {
					sum += paddedMat[i + m][j + n] * kernel[m][n];
				}
			}
			result[i][j] = sum;
		}
	}
	return result;
}

Matrix<double> BilinearInterpolation(Matrix<double> const& matrix, int newHeight, int newWidth)
{
	int oldHeight = matrix.size();
	int oldWidth = matrix[0].size();

	// ����һ���µ�ͼƬ�������ڴ洢��ֵ��Ľ��
	Matrix<double> interpolatedImage(newHeight, std::vector<double>(newWidth, 0.0));

	// ����ˮƽ�ʹ�ֱ�Ĳ�ֵ����
	double x_ratio = (double)(oldWidth - 1) / (double)(newWidth - 1);
	double y_ratio = (double)(oldHeight - 1) / (double)(newHeight - 1);
	if (x_ratio == 1 && y_ratio == 1)
	{
		return matrix;
	}

	// ����ͼƬ��ÿ�����ؽ���˫���Բ�ֵ
	for (int y = 0; y < newHeight - 1; ++y) {
		for (int x = 0; x < newWidth - 1; ++x) {
			// ������ԭʼͼƬ�ж�Ӧ��λ��
			double x_ori = x * x_ratio;
			double y_ori = y * y_ratio;
			int x_int = (int)x_ori;
			int y_int = (int)y_ori;
			double x_frac = x_ori - x_int;
			double y_frac = y_ori - y_int;

			// ִ��˫���Բ�ֵ
			interpolatedImage[y][x] =
				(1 - x_frac) * (1 - y_frac) * matrix[y_int][x_int] +
				x_frac * (1 - y_frac) * matrix[y_int][x_int + 1] +
				(1 - x_frac) * y_frac * matrix[y_int + 1][x_int] +
				x_frac * y_frac * matrix[y_int + 1][x_int + 1];
		}
	}
	return interpolatedImage;
}

ComplexMatrix FFT(ComplexMatrix const& CTData)
{
	int rows = CTData.size();
	int cols = CTData[0].size();
	// ��ԭʼ����ÿ��Ԫ�س���-1��i+j�η�
	ComplexMatrix shift_T(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			shift_T[i][j] = CTData[i][j] * pow(-1, i + j);
		}
	}
	return FFT_2D(shift_T);
}

ComplexMatrix IFFT(ComplexMatrix const& CFData)
{
	int rows = CFData.size();
	int cols = CFData[0].size();
	ComplexMatrix shift_F(rows, vector<complex<double>>(cols));
	// ��Ƶ�����ݽ���IFFT
	ComplexMatrix shift_T = IFFT_2D(CFData);
	// ��IFFT�������-1��i+j�η�
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			shift_F[i][j] = shift_T[i][j] * pow(-1, i + j);
		}
	}
	return shift_F;
}

vector<complex<double>> FFT_1D(vector<complex<double>> const& CTData) {
	// ��ȡ���ݳ���
	int N = CTData.size();

	// ������ݳ���Ϊ1��ֱ�ӷ���
	if (N == 1) {
		return vector<complex<double>>(1, CTData[0]);
	}

	// �ֱ𴴽�������ż������
	vector<complex<double>> even(N / 2), odd(N / 2);
	for (int i = 0; i < N / 2; ++i) {
		even[i] = CTData[i * 2];
		odd[i] = CTData[i * 2 + 1];
	}

	// �ֱ��������ż�����н���FFT
	vector<complex<double>> even_fft = FFT_1D(even);
	vector<complex<double>> odd_fft = FFT_1D(odd);

	// ����FFT�������
	vector<complex<double>> result(N);
	for (int k = 0; k < N / 2; ++k) {
		complex<double> t = polar(1.0, -2 * Pi * k / N) * odd_fft[k];
		result[k] = even_fft[k] + t;
		result[k + N / 2] = even_fft[k] - t;
	}

	return result;
}

vector<complex<double>> IFFT_1D(vector<complex<double>> const& CFData) {
	int N = CFData.size();
	if (N == 1) {
		return vector<complex<double>>(1, CFData[0]);
	}
	vector<complex<double>> even(N / 2), odd(N / 2);
	for (int i = 0; i < N / 2; ++i) {
		even[i] = CFData[i * 2];
		odd[i] = CFData[i * 2 + 1];
	}
	vector<complex<double>> even_ifft = IFFT_1D(even);
	vector<complex<double>> odd_ifft = IFFT_1D(odd);
	vector<complex<double>> result(N);
	for (int k = 0; k < N / 2; ++k) {
		complex<double> t = polar(1.0, 2 * Pi * k / N) * odd_ifft[k];
		result[k] = (even_ifft[k] + t) / 2.0;
		result[k + N / 2] = (even_ifft[k] - t) / 2.0;
	}
	return result;
}

ComplexMatrix FFT_2D(ComplexMatrix const& CTData)
{
	int rows = CTData.size();
	int cols = CTData[0].size();

	// ���н���FFT
	ComplexMatrix temp(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; ++i) {
		temp[i] = FFT_1D(CTData[i]);
	}

	// ���н���FFT
	ComplexMatrix result(rows, vector<complex<double>>(cols));
	for (int j = 0; j < cols; ++j) {
		vector<complex<double>> column_data(rows);
		for (int i = 0; i < rows; ++i) {
			column_data[i] = temp[i][j];
		}
		vector<complex<double>> column_fft = FFT_1D(column_data);
		for (int i = 0; i < rows; ++i) {
			result[i][j] = column_fft[i];
		}
	}
	return result;
}

ComplexMatrix IFFT_2D(ComplexMatrix const& CFData) {
	int rows = CFData.size();
	int cols = CFData[0].size();
	ComplexMatrix temp(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; ++i) {
		temp[i] = IFFT_1D(CFData[i]);
	}
	ComplexMatrix result(rows, vector<complex<double>>(cols));
	for (int j = 0; j < cols; ++j) {
		vector<complex<double>> column_data(rows);
		for (int i = 0; i < rows; ++i) {
			column_data[i] = temp[i][j];
		}
		vector<complex<double>> column_ifft = IFFT_1D(column_data);
		for (int i = 0; i < rows; ++i) {
			result[i][j] = column_ifft[i];
		}
	}
	return result;
}

Matrix<double> IdealFilter(double D0, CString type, int nHeight, int nWidth)
{
	Matrix<double> H(nHeight, vector<double>(nWidth, 0));
	int nCenterX = nWidth / 2;
	int nCenterY = nHeight / 2;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			double d = sqrt(pow(i - nCenterY, 2) + pow(j - nCenterX, 2));
			if (type == "lowpass") {
				if (d <= D0) {
					H[i][j] = 1;
				}
			}
			else if (type == "highpass") {
				if (d > D0) {
					H[i][j] = 1;
				}
			}
		}
	}
	return H;
}

Matrix<double> ButterworthFilter(double D0, CString type, int n, int nHeight, int nWidth)
{
	Matrix<double> H(nHeight, vector<double>(nWidth, 0));
	int nCenterX = nWidth / 2;
	int nCenterY = nHeight / 2;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			double d = sqrt(pow(i - nCenterY, 2) + pow(j - nCenterX, 2));
			if (type == "lowpass") {
				H[i][j] = 1 / (1 + pow(d / D0, 2 * n));
			}
			else if (type == "highpass") {
				H[i][j] = 1 / (1 + pow(D0 / d, 2 * n));
			}
		}
	}
	return H;
}

Matrix<double> GaussianFilter(double D0, CString type, int nHeight, int nWidth)
{
	Matrix<double> H(nHeight, vector<double>(nWidth, 0));
	int nCenterX = nWidth / 2;
	int nCenterY = nHeight / 2;
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			double d = sqrt(pow(i - nCenterY, 2) + pow(j - nCenterX, 2));
			if (type == "lowpass") {
				H[i][j] = exp(-pow(d, 2) / (2 * pow(D0, 2)));
			}
			else if (type == "highpass") {
				H[i][j] = 1 - exp(-pow(d, 2) / (2 * pow(D0, 2)));
			}

		}
	}
	return H;
}

ComplexMatrix ApplyFilter(ComplexMatrix const& CFData, Matrix<double> const& FilterData)
{
	int rows = CFData.size();
	int cols = CFData[0].size();
	ComplexMatrix result(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			result[i][j] = CFData[i][j] * FilterData[i][j];
		}
	}
	return result;
}

Matrix<double> MedianFilter(Matrix<double> const& Data, int nHeight, int nWidth, int nSize)
{
	// Ϊ�˷��㴦��߽磬��ԭʼ���ݽ��������
	Matrix<double> DataZeroPadding = ZeroPadding(Data, nSize / 2, nSize / 2);
	Matrix<double> result(nHeight, vector<double>(nWidth, 0));
	for (int i = nSize / 2; i < nHeight + nSize / 2; i++) {
		for (int j = nSize / 2; j < nWidth + nSize / 2; j++) {
			vector<double> temp;
			for (int m = -nSize / 2; m <= nSize / 2; m++) {
				for (int n = -nSize / 2; n <= nSize / 2; n++) {
					temp.push_back(DataZeroPadding[i + m][j + n]);
				}
			}
			sort(temp.begin(), temp.end());
			result[i - nSize / 2][j - nSize / 2] = temp[nSize * nSize / 2];
		}
	}
	return result;
}

Matrix<double> AdaptiveMedianFilter(Matrix<double> const& Data, int nHeight, int nWidth, int nSizeMax)
{
	// Ϊ�˷��㴦��߽磬��ԭʼ���ݽ��������
	Matrix<double> DataZeroPadding = ZeroPadding(Data, nSizeMax / 2, nSizeMax / 2);
	Matrix<double> result(nHeight, vector<double>(nWidth, 0));
	for (int i = nSizeMax / 2; i < nHeight + nSizeMax / 2; i++) {
		for (int j = nSizeMax / 2; j < nWidth + nSizeMax / 2; j++) {
			int nSize = 3;
			while (nSize <= nSizeMax) {
				vector<double> temp;
				for (int m = -nSize / 2; m <= nSize / 2; m++) {
					for (int n = -nSize / 2; n <= nSize / 2; n++) {
						temp.push_back(DataZeroPadding[i + m][j + n]);
					}
				}
				sort(temp.begin(), temp.end());
				double Zmin = temp[0];
				double Zmax = temp[nSize * nSize - 1];
				double Zmed = temp[nSize * nSize / 2];
				double Zxy = DataZeroPadding[i][j];
				if (Zmed > Zmin && Zmed < Zmax) {
					if (Zxy > Zmin && Zxy < Zmax) {
						result[i - nSizeMax / 2][j - nSizeMax / 2] = Zxy;
						break;
					}
					else {
						result[i - nSizeMax / 2][j - nSizeMax / 2] = Zmed;
						break;
					}
				}
				else {
					nSize += 2;
				}
			}
		}
	}
	return result;
}

Matrix<double> BilateralFilter(Matrix<double> const& input, int diameter, double sigmaDistance, double sigmaIntensity)
{

	int rows = input.size();
	int cols = input[0].size();
	//Matrix<double> inputZeroPadding = ZeroPadding(input, diameter / 2, diameter / 2);
	Matrix<double> inputZeroPadding = input;
	Matrix<double> output(rows, std::vector<double>(cols, 0.0));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			double sum = 0;
			double weightSum = 0;
			for (int m = -diameter / 2; m <= diameter / 2; m++) {
				for (int n = -diameter / 2; n <= diameter / 2; n++) {
					int x = i + m;
					int y = j + n;
					if (x >= 0 && x < rows && y >= 0 && y < cols) {
						double distance = sqrt(pow(m, 2) + pow(n, 2));
						double intensity = abs(inputZeroPadding[i][j] - inputZeroPadding[x][y]);
						double weight = exp(-pow(distance, 2) / (2 * pow(sigmaDistance, 2)) - pow(intensity, 2) / (2 * pow(sigmaIntensity, 2)));
						sum += inputZeroPadding[x][y] * weight;
						weightSum += weight;
					}
				}
			}
			output[i][j] = sum / weightSum;
		}
	}
	return output;
}

Matrix<double> Transverse(ImageSet const& imageSet, int num)
{
	if (num < 0) num = 0;
	if (num >= imageSet.size()) num = imageSet.size() - 1;
	int rows = imageSet[0].size();
	int cols = imageSet[0][0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			result[i][j] = imageSet[num][i][j];
		}
	}
	return result;
}

Matrix<double> Coronal(ImageSet const& imageSet, int num)
{
	if (num < 0) num = 0;
	if (num >= imageSet[0].size()) num = imageSet[0].size() - 1;
	int rows = imageSet.size();
	int cols = imageSet[0][0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			result[i][j] = imageSet[i][num][j];
		}
	}
	return result;
}

Matrix<double> Sagittal(ImageSet const& imageSet, int num)
{
	if (num < 0) num = 0;
	if (num >= imageSet[0][0].size()) num = imageSet[0][0].size() - 1;
	int rows = imageSet.size();
	int cols = imageSet[0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			result[i][j] = imageSet[i][j][num];
		}
	}
	return result;
}

Matrix<double> RGB2HSI(Matrix<double> const& Data)
{
	int rows = Data.size();
	int cols = Data[0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j += 3) {
			double B = Data[i][j] / 255.0;
			double G = Data[i][j + 1] / 255.0;
			double R = Data[i][j + 2] / 255.0;
			double theta = 0;
			double S = 0;
			if (R == G && G == B)
			{
				theta = 0;
				S = 0;
			}
			else
			{
				theta = acos(0.5 * ((R - G) + (R - B)) / sqrt((R - G) * (R - G) + (R - B) * (G - B)));
				S = 1 - 3 * min(R, G, B) / (R + G + B);
			}
			double H = B <= G ? theta : 2 * Pi - theta;
			double I = (R + G + B) / 3;
			result[i][j] = I * 255.0;
			result[i][j + 1] = S * 255;
			result[i][j + 2] = H * 255 / 2 / Pi;
		}
	}
	return result;
}

Matrix<double> HSI2RGB(Matrix<double> const& Data)
{
	int rows = Data.size();
	int cols = Data[0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j += 3) {
			double H = Data[i][j + 2] * 2 * Pi / 255.0;
			double S = Data[i][j + 1] / 255.0;
			double I = Data[i][j + 0] / 255.0;
			double R = 0;
			double G = 0;
			double B = 0;
			if (H >= 0 && H < 2 * Pi / 3)
			{
				B = I * (1 - S);
				R = I * (1 + S * cos(H) / cos(Pi / 3 - H));
				G = 3 * I - R - B;
			}
			else if (H >= 2 * Pi / 3 && H < 4 * Pi / 3)
			{
				H = H - 2 * Pi / 3;
				R = I * (1 - S);
				G = I * (1 + S * cos(H) / cos(Pi / 3 - H));
				B = 3 * I - R - G;
			}
			else
			{
				H = H - 4 * Pi / 3;
				G = I * (1 - S);
				B = I * (1 + S * cos(H) / cos(Pi / 3 - H));
				R = 3 * I - G - B;
			}
			if (R > 1)
				R = 1;
			if (G > 1)
				G = 1;
			if (B > 1)
				B = 1;
			result[i][j] = B * 255;
			result[i][j + 1] = G * 255;
			result[i][j + 2] = R * 255;
		}
	}
	return result;
}

Matrix<double> HSIHistogramEqualization(Matrix<double>& Data)
{
	int rows = Data.size();
	int cols = Data[0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	// ͳ��ֱ��ͼ
	vector<double> S(256, 0);
	vector<double> I(256, 0);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j += 3) {
			I[Data[i][j + 0]]++;
		}
	}
	// ���б任
	for (int i = 0; i < 256; i++) {
		S[i] = i + 10;
		if (S[i] > 255)
			S[i] = 255;
	}
	for (int j = 1; j < 256; j++) {
		I[j] += I[j - 1];
	}

	// ��һ��
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j += 3) {
			result[i][j + 2] = Data[i][j + 2];
			result[i][j + 1] = S[Data[i][j + 1]];
			result[i][j + 0] = I[Data[i][j + 0]] * 255.0 / (rows * cols / 3);
		}
	}
	return result;
}

Matrix<double> Transverse(ImageSet const& imageSet, int num, int newHeight, int newWidth)
{
	return BilinearInterpolation(Transverse(imageSet, num), newHeight, newWidth);
}

Matrix<double> Coronal(ImageSet const& imageSet, int num, int newHeight, int newWidth)
{
	return BilinearInterpolation(Coronal(imageSet, num), newHeight, newWidth);
}

Matrix<double> Sagittal(ImageSet const& imageSet, int num, int newHeight, int newWidth)
{
	return BilinearInterpolation(Sagittal(imageSet, num), newHeight, newWidth);
}

Matrix<double> StructuringElement(int nSize, CString type)
{
	Matrix<double> result(nSize, std::vector<double>(nSize, 0.0));
	if (type == "square") {
		for (int i = 0; i < nSize; i++) {
			for (int j = 0; j < nSize; j++) {
				result[i][j] = 1;
			}
		}
	}
	else if (type == "cross") {
		for (int i = 0; i < nSize; i++) {
			result[nSize / 2][i] = 1;
			result[i][nSize / 2] = 1;
		}
	}
	else if (type == "circle") {
		int nCenter = nSize / 2;
		for (int i = 0; i < nSize; i++) {
			for (int j = 0; j < nSize; j++) {
				if (pow(i - nCenter, 2) + pow(j - nCenter, 2) <= pow(nCenter, 2)) {
					result[i][j] = 1;
				}
			}
		}
	}
	return result;
}

Matrix<double> GaussianKernel(double sigma)
{
	int nSize = 2 * ceil(3 * sigma) + 1;
	Matrix<double> result(nSize, std::vector<double>(nSize, 0.0));
	double sum = 0;
	for (int i = 0; i < nSize; i++) {
		for (int j = 0; j < nSize; j++) {
			result[i][j] = exp(-(pow(i - nSize / 2, 2) + pow(j - nSize / 2, 2)) / (2 * pow(sigma, 2)));
			sum += result[i][j];
		}
	}
	for (int i = 0; i < nSize; i++) {
		for (int j = 0; j < nSize; j++) {
			result[i][j] /= sum;
		}
	}
	return result;
}

Matrix<double> GaussianBlur(Matrix<double> const& Data, double sigma)
{
	Matrix<double> kernel = GaussianKernel(sigma);
	return Con2d(Data, kernel, kernel.size() / 2);
}

void SobelOperator(Matrix<double> const& Data, int nSize, Matrix<double>& Amplitude, Matrix<double>& Angle)
{
	Matrix<double> kernelX = { {1, 0, -1}, {2, 0, -2}, {1, 0, -1} };
	Matrix<double> kernelY = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
	Matrix<double> Gx = Con2d(Data, kernelX, nSize / 2);
	Matrix<double> Gy = Con2d(Data, kernelY, nSize / 2);
	int rows = Data.size();
	int cols = Data[0].size();
	Amplitude = Matrix<double>(rows, std::vector<double>(cols, 0.0));
	Angle = Matrix<double>(rows, std::vector<double>(cols, 0.0));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			Amplitude[i][j] = sqrt(pow(Gx[i][j], 2) + pow(Gy[i][j], 2));
			Angle[i][j] = atan2(Gy[i][j], Gx[i][j]);
		}
	}
}

Matrix<double> NonMaximumSuppression(Matrix<double> const& Amplitude, Matrix<double> const& Angle)
{
	int rows = Amplitude.size();
	int cols = Amplitude[0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			double angle = Angle[i][j];
			double value = Amplitude[i][j];
			if ((angle >= -Pi / 8 && angle < Pi / 8) || (angle >= 7 * Pi / 8 && angle < -7 * Pi / 8)) {
				if (value > Amplitude[i][j - 1] && value > Amplitude[i][j + 1]) {
					result[i][j] = value;
				}
			}
			else if ((angle >= Pi / 8 && angle < 3 * Pi / 8) || (angle >= -7 * Pi / 8 && angle < -5 * Pi / 8)) {
				if (value > Amplitude[i - 1][j + 1] && value > Amplitude[i + 1][j - 1]) {
					result[i][j] = value;
				}
			}
			else if ((angle >= 3 * Pi / 8 && angle < 5 * Pi / 8) || (angle >= -5 * Pi / 8 && angle < -3 * Pi / 8)) {
				if (value > Amplitude[i - 1][j] && value > Amplitude[i + 1][j]) {
					result[i][j] = value;
				}
			}
			else {
				if (value > Amplitude[i - 1][j - 1] && value > Amplitude[i + 1][j + 1]) {
					result[i][j] = value;
				}
			}
		}
	}
	return result;
}

Matrix<double> DoubleThreshold(Matrix<double> const& NMS, double lowThreshold, double highThreshold) {
	int rows = NMS.size();
	int cols = NMS[0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (NMS[i][j] >= highThreshold) {
				result[i][j] = 255;
			}
			else if (NMS[i][j] >= lowThreshold) {
				result[i][j] = 128;
			}
			else {
				result[i][j] = 0;
			}
		}
	}

	return result;
}

Matrix<double> EdgeTracking(Matrix<double> const& DT)
{
	int rows = DT.size();
	int cols = DT[0].size();
	Matrix<double> result(rows, std::vector<double>(cols, 0.0));
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			if (DT[i][j] == 128) {
				if (DT[i - 1][j - 1] == 255 || DT[i - 1][j] == 255 || DT[i - 1][j + 1] == 255 ||
					DT[i][j - 1] == 255 || DT[i][j + 1] == 255 || DT[i + 1][j - 1] == 255 ||
					DT[i + 1][j] == 255 || DT[i + 1][j + 1] == 255) {
					result[i][j] = 255;
				}
			}
		}
	}
	return result;
}

Matrix<double> Canny(Matrix<double> const& Data, double sigma, double lowThreshold, double highThreshold) {
	// Step 1: Gaussian Blur
	Matrix<double> blurred = GaussianBlur(Data, sigma);

	// Step 2: Sobel Operator
	Matrix<double> Amplitude, Angle;
	SobelOperator(blurred, 3, Amplitude, Angle);

	// Step 3: Non-Maximum Suppression
	Matrix<double> nms = NonMaximumSuppression(Amplitude, Angle);

	// Step 4: Double Threshold
	Matrix<double> dt = DoubleThreshold(nms, lowThreshold, highThreshold);

	// Step 5: Edge Tracking by Hysteresis
	EdgeTracking(dt);

	return dt;
}



//ComplexMatrix FFT(ComplexMatrix const& CTData, long nWidth, long nHeight) {
//	unsigned char* lpSrc;      // ָ��Դͼ���ָ��
//	int y;                     // ѭ�����Ʊ���
//	int x;                     // ѭ�����Ʊ���
//	double dTmpOne;            // �����ʱ����
//	double dTmpTwo;            // �����ʱ����
//	int nTransWidth;           // ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
//	int nTransHeight;          // ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
//	double unchValue;          // ����ͼ������ػҶȵ���ʱ����
//	complex<double>* pCTData;  // ָ��ʱ�����ݵ�ָ��
//	complex<double>* pCFData;  // ָ��Ƶ�����ݵ�ָ��
//	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
//	dTmpOne = log(1.0 * m_nWidth) / log(2.0);
//	dTmpTwo = ceil(dTmpOne);
//	dTmpTwo = pow(2, dTmpTwo);
//	nTransWidth = (int)dTmpTwo;
//	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
//	dTmpOne = log(1.0 * m_nHeight) / log(2.0);
//	dTmpTwo = ceil(dTmpOne);
//	dTmpTwo = pow(2, dTmpTwo);
//	nTransHeight = (int)dTmpTwo;
//	double dReal;  // ����Ҷ�任��ʵ��
//	double dImag;  // ����Ҷ�任���鲿
//
//	pCTData = new complex<double>[nTransWidth * nTransHeight];  // �����ڴ�
//	pCFData = new complex<double>[nTransWidth * nTransHeight];  // �����ڴ�
//	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
//	for (y = 0; y < nTransHeight; y++) {
//		for (x = 0; x < nTransWidth; x++) {
//			pCTData[y * nTransWidth + x] = complex<double>(0, 0);  // ����
//		}
//	}
//	// ��ͼ�����ݴ���pCTData
//	for (y = 0; y < m_nHeight; y++) {
//		for (x = 0; x < m_nWidth; x++) {
//			// ָ��DIB��y�У���x�����ص�ָ��
//			lpSrc = (unsigned char*)pDIBBits + m_nWidth * (m_nHeight - 1 - y) + x;
//			unchValue = (*lpSrc) * pow(-1.0, x + y);
//			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
//		}
//	}
//	FFT_2D(pCTData, m_nWidth, m_nHeight, pCFData);  // ����Ҷ���任
//
//	// ����Ƶ������
//	ComplexMatrix pCFData2D(m_nHeight, vector<complex<double>>(m_nWidthBytes, 0));
//	for (int i = 0; i < m_nHeight; i++) {
//		for (int j = 0; j < m_nWidthBytes; j++) {
//			pCFData2D[i][j] = pCFData[i * nTransWidth + j];
//		}
//	}
//	delete[] pCTData;
//	delete[] pCFData;
//	pCTData = NULL;
//	pCFData = NULL;
//	return pCFData2D;
//}


//ComplexMatrix CDib::IFFT(ComplexMatrix const& CFData)
//{
//	// ʹ��FFTʵ��IFFT
//	ComplexMatrix pCTData2D(m_nHeight, vector<complex<double>>(m_nWidthBytes, 0));
//	for (int i = 0; i < m_nHeight; i++) {
//		for (int j = 0; j < m_nWidthBytes; j++) {
//			pCTData2D[i][j] = freq[i][j];
//		}
//	}
//
//}


//Matrix<double> CDib::Amplitude()
//{
	//unsigned char* pDIBBits = m_pDibBits;
	//long nWidth = m_nWidth;
	//long nHeight = m_nHeight;
	//unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	//int y;										// ѭ�����Ʊ���
	//int x;										// ѭ�����Ʊ���
	//double dTmpOne;								//�����ʱ����
	//double dTmpTwo;								//�����ʱ����
	//int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	//int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	//double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	//complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	//complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
	//// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	//dTmpOne = log(1.0 * nWidth) / log(2.0);
	//dTmpTwo = ceil(dTmpOne);
	//dTmpTwo = pow(2, dTmpTwo);
	//nTransWidth = (int)dTmpTwo;
	//// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	//dTmpOne = log(1.0 * nHeight) / log(2.0);
	//dTmpTwo = ceil(dTmpOne);
	//dTmpTwo = pow(2, dTmpTwo);
	//nTransHeight = (int)dTmpTwo;
	//double dReal;									// ����Ҷ�任��ʵ��
	//double dImag;									// ����Ҷ�任���鲿

	//pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	//pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	//// ͼ�����ݵĿ�͸߲�һ����2��
	//// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	//for (y = 0; y < nTransHeight; y++)
	//{
	//	for (x = 0; x < nTransWidth; x++)
	//	{
	//		pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
	//	}
	//}
	////��ͼ�����ݴ���pCTData
	//for (y = 0; y < nHeight; y++)
	//{
	//	for (x = 0; x < nWidth; x++)
	//	{
	//		// ָ��DIB��y�У���x�����ص�ָ��
	//		lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
	//		unchValue = (*lpSrc) * pow(-1.0, x + y);
	//		pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
	//	}
	//}
	//FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任

	//double max = pCFData[(nTransHeight / 2) * nTransWidth + (nTransWidth / 2)].real();
	//max = log(max + 1);
	//double r = 255 / max;
	//int ndHeight = (nTransHeight - nHeight) / 2;
	//int ndWidth = (nTransWidth - nWidth) / 2;
	//for (y = ndHeight; y < (nTransHeight - ndHeight); y++)
	//{
	//	for (x = ndWidth; x < (nTransWidth - ndWidth); x++)
	//	{
	//		//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
	//		double dTmp = abs(pCFData[y * nTransWidth + x]);
	//		double dTmp1 = log(dTmp + 1);
	//		dTmp1 = dTmp1 * r;
	//		// ָ��DIB��y�У���x�����ص�ָ��
	//		lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
	//		*lpSrc = (BYTE)dTmp1;
	//	}
	//}

	//delete[] pCTData;										// �ͷ��ڴ�
	//delete[] pCFData;										// �ͷ��ڴ�
	//pCTData = NULL;
	//pCFData = NULL;
//
//}