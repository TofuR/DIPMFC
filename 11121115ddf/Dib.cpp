// clang-format off
#include "pch.h"
// clang-format on

#include "Dib.h"

#include <fstream>
#include <vector>
#include "functions.h"
#include <string>

// CDib
CDib::CDib(void)
	: m_pDibBits(NULL), m_pGrayValueCount(NULL), m_pRawBuffer(NULL) {
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;
	m_numImages = 0;
}

CDib::CDib(CDib& Dib)
	: m_pDibBits(NULL), m_pGrayValueCount(NULL), m_pRawBuffer(NULL) {
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;
	m_numImages = 0;
	if (&Dib == NULL) {
		return;
	}
	if (!IsNull()) {
		Destroy();
	}
	Create(Dib.GetWidth(), Dib.GetHeight(), Dib.GetBPP(), 0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
	m_numImages = Dib.m_numImages;
	if (IsIndexed()) {
		int nColors = Dib.GetMaxColorTableEntries();
		if (nColors > 0) {
			RGBQUAD* pal{};
			pal = new RGBQUAD[nColors];
			Dib.GetColorTable(0, nColors, pal);
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	memcpy(m_pDibBits, Dib.m_pDibBits, m_nHeight * m_nWidthBytes);
}

CDib::~CDib(void) {
	m_pDibBits = NULL;
	m_pRawBuffer = NULL;
	// �ͷ�m_pRawBuffers�е������ڴ�
	for (auto& p : m_pRawBuffers) {
		delete[] p;
	}
	if (m_pGrayValueCount != NULL) {
		delete[] m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}

}


// load file and create
void CDib::LoadFile(LPCTSTR lpszPathName) {
	// ����ļ���
	CString strPathName(lpszPathName);
	int nDotIndex = strPathName.ReverseFind(_T('.'));
	if (nDotIndex == -1) {
		// �ļ���û���ҵ�"."
		AfxMessageBox(_T("��֧�ֵ��ļ���ʽ"));
		return;
	}

	CString strExtension =
		strPathName.Mid(nDotIndex + 1);  // ȡ���ļ���"."�Ժ������
	strExtension.MakeLower();            // ����չ��ת��ΪСд
	if (strExtension == _T("raw")) {
		LoadRawFile(lpszPathName);
	}
	else {
		Load(lpszPathName);
		m_nWidth = GetWidth();
		m_nHeight = GetHeight();
		m_nWidthBytes = abs(GetPitch());
		m_nBitCount = GetBPP();
		m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	}
}

//void CDib::LoadRawFile(LPCTSTR lpszPathName) {
//	m_nWidth = 512;
//	m_nHeight = 512;
//
//	// �����ݴ��ļ�����ֵ�Ļ�����//16λ������raw
//	if (m_pRawBuffer != NULL) {
//		delete[] m_pRawBuffer;
//		m_pRawBuffer = NULL;
//	}
//	m_pRawBuffer = new uint16_t[m_nWidth * m_nHeight];
//	memset(m_pRawBuffer, 0, m_nWidth * m_nHeight * sizeof(uint16_t));
//
//	// ʹ��fistream��ȡraw�ļ�������ֵ��buffer
//	std::ifstream file(lpszPathName, std::ios::binary);
//	file.read(reinterpret_cast<char*>(m_pRawBuffer),
//		m_nWidth * m_nHeight * sizeof(uint16_t));
//
//	// �ļ���ʧ�ܵ������棬Ȼ���˳�
//	if (file.fail()) {
//		AfxMessageBox(_T("��ȡ�ļ�ʧ��"));
//		return;
//	}
//	file.close();
//	// ��Ϊ��ȡ���������ظ�8λ�͵�8λ˳���Ƿ��ģ���������ظߵ�8λ���жԵ���ͬʱ��ȡ���ֵ
//	for (int i = 0; i < m_nWidth * m_nHeight; i++) {
//		m_pRawBuffer[i] =
//			((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
//	}
//	CreateDisplayDib(m_pRawBuffer, m_nWidth, m_nHeight, 8);
//}

void CDib::LoadRawFile(istream& data)
{
	size_t imageSize = m_nWidth * m_nHeight * sizeof(uint16_t);
	m_pRawBuffer = new uint16_t[m_nWidth * m_nHeight];
	memset(m_pRawBuffer, 0, imageSize);

	data.read(reinterpret_cast<char*>(m_pRawBuffer), imageSize);
	if (data.fail()) {
		AfxMessageBox(_T("��ȡ�ļ�ʧ��"));
		return;
	}

	for (int i = 0; i < m_nWidth * m_nHeight; i++) {
		m_pRawBuffer[i] =
			((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
	}
	m_pRawBuffers.push_back(m_pRawBuffer);
}

void CDib::LoadRawFile(LPCTSTR lpszPathName)
{
	m_nWidth = 512;
	m_nHeight = 512;
	m_nBitCount = 16;
	m_nWidthBytes = ((m_nWidth * m_nBitCount + 31) & ~31) / 8;
	// ��raw�ļ�
	std::ifstream file(lpszPathName, std::ios::binary);
	if (!file.is_open()) {
		AfxMessageBox(_T("�޷����ļ�"));
		return;
	}
	size_t imageSize = m_nWidth * m_nHeight * sizeof(uint16_t);
	file.seekg(0, std::ios::end);
	int totalFileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	m_numImages = totalFileSize / imageSize;

	if (m_pRawBuffer != NULL) {
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	if (!m_pRawBuffers.empty()) {
		for (auto& p : m_pRawBuffers) {
			delete[] p;
		}
		m_pRawBuffers.clear();
	}

	for (int i = 0; i < m_numImages; ++i) {
		LoadRawFile(file);
	}

	file.close();
	m_pRawBuffer = m_pRawBuffers[0];
	CreateDisplayDib(m_pRawBuffer, m_nWidth, m_nHeight, 8);
}

void CDib::CreateDisplayDib(uint16_t* pRawBuffer, int nWidth, int nHeight,
	int nBitCount) {
	// Create new Dib
	Create(nWidth, nHeight, 8, 0);
	if (IsIndexed()) {
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0) {
			RGBQUAD* pal = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++) {
				pal[i].rgbBlue = i;
				pal[i].rgbGreen = i;
				pal[i].rgbRed = i;
			}
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	// ��buffer�е�����ֵд��Dib
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j) =
				(BYTE)(pRawBuffer[i * m_nWidth + j] >> 4);
		}
	}
}

void CDib::CreateWhiteRect(double nWidth, double nHeight, double wWidth, double wHeight) {
	Create(nWidth, nHeight, 8, 0);
	if (IsIndexed()) {
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0) {
			RGBQUAD* pal = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++) {
				pal[i].rgbBlue = i;
				pal[i].rgbGreen = i;
				pal[i].rgbRed = i;
			}
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			if (j > (m_nWidth - wWidth) / 2 && j <= (m_nWidth + wWidth) / 2 &&
				i > (m_nHeight - wHeight) / 2 && i <= (m_nHeight + wHeight) / 2) {
				*(m_pDibBits + i * m_nWidthBytes + j) = 255;
			}
			else {
				*(m_pDibBits + i * m_nWidthBytes + j) = 0;
			}
		}
	}
}

void CDib::CreateColorRing(double nWidth, double nHeight)
{
	Create(nWidth, nHeight, 24, 0);
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
	// ������ɫԲ��
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			double x = 200;
			double y = 200;
			double r = 80;
			// ���㵱ǰ���ص㵽Բ�ĵľൔ
			double d = sqrt((i - x) * (i - x) + (j - y) * (j - y));
			if (d < r) {
				*(m_pDibBits + i * m_nWidthBytes + j * 3) = 255;
				*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = 0;
				*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = 0;
			}
			else {
				*(m_pDibBits + i * m_nWidthBytes + j * 3) = 0;
				*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = 0;
				*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = 0;
			}

		}
	}
	// ������ɫԲ��
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			double x = 200;
			double y = 310;
			double r = 80;
			if (sqrt((i - x) * (i - x) + (j - y) * (j - y)) < r) {
				*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = 255;
			}
		}
	}
	// ������ɫԲ��
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			double x = 285;
			double y = 255;
			double r = 80;
			if (sqrt((i - x) * (i - x) + (j - y) * (j - y)) < r) {
				*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = 255;
			}
		}
	}

}


