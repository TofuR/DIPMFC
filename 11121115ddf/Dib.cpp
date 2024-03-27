#include "pch.h"
#include "Dib.h"
#include <vector>
#include <fstream>


CDib::CDib(void) :m_pDibBits(NULL), m_pGrayValueCount(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;

}

CDib::CDib(CDib& Dib) :m_pDibBits(NULL), m_pGrayValueCount(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;

	if (&Dib == NULL)
	{
		return;
	}
	if (!IsNull())
	{
		Destroy();
	}
	Create(Dib.GetWidth(), Dib.GetHeight(), Dib.GetBPP(), 0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
	if (IsIndexed())
	{
		int nColors = Dib.GetMaxColorTableEntries();
		if (nColors > 0)
		{
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

CDib::~CDib(void)
{
	m_pDibBits = NULL;
	if (m_pGrayValueCount != NULL)
	{
		delete[]m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}
}

void CDib::LoadFile(LPCTSTR lpszPathName)
{
	Load(lpszPathName);
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();
}

void CDib::LoadRawFile(LPCTSTR lpszPathName, int nWidth, int nHeight, int nBitCount)
{
	if (nWidth <= 0 || nHeight <= 0 || nBitCount <= 0)
	{
		return;
	}

	std::vector<uint16_t> buffer(nWidth * nHeight);
	std::ifstream file(lpszPathName, std::ios::binary);
	file.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(uint16_t));
	Create(nWidth, nHeight, 8);
	HBITMAP hBitmap = (HBITMAP)Detach();
	RGBQUAD palette[256];
	for (int i = 0; i < 256; ++i)
	{
		palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = palette[i].rgbReserved = (BYTE)i;
	}
	HDC hDC = ::GetDC(NULL);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	::ReleaseDC(NULL, hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	SetDIBColorTable(hMemDC, 0, 256, palette);
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	Attach(hBitmap);

	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();

	uint16_t max = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		buffer[i] = ((buffer[i] & 0xff00) >> 8) | ((buffer[i] & 0x00ff) << 8);

		if (buffer[i] > max)
		{
			max = buffer[i];
		}
	}
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			uint16_t temp = buffer[i * m_nWidth + j];
			uint8_t gray = static_cast<uint8_t>(temp * 255.0 / max);
			int invertedY = m_nWidth - 1 - i;
			*(m_pDibBits + invertedY * m_nWidthBytes + j) = gray;
		}
	}
}

void CDib::Stretching()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			double tmp = *(m_pDibBits + i * m_nWidthBytes + j);
			if (tmp <= 100)
				*(m_pDibBits + i * m_nWidthBytes + j) = (0.5 * tmp) + 0.5;	// ��0.5��Ϊ����������
			else if (tmp > 100 && tmp < 150)
				*(m_pDibBits + i * m_nWidthBytes + j) = (3 * tmp - 250) + 0.5;
			else
				*(m_pDibBits + i * m_nWidthBytes + j) = (11.0 * tmp / 21.0 + 2550.0 / 21.0) + 0.5;
		}
	}
}

void CDib::Invert()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = 255 - *(m_pDibBits + i * m_nWidthBytes + j);
		}
	}
}

void CDib::Invert_Palette()
{
	if (IsIndexed())
	{
		int nColors = this->GetMaxColorTableEntries();
		if (nColors > 0)
		{
			RGBQUAD* pal{};
			pal = new RGBQUAD[nColors];
			this->GetColorTable(0, nColors, pal);

			for (int i = 0; i < nColors; ++i)
			{
				pal[i].rgbRed = 255 - pal[i].rgbRed;
				pal[i].rgbGreen = 255 - pal[i].rgbGreen;
				pal[i].rgbBlue = 255 - pal[i].rgbBlue;
			}

			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
}

void CDib::BitPlane_Slicing(int a)
{
	if (a < 0 || a > 7)
	{
		return;
	}

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (*(m_pDibBits + i * m_nWidthBytes + j) >> a & 1) * 255;

		}
	}
}

