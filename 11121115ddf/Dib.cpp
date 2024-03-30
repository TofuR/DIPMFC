// clang-format off
#include "pch.h"
// clang-format on

#include "Dib.h"

#include <fstream>
#include <vector>
#include "functions.h"


CDib::CDib(void)
	: m_pDibBits(NULL), m_pGrayValueCount(NULL), m_pRawBuffer(NULL) {
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;
}

CDib::CDib(CDib& Dib)
	: m_pDibBits(NULL), m_pGrayValueCount(NULL), m_pRawBuffer(NULL) {
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;

	if (&Dib == NULL) {
		return;
	}
	if (!IsNull()) {
		Destroy();
	}
	Create(Dib.GetWidth(), Dib.GetHeight(), Dib.GetBPP(), 0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
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

	if (m_pRawBuffer != NULL) {
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	if (m_pGrayValueCount != NULL) {
		delete[] m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}
}

void CDib::LoadFile(LPCTSTR lpszPathName) {
	// 检查文件名
	CString strPathName(lpszPathName);
	int nDotIndex = strPathName.ReverseFind(_T('.'));
	if (nDotIndex == -1) {
		// 文件名没有找到"."
		AfxMessageBox(_T("不支持的文件格式"));
		return;
	}

	CString strExtension =
		strPathName.Mid(nDotIndex + 1);  // 取出文件名"."以后的内容
	strExtension.MakeLower();            // 将扩展名转换为小写
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

void CDib::LoadRawFile(LPCTSTR lpszPathName) {
	m_nWidth = 512;
	m_nHeight = 512;

	// 创建暂存文件像素值的缓冲区//16位，保存raw
	if (m_pRawBuffer != NULL) {
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	m_pRawBuffer = new uint16_t[m_nWidth * m_nHeight];
	memset(m_pRawBuffer, 0, m_nWidth * m_nHeight * sizeof(uint16_t));

	// 使用fistream读取raw文件的像素值进buffer
	std::ifstream file(lpszPathName, std::ios::binary);
	file.read(reinterpret_cast<char*>(m_pRawBuffer),
		m_nWidth * m_nHeight * sizeof(uint16_t));

	// 文件打开失败弹出警告，然后退出
	if (file.fail()) {
		AfxMessageBox(_T("读取文件失败"));
		return;
	}
	file.close();
	// 因为读取进来的像素高8位和低8位顺序是反的，这里对像素高低8位进行对调，同时获取最大值
	for (int i = 0; i < m_nWidth * m_nHeight; i++) {
		m_pRawBuffer[i] =
			((m_pRawBuffer[i] & 0xff00) >> 8) | ((m_pRawBuffer[i] & 0x00ff) << 8);
	}
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
	// 将buffer中的像素值写入Dib
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

void CDib::Stretching() {
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			double tmp = *(m_pDibBits + i * m_nWidthBytes + j);
			if (tmp <= 100)
				*(m_pDibBits + i * m_nWidthBytes + j) =
				(0.5 * tmp) + 0.5;  // 加0.5是为了四舍五入
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
	vector<vector<double>> Smoth_Kernel =
		vector<vector<double>>(3, vector<double>(3, 1.0 / 9));
	vector<vector<double>> pDibBits2D = Conv2d(Smoth_Kernel, 1, 1, 1);
	Read_from_vector(pDibBits2D);
}

void CDib::Sobel() {
	vector<vector<double>> Sobel_Kernel_X = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
	vector<vector<double>> Sobel_Kernel_Y = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
	vector<vector<double>> pDibBits2D_X = Conv2d(Sobel_Kernel_X, 1, 1, 1);
	vector<vector<double>> pDibBits2D_Y = Conv2d(Sobel_Kernel_Y, 1, 1, 1);
	vector<vector<double>> pDibBits2D(m_nHeight,
		vector<double>(m_nWidthBytes, 0));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			pDibBits2D[i][j] = sqrt(pDibBits2D_X[i][j] * pDibBits2D_X[i][j] +
				pDibBits2D_Y[i][j] * pDibBits2D_Y[i][j]);
		}
	}
	Read_from_vector(pDibBits2D);
}

void CDib::Laplace() {
	vector<vector<double>> Laplace_Kernel =
		vector<vector<double>>(3, vector<double>(3, -1));
	Laplace_Kernel[1][1] = 8;
	vector<vector<double>> pDibBits2D = Conv2d(Laplace_Kernel, 1, 1, 1);
	Read_from_vector(pDibBits2D);
}

void CDib::Gaussian1D(int kernel_size) {
	vector<vector<double>> Gaussian_Kernel_X =
		vector<vector<double>>(1, vector<double>(kernel_size, 0));
	vector<vector<double>> Gaussian_Kernel_Y =
		vector<vector<double>>(kernel_size, vector<double>(1, 0));
	double sigma = 3.0;
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
	vector<vector<double>> pDibBits2D_X =
		Conv2d(Gaussian_Kernel_X, 1, 0, kernel_size / 2);
	Read_from_vector(pDibBits2D_X);
	vector<vector<double>> pDibBits2D_Y =
		Conv2d(Gaussian_Kernel_Y, 1, kernel_size / 2, 0);
	Read_from_vector(pDibBits2D_Y);
}

void CDib::Gaussian2D(int kernel_size) {
	vector<vector<double>> Gaussian_Kernel =
		vector<vector<double>>(kernel_size, vector<double>(kernel_size, 0));
	double sigma = 3.0;
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
	vector<vector<double>> pDibBits2D =
		Conv2d(Gaussian_Kernel, 1, kernel_size / 2, kernel_size / 2);
	Read_from_vector(pDibBits2D);
}

void CDib::BoneWindow(double midpoint, double width) {
	Window_1(midpoint, width);
}

void CDib::LungWindow(double midpoint, double width) {
	Window_1(midpoint, width);
}

void CDib::Amplitude()
{
	vector<vector<double>> pDibBits2D = Read_from_pDibBits();
	vector<vector<complex<double>>> CTData = Double2Complex(pDibBits2D);
	vector<vector<complex<double>>> CFData = ::FFT(CTData);
	// 计算幅度谱
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
	vector<vector<double>> pDibBits2D_Amplitude = Complex2Double(CFData);
	Read_from_vector(pDibBits2D_Amplitude);

	//unsigned char* pDIBBits = m_pDibBits;
	//long nWidth = m_nWidth;
	//long nHeight = m_nHeight;
	//unsigned char* lpSrc;							// 指向源图像的指针
	//int y;										// 循环控制变量
	//int x;										// 循环控制变量
	//double dTmpOne;								//存放临时变量
	//double dTmpTwo;								//存放临时变量
	//int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	//int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	//double unchValue;								// 存贮图像各像素灰度的临时变量
	//complex<double>* pCTData;						// 指向时域数据的指针
	//complex<double>* pCFData;						// 指向频域数据的指针
	//// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	//dTmpOne = log(1.0 * nWidth) / log(2.0);
	//dTmpTwo = ceil(dTmpOne);
	//dTmpTwo = pow(2, dTmpTwo);
	//nTransWidth = (int)dTmpTwo;
	//// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	//dTmpOne = log(1.0 * nHeight) / log(2.0);
	//dTmpTwo = ceil(dTmpOne);
	//dTmpTwo = pow(2, dTmpTwo);
	//nTransHeight = (int)dTmpTwo;
	//double dReal;									// 傅立叶变换的实部
	//double dImag;									// 傅立叶变换的虚部

	//pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	//pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	//// 图像数据的宽和高不一定是2的
	//// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	//for (y = 0; y < nTransHeight; y++)
	//{
	//	for (x = 0; x < nTransWidth; x++)
	//	{
	//		pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// 补零
	//	}
	//}
	////把图像数据传给pCTData
	//for (y = 0; y < nHeight; y++)
	//{
	//	for (x = 0; x < nWidth; x++)
	//	{
	//		// 指向DIB第y行，第x个象素的指针
	//		lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
	//		unchValue = (*lpSrc) * pow(-1.0, x + y);
	//		pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
	//	}
	//}
	//FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换

	//double max = pCFData[(nTransHeight / 2) * nTransWidth + (nTransWidth / 2)].real();
	//max = log(max + 1);
	//double r = 255 / max;
	//int ndHeight = (nTransHeight - nHeight) / 2;
	//int ndWidth = (nTransWidth - nWidth) / 2;
	//for (y = ndHeight; y < (nTransHeight - ndHeight); y++)
	//{
	//	for (x = ndWidth; x < (nTransWidth - ndWidth); x++)
	//	{
	//		//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
	//		double dTmp = abs(pCFData[y * nTransWidth + x]);
	//		double dTmp1 = log(dTmp + 1);
	//		dTmp1 = dTmp1 * r;
	//		// 指向DIB第y行，第x个象素的指针
	//		lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
	//		*lpSrc = (BYTE)dTmp1;
	//	}
	//}

	//delete[] pCTData;										// 释放内存
	//delete[] pCFData;										// 释放内存
	//pCTData = NULL;
	//pCFData = NULL;

}

void CDib::Phase()
{
	vector<vector<double>> pDibBits2D = Read_from_pDibBits();
	vector<vector<complex<double>>> CTData = Double2Complex(pDibBits2D);
	vector<vector<complex<double>>> CFData = ::FFT(CTData);
	// 计算图像的相位谱
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
	vector<vector<double>> pDibBits2D_Phase = Complex2Double(CFData);
	Read_from_vector(pDibBits2D_Phase);
}

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

vector<vector<double>> CDib::Padding(int a, int b) {
	vector<vector<double>> pDibBits2D(m_nHeight + a * 2,
		vector<double>(m_nWidthBytes + b * 2, 0));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			pDibBits2D[i + a][j + b] = *(m_pDibBits + i * m_nWidthBytes + j);
		}
	}
	return pDibBits2D;
}

vector<vector<double>> CDib::Conv2d(vector<vector<double>> const& kernel,
	int stride, int h_padding, int w_padding) {
	vector<vector<double>> temp = Padding(h_padding, w_padding);
	vector<vector<double>> pDibBits2D(m_nHeight,
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

vector<vector<double>> CDib::Read_from_pDibBits()
{
	vector<vector<double>> pDibBits2D(m_nHeight, vector<double>(m_nWidthBytes, 0));
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			pDibBits2D[i][j] = *(m_pDibBits + i * m_nWidthBytes + j);
		}
	}
	return pDibBits2D;
}

void CDib::Read_from_vector(vector<vector<unsigned char>> const& pDibBits2D) {
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			*(m_pDibBits + i * m_nWidthBytes + j) = pDibBits2D[i][j];
		}
	}
}