// Image Processing in Spatial Domain

void CDib::Stretching() {
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			double tmp = *(m_pDibBits + i * m_nWidthBytes + j);
			if (tmp <= 100)
				*(m_pDibBits + i * m_nWidthBytes + j) =
				(0.5 * tmp) + 0.5;  // ��0.5��Ϊ����������
			else if (tmp > 100 && tmp < 150)
				*(m_pDibBits + i * m_nWidthBytes + j) = (3 * tmp - 250) + 0.5;
			else
				*(m_pDibBits + i * m_nWidthBytes + j) =
				(11.0 * tmp / 21.0 + 2550.0 / 21.0) + 0.5;
		}
	}
}

void CDib::Invert() {
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j) =
				255 - *(m_pDibBits + i * m_nWidthBytes + j);
		}
	}
}

void CDib::Invert_Palette() {
	if (IsIndexed()) {
		int nColors = this->GetMaxColorTableEntries();
		if (nColors > 0) {
			RGBQUAD* pal{};
			pal = new RGBQUAD[nColors];
			this->GetColorTable(0, nColors, pal);

			for (int i = 0; i < nColors; ++i) {
				pal[i].rgbRed = 255 - pal[i].rgbRed;
				pal[i].rgbGreen = 255 - pal[i].rgbGreen;
				pal[i].rgbBlue = 255 - pal[i].rgbBlue;
			}

			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
}

void CDib::BitPlane_Slicing(int a) {
	if (a < 0 || a > 7) {
		return;
	}

	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j) =
				(*(m_pDibBits + i * m_nWidthBytes + j) >> a & 1) * 255;
		}
	}
}