void CDib::Histogram_Equalization()
{
	long nColors = GetMaxColorTableEntries();
	if (nColors == 0)
	{
		return;
	}
	long* pGrayValueCount = GrayValueCount();
	long* pGrayValueCountSum = new long[nColors];
	memset(pGrayValueCountSum, 0, nColors * sizeof(long));
	pGrayValueCountSum[0] = pGrayValueCount[0];
	for (int i = 1; i < nColors; i++)
	{
		pGrayValueCountSum[i] = pGrayValueCountSum[i - 1] + pGrayValueCount[i];
	}
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (unsigned char)(255 * pGrayValueCountSum[*(m_pDibBits + i * m_nWidthBytes + j)] / (m_nHeight * m_nWidth) + 0.5);
		}
	}
	delete[] pGrayValueCount;
	delete[] pGrayValueCountSum;
}

void CDib::Smoothing()
{
	vector<vector<double>> Smoth_Kernel = vector<vector<double>>(3, vector<double>(3, 1.0 / 9));
	vector<vector<double>> pDibBits2D = Conv2d(Smoth_Kernel, 1, 1, 1);
	vector2uchar(pDibBits2D);
}

void CDib::Sobel()
{
	vector<vector<double>> Sobel_Kernel_X = { {1,2,1},{ 0,0,0 },{ -1,-2,-1 } };
	vector<vector<double>> Sobel_Kernel_Y = { {-1,0,1},{ -2,0,2 },{ -1,0,1 } };
	vector<vector<double>> pDibBits2D_X = Conv2d(Sobel_Kernel_X, 1, 1, 1);
	vector<vector<double>> pDibBits2D_Y = Conv2d(Sobel_Kernel_Y, 1, 1, 1);
	vector<vector<double>> pDibBits2D(m_nHeight, vector<double>(m_nWidthBytes, 0));
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			pDibBits2D[i][j] = sqrt(pDibBits2D_X[i][j] * pDibBits2D_X[i][j] + pDibBits2D_Y[i][j] * pDibBits2D_Y[i][j]);
		}
	}
	vector2uchar(pDibBits2D);
}

void CDib::Laplace()
{
	vector<vector<double>> Laplace_Kernel = vector<vector<double>>(3, vector<double>(3, -1));
	Laplace_Kernel[1][1] = 8;
	vector<vector<double>> pDibBits2D = Conv2d(Laplace_Kernel, 1, 1, 1);
	vector2uchar(pDibBits2D);
}

void CDib::Gaussian1D(int kernel_size)
{
	vector<vector<double>> Gaussian_Kernel_X = vector<vector<double>>(1, vector<double>(kernel_size, 0));
	vector<vector<double>> Gaussian_Kernel_Y = vector<vector<double>>(kernel_size, vector<double>(1, 0));
	double sigma = 3.0;
	double sum = 0.0;
	for (int i = 0; i < kernel_size; i++)
	{
		Gaussian_Kernel_X[0][i] = exp(-((i - kernel_size / 2) * (i - kernel_size / 2)) / (2 * sigma * sigma)) / (sqrt(2 * Pi) * sigma);
		Gaussian_Kernel_Y[i][0] = exp(-((i - kernel_size / 2) * (i - kernel_size / 2)) / (2 * sigma * sigma)) / (sqrt(2 * Pi) * sigma);
		sum += Gaussian_Kernel_X[0][i];
	}
	for (int i = 0; i < kernel_size; i++)
	{
		Gaussian_Kernel_X[0][i] /= sum;
		Gaussian_Kernel_Y[i][0] /= sum;
	}
	vector<vector<double>> pDibBits2D_X = Conv2d(Gaussian_Kernel_X, 1, 0, kernel_size / 2);
	vector2uchar(pDibBits2D_X);
	vector<vector<double>> pDibBits2D_Y = Conv2d(Gaussian_Kernel_Y, 1, kernel_size / 2, 0);
	vector2uchar(pDibBits2D_Y);
}

void CDib::Gaussian2D(int kernel_size)
{
	vector<vector<double>> Gaussian_Kernel = vector<vector<double>>(kernel_size, vector<double>(kernel_size, 0));
	double sigma = 3.0;
	double sum = 0.0;
	for (int i = 0; i < kernel_size; i++)
	{
		for (int j = 0; j < kernel_size; j++)
		{
			Gaussian_Kernel[i][j] = exp(-((i - kernel_size / 2) * (i - kernel_size / 2) + (j - kernel_size / 2) * (j - kernel_size / 2)) / (2 * sigma * sigma)) / (2 * Pi * sigma * sigma);
			sum += Gaussian_Kernel[i][j];
		}
	}
	for (int i = 0; i < kernel_size; i++)
	{
		for (int j = 0; j < kernel_size; j++)
		{
			Gaussian_Kernel[i][j] /= sum;
		}
	}
	vector<vector<double>> pDibBits2D = Conv2d(Gaussian_Kernel, 1, kernel_size / 2, kernel_size / 2);
	vector2uchar(pDibBits2D);
}