void CDib::Read_from_vector(vector<vector<double>> const& pDibBits2D) {
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

BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight) {
	unsigned char* lpSrc;      // 指向源图像的指针
	int y;                     // 循环控制变量
	int x;                     // 循环控制变量
	double dTmpOne;            // 存放临时变量
	double dTmpTwo;            // 存放临时变量
	int nTransWidth;           // 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;          // 傅立叶变换的高度（2的整数次幂）
	double unchValue;          // 存贮图像各像素灰度的临时变量
	complex<double>* pCTData;  // 指向时域数据的指针
	complex<double>* pCFData;  // 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;  // 傅立叶变换的实部
	double dImag;  // 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];  // 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];  // 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);  // 补零
		}
	}
	// 把图像数据传给pCTData
	for (y = 0; y < nHeight; y++) {
		for (x = 0; x < nWidth; x++) {
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);  // 傅立叶正变换

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth);  // 傅立叶反变换

	for (y = 0; y < nHeight; y++)  // 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < nWidth; x++) {
			// 需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y * nTransWidth + x].real();  // 实部
			dImag = pCTData[y * nTransWidth + x].imag();  // 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0) {
				unchValue = 0;
			}
			if (unchValue > 0xff) {
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete[] pCTData;  // 释放内存
	delete[] pCFData;  // 释放内存
	pCTData = NULL;
	pCFData = NULL;
	return (true);  // 返回结果
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过变换的数据
 *   nLevel
 *－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶变换。
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double>* pCTData, complex<double>* pCFData,
	int nLevel) {
	// 循环控制变量
	int i;
	int j;
	int k;

	// 傅立叶变换点数
	int nCount = 0;

	// 计算傅立叶变换点数
	nCount = (int)pow(2.0, nLevel);

	// 某一级的长度
	int nBtFlyLen;
	nBtFlyLen = 0;

	// 变换系数的角度 ＝2 * PI * i / nCount
	double dAngle;

	complex<double>* pCW;

	// 分配内存，存储傅立叶变化需要的系数表
	pCW = new complex<double>[nCount / 2];

	// 计算傅立叶变换的系数
	for (i = 0; i < nCount / 2; i++) {
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double>(cos(dAngle), sin(dAngle));
	}

	// 变换需要的工作空间
	complex<double>* pCWork1, * pCWork2;

	// 分配工作空间
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	// 临时变量
	complex<double>* pCTmp;

	// 初始化，写入数据
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// 临时变量
	int nInter;
	nInter = 0;

	// 蝶形算法进行快速傅立叶变换
	for (k = 0; k < nLevel; k++) {
		for (j = 0; j < (int)pow(2.0, k); j++) {
			// 计算长度
			nBtFlyLen = (int)pow(2.0, (nLevel - k));

			// 倒序重排，加权计算
			for (i = 0; i < nBtFlyLen / 2; i++) {
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] =
					pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] =
					(pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2]) *
					pCW[(int)(i * pow(2.0, k))];
			}
		}

		// 交换 pCWork1和pCWork2的数据
		pCTmp = pCWork1;
		pCWork1 = pCWork2;
		pCWork2 = pCTmp;
	}

	// 重新排序
	for (j = 0; j < nCount; j++) {
		nInter = 0;
		for (i = 0; i < nLevel; i++) {
			if (j & (1 << i)) {
				nInter += 1 << (nLevel - i - 1);
			}
		}
		pCFData[j] = pCWork1[nInter];
	}

	// 释放内存空间
	delete[] pCW;
	delete[] pCWork1;
	delete[] pCWork2;
	pCW = NULL;
	pCWork1 = NULL;
	pCWork2 = NULL;
}