void CDib::Histogram_Equalization() {
	long nColors = GetMaxColorTableEntries();
	if (nColors == 0) {
		return;
	}
	long* pGrayValueCount = GrayValueCount();
	long* pGrayValueCountSum = new long[nColors];
	memset(pGrayValueCountSum, 0, nColors * sizeof(long));
	pGrayValueCountSum[0] = pGrayValueCount[0];
	for (int i = 1; i < nColors; i++) {
		pGrayValueCountSum[i] = pGrayValueCountSum[i - 1] + pGrayValueCount[i];
	}
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j) = (unsigned char)(255 * pGrayValueCountSum[*(m_pDibBits + i * m_nWidthBytes + j)] / (m_nHeight * m_nWidth) + 0.5);
		}
	}
	delete[] pGrayValueCount;
	delete[] pGrayValueCountSum;
}

void CDib::Smoothing() {
	Matrix<double> Smoth_Kernel =
		Matrix<double>(3, vector<double>(3, 1.0 / 9));
	Matrix<double> pDibBits2D = Conv2d(Smoth_Kernel, 1, 1, 1);
	Read(pDibBits2D);
}

void CDib::Sobel() {
	Matrix<double> Sobel_Kernel_X = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
	Matrix<double> Sobel_Kernel_Y = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
	Matrix<double> pDibBits2D_X = Conv2d(Sobel_Kernel_X, 1, 1, 1);
	Matrix<double> pDibBits2D_Y = Conv2d(Sobel_Kernel_Y, 1, 1, 1);
	Matrix<double> pDibBits2D(m_nHeight,
		vector<double>(m_nWidthBytes, 0));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			pDibBits2D[i][j] = sqrt(pDibBits2D_X[i][j] * pDibBits2D_X[i][j] +
				pDibBits2D_Y[i][j] * pDibBits2D_Y[i][j]);
		}
	}
	Read(pDibBits2D);
}

void CDib::Laplace() {
	Matrix<double> Laplace_Kernel =
		Matrix<double>(3, vector<double>(3, -1));
	Laplace_Kernel[1][1] = 9;
	Matrix<double> pDibBits2D = Conv2d(Laplace_Kernel, 1, 1, 1);
	Read(pDibBits2D);
}

void CDib::Gaussian1D(int kernel_size) {
	Matrix<double> Gaussian_Kernel_X =
		Matrix<double>(1, vector<double>(kernel_size, 0));
	Matrix<double> Gaussian_Kernel_Y =
		Matrix<double>(kernel_size, vector<double>(1, 0));
	double sigma = (kernel_size - 1) / 3;
	double sum = 0.0;
	for (int i = 0; i < kernel_size; i++) {
		Gaussian_Kernel_X[0][i] =
			exp(-((i - kernel_size / 2) * (i - kernel_size / 2)) /
				(2 * sigma * sigma)) /
			(sqrt(2 * Pi) * sigma);
		Gaussian_Kernel_Y[i][0] =
			exp(-((i - kernel_size / 2) * (i - kernel_size / 2)) /
				(2 * sigma * sigma)) /
			(sqrt(2 * Pi) * sigma);
		sum += Gaussian_Kernel_X[0][i];
	}
	for (int i = 0; i < kernel_size; i++) {
		Gaussian_Kernel_X[0][i] /= sum;
		Gaussian_Kernel_Y[i][0] /= sum;
	}
	Matrix<double> pDibBits2D_X =
		Conv2d(Gaussian_Kernel_X, 1, 0, kernel_size / 2);
	Read(pDibBits2D_X);
	Matrix<double> pDibBits2D_Y =
		Conv2d(Gaussian_Kernel_Y, 1, kernel_size / 2, 0);
	Read(pDibBits2D_Y);
}

void CDib::Gaussian2D(int kernel_size) {
	Matrix<double> Gaussian_Kernel =
		Matrix<double>(kernel_size, vector<double>(kernel_size, 0));
	double sigma = (kernel_size - 1) / 3;
	double sum = 0.0;
	for (int i = 0; i < kernel_size; i++) {
		for (int j = 0; j < kernel_size; j++) {
			Gaussian_Kernel[i][j] =
				exp(-((i - kernel_size / 2) * (i - kernel_size / 2) +
					(j - kernel_size / 2) * (j - kernel_size / 2)) /
					(2 * sigma * sigma)) /
				(2 * Pi * sigma * sigma);
			sum += Gaussian_Kernel[i][j];
		}
	}
	for (int i = 0; i < kernel_size; i++) {
		for (int j = 0; j < kernel_size; j++) {
			Gaussian_Kernel[i][j] /= sum;
		}
	}
	Matrix<double> pDibBits2D =
		Conv2d(Gaussian_Kernel, 1, kernel_size / 2, kernel_size / 2);
	Read(pDibBits2D);
}

void CDib::BoneWindow(double midpoint, double width) {
	Window_1(midpoint, width);
}

void CDib::LungWindow(double midpoint, double width) {
	Window_1(midpoint, width);
}