void CDib::BoneWindow(double midpoint, double width)
{
	Window_1(midpoint, width);
}

void CDib::LungWindow(double midpoint, double width)
{
	Window_1(midpoint, width);
}

long* CDib::GrayValueCount()
{
	long nColors = GetMaxColorTableEntries();
	if (nColors == 0)
	{
		return NULL;
	}
	long* pGrayValueCount = new long[nColors];
	memset(pGrayValueCount, 0, nColors * sizeof(long));
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			pGrayValueCount[*(m_pDibBits + i * m_nWidthBytes + j)]++;
		}
	}
	return pGrayValueCount;
}

vector<vector<double>> CDib::Padding(int a, int b)
{
	vector<vector<double>> pDibBits2D(m_nHeight + a * 2, vector<double>(m_nWidthBytes + b * 2, 0));
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			pDibBits2D[i + a][j + b] = *(m_pDibBits + i * m_nWidthBytes + j);
		}
	}
	return pDibBits2D;
}

vector<vector<double>> CDib::Conv2d(vector<vector<double>> const& kernel, int stride, int h_padding, int w_padding)
{
	vector<vector<double>> temp = Padding(h_padding, w_padding);
	vector<vector<double>> pDibBits2D(m_nHeight, vector<double>(m_nWidthBytes, 0));
	for (int i = 0; i < m_nHeight; i += stride)
	{
		for (int j = 0; j < m_nWidthBytes; j += stride)
		{
			double nSum = 0;
			for (int k = 0; k < kernel.size(); k++)
			{
				for (int l = 0; l < kernel[0].size(); l++)
				{
					nSum += temp[i + k][j + l] * kernel[k][l];
				}
			}
			pDibBits2D[i][j] = nSum;
		}
	}
	return pDibBits2D;
}

void CDib::vector2uchar(vector<vector<unsigned char>> const& pDibBits2D)
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = pDibBits2D[i][j];
		}
	}
}

void CDib::vector2uchar(vector<vector<double>> const& pDibBits2D)
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (unsigned char)(pDibBits2D[i][j] + 0.5);
		}
	}
}

void CDib::Window_1(double midpoint, double width)
{
	if (m_pDibBits == NULL)
	{
		return;
	}
	int nColors = GetMaxColorTableEntries();
	if (nColors == 0)
	{
		return;
	}
	RGBQUAD* pColorTable = new RGBQUAD[nColors];
	GetColorTable(0, nColors, pColorTable);
	for (int i = 0; i < nColors; i++)
	{
		int gray = 255 * ((i - midpoint) / width + 0.5);
		if (gray < 0)
		{
			gray = 0;
		}
		if (gray > 255)
		{
			gray = 255;
		}
		pColorTable[i].rgbRed = pColorTable[i].rgbGreen = pColorTable[i].rgbBlue = gray;
	}
	SetColorTable(0, nColors, pColorTable);
	delete[] pColorTable;
}


BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight)
{
	unsigned char* lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ��ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double>* pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double>* pCFData;						// ָ��Ƶ�����ݵ�ָ��
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
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ��͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y * nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc) * pow(-1.0, x + y);
			pCTData[y * nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y * nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y * nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete pCTData;										// �ͷ��ڴ�
	delete pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	return (true);										//���ؽ��
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ�任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ����任������
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ�任��
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double>* pCTData, complex<double>* pCFData, int nLevel)
{
	// ѭ�����Ʊ���
	int		i;
	int     j;
	int     k;

	// ����Ҷ�任����
	int	nCount = 0;

	// ���㸵��Ҷ�任����
	nCount = (int)pow(2.0, nLevel);

	// ĳһ���ĳ���
	int		nBtFlyLen;
	nBtFlyLen = 0;

	// �任ϵ���ĽǶ� ��2 * PI * i / nCount
	double	dAngle;

	complex<double>* pCW;

	// �����ڴ棬�洢����Ҷ�仯��Ҫ��ϵ����
	pCW = new complex<double>[nCount / 2];

	// ���㸵��Ҷ�任��ϵ��
	for (i = 0; i < nCount / 2; i++)
	{
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
	for (k = 0; k < nLevel; k++)
	{
		for (j = 0; j < (int)pow(2.0, k); j++)
		{
			//���㳤��
			nBtFlyLen = (int)pow(2.0, (nLevel - k));

			//�������ţ���Ȩ����
			for (i = 0; i < nBtFlyLen / 2; i++)
			{
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] =
					pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] =
					(pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2])
					* pCW[(int)(i * pow(2.0, k))];
			}
		}

		// ���� pCWork1��pCWork2������
		pCTmp = pCWork1;
		pCWork1 = pCWork2;
		pCWork2 = pCTmp;
	}

	// ��������
	for (j = 0; j < nCount; j++)
	{
		nInter = 0;
		for (i = 0; i < nLevel; i++)
		{
			if (j & (1 << i))
			{
				nInter += 1 << (nLevel - i - 1);
			}
		}
		pCFData[j] = pCWork1[nInter];
	}

	// �ͷ��ڴ�ռ�
	delete pCW;
	delete pCWork1;
	delete pCWork2;
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
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double>* pCFData, complex<double>* pCTData, int nLevel)
{

	// ѭ�����Ʊ���
	int		i;

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
	for (i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double>(pCWork[i].real(), -pCWork[i].imag());
	}

	// ���ÿ��ٸ���Ҷ�任ʵ�ַ��任������洢��pCTData��
	FFT_1D(pCWork, pCTData, nLevel);

	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�������������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��nCount
	for (i = 0; i < nCount; i++)
	{
		pCTData[i] =
			complex<double>(pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}

	// �ͷ��ڴ�
	delete pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_2D()
 *
 * \�������:
 *   complex<double> * pCTData	- ͼ������
 *   int    nWidth				- ���ݿ���
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
void CDib::FFT_2D(complex<double>* pCTData, int nWidth, int nHeight, complex<double>* pCFData)
{

	// ѭ�����Ʊ���
	int	x;
	int	y;

	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;

	// ���и���Ҷ�任�Ŀ��Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ��Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ���	��2���������ݣ�
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
	int		nXLev;
	int		nYLev;

	// ����x��y�����У������ϵĵ�������
	nXLev = (int)(log(1.0 * nTransWidth) / log(2.0) + 0.5);
	nYLev = (int)(log(1.0 * nTransHeight) / log(2.0) + 0.5);

	for (y = 0; y < nTransHeight; y++)
	{
		// x������п��ٸ���Ҷ�任
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}

	// pCFData��Ŀǰ�洢��pCTData�����б任�Ľ��
	// Ϊ��ֱ������FFT_1D����Ҫ��pCFData�Ķ�ά����ת�ã���һ������FFT_1D����
	// ����Ҷ�б任��ʵ�����൱�ڶ��н��и���Ҷ�任��
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}

	for (x = 0; x < nTransWidth; x++)
	{
		// ��x������п��ٸ���Ҷ�任��ʵ�����൱�ڶ�ԭ����ͼ�����ݽ����з����
		// ����Ҷ�任
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData��Ŀǰ�洢��pCTData������ά����Ҷ�任�Ľ��������Ϊ�˷����з���
	// �ĸ���Ҷ�任�����������ת�ã����ڰѽ��ת�û���
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransWidth * y + x] = pCFData[nTransHeight * x + y];
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
 *   int    nWidth				- ͼ�����ݿ���
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
void CDib::IFFT_2D(complex<double>* pCFData, complex<double>* pCTData, int nWidth, int nHeight)
{
	// ѭ�����Ʊ���
	int	x;
	int	y;

	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;

	// ���и���Ҷ�任�Ŀ��Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ��Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ���	��2���������ݣ�
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

	//��ʱ����
	complex<double>* pCTmp;

	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x];
			pCWork[nTransWidth * y + x] = complex<double>(pCTmp->real(), -pCTmp->imag());
		}
	}

	// ���ø���Ҷ���任
	FFT_2D(pCWork, nWidth, nHeight, pCTData);

	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�������������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCTData[nTransWidth * y + x];
			pCTData[nTransWidth * y + x] =
				complex<double>(pCTmp->real() / (nTransWidth * nTransHeight),
					-pCTmp->imag() / (nTransWidth * nTransHeight));
		}
	}
	delete pCWork;
	pCWork = NULL;
}