/*************************************************************************
 *
 * \函数名称：
 *    IFFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要反变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过反变换的数据
 *   nLevel
 *－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double>* pCFData, complex<double>* pCTData,
	int nLevel) {
	// 循环控制变量
	int i;

	// 傅立叶反变换点数
	int nCount;

	// 计算傅立叶变换点数
	nCount = (int)pow(2.0, nLevel);

	// 变换需要的工作空间
	complex<double>* pCWork;

	// 分配工作空间
	pCWork = new complex<double>[nCount];

	// 将需要反变换的数据写入工作空间pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);

	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for (i = 0; i < nCount; i++) {
		pCWork[i] = complex<double>(pCWork[i].real(), -pCWork[i].imag());
	}

	// 调用快速傅立叶变换实现反变换，结果存储在pCTData中
	FFT_1D(pCWork, pCTData, nLevel);

	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数nCount
	for (i = 0; i < nCount; i++) {
		pCTData[i] = complex<double>(pCTData[i].real() / nCount,
			-pCTData[i].imag() / nCount);
	}

	// 释放内存
	delete[] pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 图像数据
 *   int    nWidth				- 数据宽度
 *   int    nHeight				- 数据高度
 *   complex<double> * pCFData	- 傅立叶变换后的结果
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶变换。
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double>* pCTData, int nWidth, int nHeight,
	complex<double>* pCFData) {
	// 循环控制变量
	int x;
	int y;

	// 临时变量
	double dTmpOne;
	double dTmpTwo;

	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int nTransWidth;
	int nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;

	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	// x，y（行列）方向上的迭代次数
	int nXLev;
	int nYLev;

	// 计算x，y（行列）方向上的迭代次数
	nXLev = (int)(log(1.0 * nTransWidth) / log(2.0) + 0.5);
	nYLev = (int)(log(1.0 * nTransHeight) / log(2.0) + 0.5);

	for (y = 0; y < nTransHeight; y++) {
		// x方向进行快速傅立叶变换
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}

	// pCFData中目前存储了pCTData经过行变换的结果
	// 为了直接利用FFT_1D，需要把pCFData的二维数据转置，再一次利用FFT_1D进行
	// 傅立叶行变换（实际上相当于对列进行傅立叶变换）
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}

	for (x = 0; x < nTransWidth; x++) {
		// 对x方向进行快速傅立叶变换，实际上相当于对原来的图象数据进行列方向的
		// 傅立叶变换
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData中目前存储了pCTData经过二维傅立叶变换的结果，但是为了方便列方向
	// 的傅立叶变换，对其进行了转置，现在把结果转置回来
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
 * \函数名称：
 *   IFFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCFData	- 频域数据
 *   complex<double> * pCTData	- 时域数据
 *   int    nWidth				- 图像数据宽度
 *   int    nHeight				- 图像数据高度
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double>* pCFData, complex<double>* pCTData,
	int nWidth, int nHeight) {
	// 循环控制变量
	int x;
	int y;

	// 临时变量
	double dTmpOne;
	double dTmpTwo;

	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int nTransWidth;
	int nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0 * nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;

	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0 * nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	// 分配工作需要的内存空间
	complex<double>* pCWork = new complex<double>[nTransWidth * nTransHeight];

	// 临时变量
	complex<double>* pCTmp;

	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for (y = 0; y < nTransHeight; y++) {
		for (x = 0; x < nTransWidth; x++) {
			pCTmp = &pCFData[nTransWidth * y + x];
			pCWork[nTransWidth * y + x] =
				complex<double>(pCTmp->real(), -pCTmp->imag());
		}
	}

	// 调用傅立叶正变换
	FFT_2D(pCWork, nWidth, nHeight, pCTData);

	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数
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