void CDib::Show(ImageMatrix const& image)
{
	m_nWidth = image[0].size();
	m_nHeight = image.size();
	if (m_pRawBuffer != NULL)
		delete[] m_pRawBuffer;

	m_pRawBuffer = Matrix2Uint16(image);
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j) =
				(BYTE)(m_pRawBuffer[i * m_nWidth + j] >> 4);
			//CreateDisplayDib(m_pRawBuffer, nWidth, nHeight, 8);
		}
	}
}
// Image Processing in Frequency Domain

Matrix<double> CDib::Amplitude()
{
	Matrix<double> pDibBits2D = Tovector();
	vector<vector<complex<double>>> CTData = Double2Complex(pDibBits2D);
	vector<vector<complex<double>>> CFData = ::FFT(CTData);
	// ���������
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			CFData[i][j] = log(1 + abs(CFData[i][j]));
		}
	}
	double max = findmax(Complex2Double(CFData));
	double r = 255 / max;
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			CFData[i][j] = CFData[i][j] * r;
		}
	}
	Matrix<double> pDibBits2D_Amplitude = Complex2Double(CFData);
	Read(pDibBits2D_Amplitude);
	return pDibBits2D_Amplitude;
}

Matrix<double> CDib::Phase()
{
	Matrix<double> pDibBits2D = Tovector();
	vector<vector<complex<double>>> CTData = Double2Complex(pDibBits2D);
	vector<vector<complex<double>>> CFData = ::FFT(CTData);
	// ����ͼ�����λ��
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			CFData[i][j] = arg(CFData[i][j]);
		}
	}
	double max = findmax(Complex2Double(CFData));
	double r = 255 / max;
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			CFData[i][j] = CFData[i][j] * r;
		}
	}
	Matrix<double> pDibBits2D_Phase = Complex2Double(CFData);
	Read(pDibBits2D_Phase);
	return pDibBits2D_Phase;
}

Matrix<double> CDib::Filter(CString filter, CString type, double D0, int n)
{
	Matrix<double> pDibBits2D = Tovector();		// ��ȡͼ��Ϊ��ά����
	vector<vector<complex<double>>> CTData = Double2Complex(pDibBits2D);	// ����ά����ת��Ϊ������ά����
	vector<vector<complex<double>>> CFData = ::FFT(CTData);	// �Ը�����ά������и���Ҷ�任����д�˶�vector������FFT��������functions.h���棩
	Matrix<double> Filter;	// �˲���
	if (filter == "Ideal") {
		Filter = ::IdealFilter(D0, type, m_nHeight, m_nWidth);
	}
	else if (filter == "Butterworth") {
		Filter = ::ButterworthFilter(D0, type, n, m_nHeight, m_nWidth);
	}
	else if (filter == "Gaussian") {
		Filter = ::GaussianFilter(D0, type, m_nHeight, m_nWidth);
	}
	else {
		return Matrix<double>();
	}
	CFData = ::ApplyFilter(CFData, Filter);	// ��Ƶ�����ݽ����˲�
	CTData = ::IFFT(CFData);	// ���˲����Ƶ�����ݽ��и���Ҷ���任
	Matrix<double> pDibBits2D_Filter = Complex2Double(CTData);	// ��������ά����ת��Ϊ��ά����
	Read(pDibBits2D_Filter);	// ����ά����д��ͼ��
	return pDibBits2D_Filter;	// ���ض�ά���飬���Ӵ���֮��ĸ�����
}


// Image Restoration

Matrix<double> CDib::HufnagelDegration(double k)
{
	Matrix<double> pDibBits2D = Tovector();	// ��ȡͼ��Ϊ��ά����
	// �Զ�ά�������Hufnagel�˻�
	Matrix<double> H(m_nHeight, vector<double>(m_nWidth, 0));
	// ����Hufnagel and Stanley[1964]�˻��˲���
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			H[i][j] = pow(Ei, -k * pow((pow(i - m_nHeight / 2, 2) + pow(j - m_nWidth / 2, 2)), 5.0 / 6.0));
		}
	}
	vector<vector<complex<double>>> CFData = ::FFT(Double2Complex(pDibBits2D));
	vector<vector<complex<double>>> Filter = Double2Complex(H);
	// ��Ƶ��������Hufnagel�˻��˲������о��
	CFData = ::ApplyFilter(CFData, Complex2Double(Filter));
	Matrix<double> pDibBits2D_De = Complex2Double(::IFFT(CFData));
	Read(pDibBits2D_De);	// ����ά����д��ͼ��
	return pDibBits2D_De;	// ���ض�ά���飬���Ӵ���֮��ĸ�����
}

