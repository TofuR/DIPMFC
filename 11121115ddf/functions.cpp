#include "pch.h"
#include "functions.h"
#include <algorithm>


double findmax(vector<vector<double>> const& matrix)
{
	vector<double> temp;
	for (const auto& row : matrix) {
		temp.insert(temp.end(), row.begin(), row.end());
	}
	return *max_element(temp.begin(), temp.end());
}

vector<vector<complex<double>>> Double2Complex(vector<vector<double>> const& realData) {
	vector<vector<complex<double>>> complexData;
	for (const auto& row : realData) {
		vector<complex<double>> complexRow;
		for (const auto& value : row) {
			complexRow.push_back(complex<double>(value, 0.0)); // ����������0���
		}
		complexData.push_back(complexRow);
	}
	return complexData;
}

vector<vector<double>> Complex2Double(vector<vector<complex<double>>> const& input) {
	vector<vector<double>> output;
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

vector<vector<double>> ZeroPadding(vector<vector<double>> const& Data, int a, int b)
{
	int rows = Data.size();
	int cols = Data[0].size();
	vector<vector<double>> result(rows + a * 2, vector<double>(cols + b * 2, 0));
	for (int i = a; i < rows + a; i++) {
		for (int j = b; j < cols + b; j++) {
			result[i][j] = Data[i - a][j - b];
		}
	}
	return result;
}

vector<vector<complex<double>>> FFT(vector<vector<complex<double>>> const& CTData)
{
	int rows = CTData.size();
	int cols = CTData[0].size();
	// ��ԭʼ����ÿ��Ԫ�س���-1��i+j�η�
	vector<vector<complex<double>>> shift_T(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			shift_T[i][j] = CTData[i][j] * pow(-1, i + j);
		}
	}
	return FFT_2D(shift_T);
}

vector<vector<complex<double>>> IFFT(vector<vector<complex<double>>> const& CFData)
{
	int rows = CFData.size();
	int cols = CFData[0].size();
	vector<vector<complex<double>>> shift_F(rows, vector<complex<double>>(cols));
	// ��Ƶ�����ݽ���IFFT
	vector<vector<complex<double>>> shift_T = IFFT_2D(CFData);
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

vector<vector<complex<double>>> FFT_2D(vector<vector<complex<double>>> const& CTData)
{
	int rows = CTData.size();
	int cols = CTData[0].size();

	// ���н���FFT
	vector<vector<complex<double>>> temp(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; ++i) {
		temp[i] = FFT_1D(CTData[i]);
	}

	// ���н���FFT
	vector<vector<complex<double>>> result(rows, vector<complex<double>>(cols));
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

vector<vector<complex<double>>> IFFT_2D(vector<vector<complex<double>>> const& CFData) {
	int rows = CFData.size();
	int cols = CFData[0].size();
	vector<vector<complex<double>>> temp(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; ++i) {
		temp[i] = IFFT_1D(CFData[i]);
	}
	vector<vector<complex<double>>> result(rows, vector<complex<double>>(cols));
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

vector<vector<double>> IdealFilter(double D0, CString type, int nHeight, int nWidth)
{
	vector<vector<double>> H(nHeight, vector<double>(nWidth, 0));
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

vector<vector<double>> ButterworthFilter(double D0, CString type, int n, int nHeight, int nWidth)
{
	vector<vector<double>> H(nHeight, vector<double>(nWidth, 0));
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

vector<vector<double>> GaussianFilter(double D0, CString type, int nHeight, int nWidth)
{
	vector<vector<double>> H(nHeight, vector<double>(nWidth, 0));
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

vector<vector<complex<double>>> ApplyFilter(vector<vector<complex<double>>> const& CFData, vector<vector<double>> const& FilterData)
{
	int rows = CFData.size();
	int cols = CFData[0].size();
	vector<vector<complex<double>>> result(rows, vector<complex<double>>(cols));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			result[i][j] = CFData[i][j] * FilterData[i][j];
		}
	}
	return result;
}

vector<vector<double>> MedianFilter(vector<vector<double>> const& Data, int nHeight, int nWidth, int nSize)
{
	// Ϊ�˷��㴦��߽磬��ԭʼ���ݽ��������
	vector<vector<double>> DataZeroPadding = ZeroPadding(Data, nSize / 2, nSize / 2);
	vector<vector<double>> result(nHeight, vector<double>(nWidth, 0));
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

vector<vector<double>> AdaptiveMedianFilter(vector<vector<double>> const& Data, int nHeight, int nWidth, int nSizeMax)
{
	// Ϊ�˷��㴦��߽磬��ԭʼ���ݽ��������
	vector<vector<double>> DataZeroPadding = ZeroPadding(Data, nSizeMax / 2, nSizeMax / 2);
	vector<vector<double>> result(nHeight, vector<double>(nWidth, 0));
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





//vector<vector<complex<double>>> FFT(vector<vector<complex<double>>> const& CTData, long nWidth, long nHeight) {
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
//	vector<vector<complex<double>>> pCFData2D(m_nHeight, vector<complex<double>>(m_nWidthBytes, 0));
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


//vector<vector<complex<double>>> CDib::IFFT(vector<vector<complex<double>>> const& CFData)
//{
//	// ʹ��FFTʵ��IFFT
//	vector<vector<complex<double>>> pCTData2D(m_nHeight, vector<complex<double>>(m_nWidthBytes, 0));
//	for (int i = 0; i < m_nHeight; i++) {
//		for (int j = 0; j < m_nWidthBytes; j++) {
//			pCTData2D[i][j] = freq[i][j];
//		}
//	}
//
//}


//vector<vector<double>> CDib::Amplitude()
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