Matrix<double> CDib::InverseFilter(double D0, double k)
{
	Matrix<double> pDibBits2D = Tovector();	// ��ȡͼ��Ϊ��ά����
	vector<vector<complex<double>>> CTData = Double2Complex(pDibBits2D);	// ����ά����ת��Ϊ������ά����
	vector<vector<complex<double>>> CFData = ::FFT(CTData);	// �Ը�����ά������и���Ҷ�任����д�˶�vector������FFT��������functions.h���棩
	Matrix<double> Filter(m_nHeight, vector<double>(m_nWidth, 0));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			if (sqrt(pow(i - m_nHeight / 2, 2) + pow(j - m_nWidth / 2, 2)) <= D0) {
				Filter[i][j] = 1.0 / pow(Ei, -k * pow((pow(i - m_nHeight / 2, 2) + pow(j - m_nWidth / 2, 2)), 5.0 / 6.0));
			}
			else {
				Filter[i][j] = 0;
			}
		}
	}
	CFData = ::ApplyFilter(CFData, Filter);	// ��Ƶ�����ݽ����˲�
	CTData = ::IFFT(CFData);	// ���˲����Ƶ�����ݽ��и���Ҷ���任
	Matrix<double> pDibBits2D_F = Complex2Double(CTData);	// ��������ά����ת��Ϊ��ά����
	Read(pDibBits2D_F);	// ����ά����д��ͼ��
	return pDibBits2D_F;	// ���ض�ά���飬���Ӵ���֮��ĸ�����
}

Matrix<double> CDib::WienerFilter(double k)
{
	Matrix<double> pDibBits2D = Tovector();
	vector<vector<complex<double>>> CTData = Double2Complex(pDibBits2D);
	vector<vector<complex<double>>> CFData = ::FFT(CTData);
	Matrix<double> H(m_nHeight, vector<double>(m_nWidth, 0));
	Matrix<double> W(m_nHeight, vector<double>(m_nWidth, 0));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			H[i][j] = pow(Ei, -k * pow((pow(i - m_nHeight / 2, 2) + pow(j - m_nWidth / 2, 2)), 5.0 / 6.0));
		}
	}
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			W[i][j] = 1.0 / H[i][j] * (H[i][j] * H[i][j] / (H[i][j] * H[i][j] + 0.01));
		}
	}
	CFData = ::ApplyFilter(CFData, W);	// ��Ƶ�����ݽ����˲�
	CTData = ::IFFT(CFData);
	Matrix<double> pDibBits2D_F = Complex2Double(CTData);
	Read(pDibBits2D_F);
	return pDibBits2D_F;
}

Matrix<double> CDib::MedianFilter(int size)
{
	Matrix<double> pDibBits2D = Tovector();	// ��ȡͼ��Ϊ��ά����
	Matrix<double> pDibBits2D_Filter = ::MedianFilter(pDibBits2D, m_nHeight, m_nWidth, size);	// �Զ�ά���������ֵ�˲�
	Read(pDibBits2D_Filter);	// ����ά����д��ͼ��
	return pDibBits2D_Filter;	// ���ض�ά���飬���Ӵ���֮��ĸ�����
}

Matrix<double> CDib::AdaptiveMedianFilter(int SizeMax)
{
	Matrix<double> pDibBits2D = Tovector();	// ��ȡͼ��Ϊ��ά����
	Matrix<double> pDibBits2D_Filter = ::AdaptiveMedianFilter(pDibBits2D, m_nHeight, m_nWidth, SizeMax);	// �Զ�ά�����������Ӧ��ֵ�˲�
	Read(pDibBits2D_Filter);	// ����ά����д��ͼ��
	return pDibBits2D_Filter;	// ���ض�ά���飬���Ӵ���֮��ĸ�����
}

Matrix<double> CDib::BilateralFilter(int diameter, double sigmaDistance, double sigmaIntensity)
{
	Matrix<double> pDibBits2D = Tovector();
	Matrix<double> pDibBits2D_Filter = ::BilateralFilter(pDibBits2D, diameter, sigmaDistance, sigmaIntensity);
	Read(pDibBits2D_Filter);
	return pDibBits2D_Filter;
}


// Color Image Processing

Matrix<double> CDib::GetHSIChannel(HSIChannel channel)
{
	Matrix<double> Data = Tovector();
	Matrix<double> Data_HSI = ::RGB2HSI(Data);
	Matrix<double> Data_Channel(m_nHeight, vector<double>(m_nWidth, 0));
	if (channel == HSIChannel::HUE) {
		for (int i = 0; i < m_nHeight; i++) {
			for (int j = 0; j < m_nWidth; j++) {
				Data_Channel[i][j] = Data_HSI[i][j * 3 + 2];
			}
		}
	}
	else if (channel == HSIChannel::SATURATION) {
		for (int i = 0; i < m_nHeight; i++) {
			for (int j = 0; j < m_nWidth; j++) {
				Data_Channel[i][j] = Data_HSI[i][j * 3 + 1];
			}
		}
	}
	else if (channel == HSIChannel::INTENSITY) {
		for (int i = 0; i < m_nHeight; i++) {
			for (int j = 0; j < m_nWidth; j++) {
				Data_Channel[i][j] = Data_HSI[i][j * 3 + 0];
			}
		}
	}
	return Data_Channel;
}

void CDib::ShowHSIChannel(HSIChannel channel)
{
	Matrix<double> Data_Channel = GetHSIChannel(channel);
	// ����ά����д��ͼ��ÿ�����ص�RGBΪData_Channel[i][j]
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 0) = (unsigned char)(Data_Channel[i][j] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = (unsigned char)(Data_Channel[i][j] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = (unsigned char)(Data_Channel[i][j] + 0.5);
		}
	}
}

void CDib::RGB2HSI()
{
	Matrix<double> Data = Tovector();
	Matrix<double> Data_HSI = ::RGB2HSI(Data);
	// ����ά����д��ͼ��ÿ�����ص�RGBΪData_HSI[i][j * 3], Data_HSI[i][j * 3 + 1], Data_HSI[i][j * 3 + 2]
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 0) = (unsigned char)(Data_HSI[i][j * 3 + 0] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = (unsigned char)(Data_HSI[i][j * 3 + 1] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = (unsigned char)(Data_HSI[i][j * 3 + 2] + 0.5);
		}
	}
}

void CDib::HSI2RGB()
{
	Matrix<double> Data = Tovector();
	Matrix<double> Data_RGB = ::HSI2RGB(Data);
	// ����ά����д��ͼ��ÿ�����ص�RGBΪData_RGB[i][j * 3], Data_RGB[i][j * 3 + 1], Data_RGB[i][j * 3 + 2]
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j * 3) = (unsigned char)(Data_RGB[i][j * 3] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = (unsigned char)(Data_RGB[i][j * 3 + 1] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = (unsigned char)(Data_RGB[i][j * 3 + 2] + 0.5);
		}
	}
}

void CDib::HSIHistogramEqualization()
{
	Matrix<double> Data = Tovector();
	Matrix<double> Data_HSI = ::RGB2HSI(Data);
	Matrix<double> Data_HSI_Equalization = ::HSIHistogramEqualization(Data_HSI);

	Matrix<double> Data_RGB = ::HSI2RGB(Data_HSI_Equalization);
	// ����ά����д��ͼ��ÿ�����ص�RGBΪData_RGB[i][j * 3], Data_RGB[i][j * 3 + 1], Data_RGB[i][j * 3 + 2]
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j * 3) = (unsigned char)(Data_RGB[i][j * 3 + 0] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 1) = (unsigned char)(Data_RGB[i][j * 3 + 1] + 0.5);
			*(m_pDibBits + i * m_nWidthBytes + j * 3 + 2) = (unsigned char)(Data_RGB[i][j * 3 + 2] + 0.5);
		}
	}
}

// some functions

long* CDib::GrayValueCount() {
	long nColors = GetMaxColorTableEntries();
	if (nColors == 0) {
		return NULL;
	}
	long* pGrayValueCount = new long[nColors];
	memset(pGrayValueCount, 0, nColors * sizeof(long));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			pGrayValueCount[*(m_pDibBits + i * m_nWidthBytes + j)]++;
		}
	}
	return pGrayValueCount;
}

Matrix<double> CDib::Padding(int a, int b) {
	Matrix<double> pDibBits2D(m_nHeight + a * 2,
		vector<double>(m_nWidthBytes + b * 2, 0));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			pDibBits2D[i + a][j + b] = *(m_pDibBits + i * m_nWidthBytes + j);
		}
	}
	return pDibBits2D;
}

Matrix<double> CDib::Conv2d(Matrix<double> const& kernel,
	int stride, int h_padding, int w_padding) {
	Matrix<double> temp = Padding(h_padding, w_padding);
	Matrix<double> pDibBits2D(m_nHeight,
		vector<double>(m_nWidthBytes, 0));
	for (int i = 0; i < m_nHeight; i += stride) {
		for (int j = 0; j < m_nWidthBytes; j += stride) {
			double nSum = 0;
			for (int k = 0; k < kernel.size(); k++) {
				for (int l = 0; l < kernel[0].size(); l++) {
					nSum += temp[i + k][j + l] * kernel[k][l];
				}
			}
			pDibBits2D[i][j] = nSum;
		}
	}
	return pDibBits2D;
}

Matrix<double> CDib::Tovector()
{
	if (m_pDibBits == nullptr) {
		return Matrix<double>();
	}
	Matrix<double> Data(m_nHeight, vector<double>(m_nWidthBytes, 0));
	if (m_nBitCount == 8) {
		for (int i = 0; i < m_nHeight; i++) {
			for (int j = 0; j < m_nWidthBytes; j++) {
				Data[i][j] = *(m_pDibBits + i * m_nWidthBytes + j);
			}
		}
		return Data;
	}
	else if (m_nBitCount == 24) {
		for (int i = 0; i < m_nHeight; i++) {
			for (int j = 0; j < m_nWidthBytes; j += 3) {
				Data[i][j] = *(m_pDibBits + i * m_nWidthBytes + j);
				Data[i][j + 1] = *(m_pDibBits + i * m_nWidthBytes + j + 1);
				Data[i][j + 2] = *(m_pDibBits + i * m_nWidthBytes + j + 2);
			}
		}
		return Data;
	}
}

ImageSet CDib::TovectorMatrix()
{
	// ��m_pRawBuffers��ȡ��vector<Matrix<double>>��
	if (m_pDibBits == nullptr)
	{
		return ImageSet();
	}
	ImageSet matrix3d;
	for (auto& p : m_pRawBuffers) {
		Matrix<double> matrix(m_nHeight, vector<double>(m_nWidth, 0));
		for (int i = 0; i < m_nHeight; i++) {
			for (int j = 0; j < m_nWidth; j++) {
				matrix[i][j] = p[i * m_nWidth + j];
			}
		}
		matrix3d.push_back(matrix);
	}
	return matrix3d;
}

void CDib::Read(vector<vector<unsigned char>> const& pDibBits2D) {
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j) = pDibBits2D[i][j];
		}
	}
}

void CDib::Read(Matrix<double> const& pDibBits2D) {
	if (m_nBitCount) {
		for (int i = 0; i < m_nHeight; i++) {
			for (int j = 0; j < m_nWidthBytes; j++) {
				double tmp = pDibBits2D[i][j];
				if (tmp < 0) {
					tmp = 0;
				}
				if (tmp > 255) {
					tmp = 255;
				}
				*(m_pDibBits + i * m_nWidthBytes + j) = (unsigned char)(tmp + 0.5);
			}
		}
	}
}

void CDib::Window_1(double level, double width) {
	if (m_pDibBits == NULL) {
		return;
	}
	int nColors = GetMaxColorTableEntries();
	if (nColors == 0) {
		return;
	}
	RGBQUAD* pColorTable = new RGBQUAD[nColors];
	GetColorTable(0, nColors, pColorTable);
	for (int i = 0; i < nColors; i++) {
		int gray = 255 * ((i - level) / width + 0.5);
		if (gray < 0) {
			gray = 0;
		}
		if (gray > 255) {
			gray = 255;
		}
		pColorTable[i].rgbRed = pColorTable[i].rgbGreen = pColorTable[i].rgbBlue =
			gray;
	}
	SetColorTable(0, nColors, pColorTable);
	delete[] pColorTable;
}

double CDib::PSNR(Matrix<double> const& OriImage)
{
	// ����PSNR
	double sum = 0;
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			sum += pow(OriImage[i][j] - m_pDibBits[i * m_nWidth + j], 2);
		}
	}
	double mse = sum / (m_nHeight * m_nWidth);
	double psnr = 10 * log10(255 * 255 / mse);
	return psnr;
}


// FFT

BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight) {
	unsigned char* lpSrc;      // ָ��Դͼ���ָ��
	int y;                     // ѭ�����Ʊ���
	int x;                     // ѭ�����Ʊ���
	double dTmpOne;            // �����ʱ����
	double dTmpTwo;            // �����ʱ����
	int nTransWidth;           // ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;          // ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;          // ����ͼ������ػҶȵ���ʱ����
	complex<double>* pCTData;  // ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;  // ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;  // ����Ҷ�任��ʵ��
	double dImag;  // ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];  // �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];  // �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);  // ����
		}
	}
	// ��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++) {
		for (x = 0; x < nWidth; x++) {
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);  // ����Ҷ���任

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth);  // ����Ҷ���任

	for (y = 0; y < nHeight; y++)  // ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++) {
			// ��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y * nTransWidth + x].real();  // ʵ��
			dImag = pCTData[y * nTransWidth + x].imag();  // �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0) {
				unchValue = 0;
			}
			if (unchValue > 0xff) {
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[] pCTData;  // �ͷ��ڴ�
	delete[] pCFData;  // �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	return (true);  // ���ؽ��
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ�任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ����任������
 *   nLevel
 *������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ�任��
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double>* pCTData, complex<double>* pCFData,
	int nLevel) {
	// ѭ�����Ʊ���
	int i;
	int j;
	int k;

	// ����Ҷ�任����
	int nCount = 0;

	// ���㸵��Ҷ�任����
	nCount = (int)pow(2.0, nLevel);

	// ĳһ���ĳ���
	int nBtFlyLen;
	nBtFlyLen = 0;

	// �任ϵ���ĽǶ� ��2 * PI * i / nCount
	double dAngle;

	complex<double>* pCW;

	// �����ڴ棬�洢����Ҷ�仯��Ҫ��ϵ����
	pCW = new complex<double>[nCount / 2];

	// ���㸵��Ҷ�任��ϵ��
	for (i = 0; i < nCount / 2; i++) {
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double>(cos(dAngle), sin(dAngle));
	}

	// �任��Ҫ�Ĺ����ռ�
	complex<double>* pCWork1, * pCWork2;

	// ���乤���ռ�
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	// ��ʱ����
	complex<double>* pCTmp;

	// ��ʼ����д������
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// ��ʱ����
	int nInter;
	nInter = 0;

	// �����㷨���п��ٸ���Ҷ�任
	for (k = 0; k < nLevel; k++) {
		for (j = 0; j < (int)pow(2.0, k); j++) {
			// ���㳤��
			nBtFlyLen = (int)pow(2.0, (nLevel - k));

			// �������ţ���Ȩ����
			for (i = 0; i < nBtFlyLen / 2; i++) {
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] =
					pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] =
					(pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2]) *
					pCW[(int)(i * pow(2.0, k))];
			}
		}

		// ���� pCWork1��pCWork2������
		pCTmp = pCWork1;
		pCWork1 = pCWork2;
		pCWork2 = pCTmp;
	}

	// ��������
	for (j = 0; j < nCount; j++) {
		nInter = 0;
		for (i = 0; i < nLevel; i++) {
			if (j & (1 << i)) {
				nInter += 1 << (nLevel - i - 1);
			}
		}
		pCFData[j] = pCWork1[nInter];
	}

	// �ͷ��ڴ�ռ�
	delete[] pCW;
	delete[] pCWork1;
	delete[] pCWork2;
	pCW = NULL;
	pCWork1 = NULL;
	pCWork2 = NULL;
}

/*************************************************************************
 *
 * \�������ƣ�
 *    IFFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ���任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ������任������
 *   nLevel
 *������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double>* pCFData, complex<double>* pCTData,
	int nLevel) {
	// ѭ�����Ʊ���
	int i;

	// ����Ҷ���任����
	int nCount;

	// ���㸵��Ҷ�任����
	nCount = (int)pow(2.0, nLevel);

	// �任��Ҫ�Ĺ����ռ�
	complex<double>* pCWork;

	// ���乤���ռ�
	pCWork = new complex<double>[nCount];

	// ����Ҫ���任������д�빤���ռ�pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);

	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for (i = 0; i < nCount; i++) {
		pCWork[i] = complex<double>(pCWork[i].real(), -pCWork[i].imag());
	}

	// ���ÿ��ٸ���Ҷ�任ʵ�ַ��任������洢��pCTData��
	FFT_1D(pCWork, pCTData, nLevel);

	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��nCount
	for (i = 0; i < nCount; i++) {
		pCTData[i] = complex<double>(pCTData[i].real() / nCount,
			-pCTData[i].imag() / nCount);
	}

	// �ͷ��ڴ�
	delete[] pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_2D()
 *
 * \�������:
 *   complex<double> * pCTData	- ͼ������
 *   int    nWidth				- ���ݿ��
 *   int    nHeight				- ���ݸ߶�
 *   complex<double> * pCFData	- ����Ҷ�任��Ľ��
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ�任��
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double>* pCTData, int nWidth, int nHeight,
	complex<double>* pCFData) {
	// ѭ�����Ʊ���
	int x;
	int y;

	// ��ʱ����
	double dTmpOne;
	double dTmpTwo;

	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int nTransWidth;
	int nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;

	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	// x��y�����У������ϵĵ�������
	int nXLev;
	int nYLev;

	// ����x��y�����У������ϵĵ�������
	nXLev = (int)(log(1.0 * nTransWidth) / log(2.0) + 0.5);
	nYLev = (int)(log(1.0 * nTransHeight) / log(2.0) + 0.5);

	for (y = 0; y < nTransHeight; y++) {
		// x������п��ٸ���Ҷ�任
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}

	// pCFData��Ŀǰ�洢��pCTData�����б任�Ľ��
	// Ϊ��ֱ������FFT_1D����Ҫ��pCFData�Ķ�ά����ת�ã���һ������FFT_1D����
	// ����Ҷ�б任��ʵ�����൱�ڶ��н��и���Ҷ�任��
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}

	for (x = 0; x < nTransWidth; x++) {
		// ��x������п��ٸ���Ҷ�任��ʵ�����൱�ڶ�ԭ����ͼ�����ݽ����з����
		// ����Ҷ�任
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData��Ŀǰ�洢��pCTData������ά����Ҷ�任�Ľ��������Ϊ�˷����з���
	// �ĸ���Ҷ�任�����������ת�ã����ڰѽ��ת�û���
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTData[nTransWidth * y + x] = pCFData[nTransHeight * x + y];
		}
	}

	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCFData[nTransWidth * y + x] = pCTData[nTransHeight * y + x];
		}
	}
}

/*************************************************************************
 *
 * \�������ƣ�
 *   IFFT_2D()
 *
 * \�������:
 *   complex<double> * pCFData	- Ƶ������
 *   complex<double> * pCTData	- ʱ������
 *   int    nWidth				- ͼ�����ݿ��
 *   int    nHeight				- ͼ�����ݸ߶�
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double>* pCFData, complex<double>* pCTData,
	int nWidth, int nHeight) {
	// ѭ�����Ʊ���
	int x;
	int y;

	// ��ʱ����
	double dTmpOne;
	double dTmpTwo;

	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int nTransWidth;
	int nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;

	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	// ���乤����Ҫ���ڴ�ռ�
	complex<double>* pCWork = new complex<double>[nTransWidth * nTransHeight];

	// ��ʱ����
	complex<double>* pCTmp;

	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTmp = &pCFData[nTransWidth * y + x];
			pCWork[nTransWidth * y + x] =
				complex<double>(pCTmp->real(), -pCTmp->imag());
		}
	}

	// ���ø���Ҷ���任
	FFT_2D(pCWork, nWidth, nHeight, pCTData);

	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTmp = &pCTData[nTransWidth * y + x];
			pCTData[nTransWidth * y + x] =
				complex<double>(pCTmp->real() / (nTransWidth * nTransHeight),
					-pCTmp->imag() / (nTransWidth * nTransHeight));
		}
	}
	delete[] pCWork;
	pCWork = NULL;
}
