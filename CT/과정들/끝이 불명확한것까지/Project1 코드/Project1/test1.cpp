#define _USE_MATH_DEFINES
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "hh.h"

//===========================================================================
float arr[MAX_R][MAX_C];			//����raw���� ���� �迭, 8��Ʈ
float arr2[MAX_R][MAX_C];		//rotation�� raw���� ���� �迭, 8��Ʈ
float arr3[MAX_R][MAX_C];
float oneDSinogram[MAX_C];		//one Dimension Sinogram, 1���� sinogram�迭, 16��Ʈ
float ramLak[MAX_C];			//ram-Lak filter �� ����
float sinogramFilter[MAX_C];	//���;��� sinogram
float sinogramArr[360][MAX_C];	//sinogram �� ����, 16��Ʈ
float arr4[MAX_R][MAX_C];		//sinogram ������ ������ ������ 2���� �迭 (backProjection���� �̿�)
float arr5[MAX_R][MAX_C];		//Oritoation�� raw���� ���� �迭
float arr6[MAX_R][MAX_C];		// rotation�� ���� ������ų �迭
float min;
float max;


FILE *fp;//���� �ҷ���
FILE *testsinogram;						//sinogram ���
FILE *testsinogramfft;
FILE *testLast;							//���� ����(���� ����� ��) ���

//===========================================================================
//������ �ҷ����� �Լ�
void openFile(void)
{
	//fp = fopen("Head001.raw", "rb");	//8��Ʈ¥����, 0~255
	//fp = fopen("testCircle.raw", "rb");	//8��Ʈ¥����, 0~255
	//fp = fopen("testSquare.raw", "rb");	//8��Ʈ¥����, 0~255

	fp = fopen("Phantom.raw", "rb");	//8��Ʈ¥����, 0~255
	//fread(arr, sizeof(unsigned int), MAX_R*MAX_C, fp);
	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			arr[i][j] = fgetc(fp);
			//printf("%d ", arr[i][j]);	//��½���
		}
		//printf("\n");					//��½���
	}
	fclose(fp);

	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			arr3[i][j] = arr[i][j];
		}
	}
}

//===========================================================================
//===========================================================================
//rotation ��갪 ��ȯ�ϴ� �Լ� ************�߽��� �������� ������ ���� ���� �߽����� �̵�, �ٽ� ��ǥ ������� ������ֱ�
setLC calRotation(int i, int j,double t) {
	setLC n;
	n.rd = ((i - MAX_R / 2)*cos(t) + (j - MAX_C / 2)*sin(t)) + MAX_R / 2;	//�������� ����
	n.cd = ((-1)*(i - MAX_R / 2)*sin(t) + (j - MAX_C / 2)*cos(t)) + MAX_C / 2;
	n.r = (int)(n.rd);
	n.c = (int)(n.cd);
	//n.r = (int)(n.rd + 0.5);//0.5-->�ݿø� ���ؼ�
	//n.c = (int)(n.cd + 0.5);
	//printf("%d %d\n", n.r, n.c);
	return n; 
}

//===========================================================================
//rotation��Ű�� �Լ�
void rotation(double n)
{

	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			if (calRotation(i,j,n).r>=0&& calRotation(i, j, n).r<=511&& calRotation(i, j, n).c>=0&& calRotation(i, j, n).c<=511)	//index�� 0~511 ������ ����� ������
			{
				arr2[calRotation(i, j, n).r][calRotation(i, j, n).c] = arr[i][j];
				//printf("%d ", arr2[calRotation(i, j, n).r][calRotation(i, j, n).c]);
			}
			else
			{
				//printf("%d %d\n",calRotation(i,j,n).r,calRotation(i,j,n).c);
			}
		}
	}
//�迭����
for (int i = 0; i < MAX_R; i++)
{
	for (int j = 0; j < MAX_C; j++)
	{
		arr3[i][j] = arr2[i][j];
	}
}
}

//===========================================================================
//1���� sinogram���Ϸ� �����ϴ� �Լ�
void makeSinogram(void)	//**sinogram�� 16bits grayscale��
{
	//������ ��� ����
	for (int i = 0; i < MAX_C; i++)
	{
		for (int j = 0; j < MAX_R; j++)
		{
			oneDSinogram[i] += arr3[j][i];
		}
		oneDSinogram[i] /= 512;		//(pow(2,9))
		//printf("%d ",oneDSinogram[i]);
	}
	//printf("\n\n");
}

//===========================================================================
//2���� sinogram���� ����� �Լ�
void makeSinogramPlane(void)
{
	for (int i = 0; i < 360; i++)//180��(180���̹Ƿ�)�� �ݺ��ص� ������ �����ڷ�� ���غ��� ���ؼ� 360�� ����
	{
		//printf("\n\n\n%d\n\n", i);
		makeSinogram();

		//sinogram�� 1���� �迭 2���� sinogram�� ����
		for (int j = 0; j < MAX_C; j++)
		{
			sinogramArr[i][j] = (float)oneDSinogram[j];
			//printf("%f\n", (float)oneDSinogram[j]);
		}
		//rotation�ؼ� arr�� �ٲ�
		rotation((M_PI / 180 * i));

	}

	testsinogram = fopen("Phantomsinogram.raw", "wb");
	for (int r = 0; r < 360; r++)
	{
		for (int c = 0; c < MAX_C; c++)

		{
			fputc(sinogramArr[r][c], testsinogram);
		}
	}
	fclose(testsinogram);
}

//########################################################################################################################
//########################################################################################################################
//Inverse Radon-Transform 

//===========================================================================
//�����ϴ� �迭�� arr6�� �ʱ�ȭ��Ű�� �Լ�
void setZero1(void)
{
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			arr6[i][j] = 0;
		}
	}
}

//===========================================================================
//sinogram ���پ� ���� arr4�� �ʱ�ȭ��Ű�� �Լ�
void setZero2(void)
{
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			arr4[i][j] = 0;
		}
	}
}

//===========================================================================
//1���� FFT
/*
void FFT(double* g, unsigned long N, int isign)
{ 
	int mmax, istep;
	int m;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;
	double temp;

	//
	int n = N * 2;
	int j = 1;
	for (int i = 1; i < n; i += 2)
	{
		printf("j: %d gj %f gi: %f\n", j, g[j], g[i]);
		if (j > i)
		{
			temp = g[j];
			g[j] = g[i];
			g[i] = temp;

			temp = g[j + 1];
			g[j + 1] = g[i + 1];
			g[i + 1] = temp;
		}
		m = n >> 1;
		while (j > m&&m >= 2)
		{
			j -= m;
			m >>= 1;
			//m = m / 2;
		}
		j += m;
	}

	//
	mmax = 2;	//2���� DFT�� ����
	while (n > mmax)
	{
		istep = mmax << 1;
		theta = isign * (6.28318530717959 / mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2)
		{
			for (int i = m; i <= n; i += istep)
			{
				j = i + mmax;
				tempr = double(wr*g[j] - wi * g[j + 1]);
				tempi = double(wr*g[j + 1] + wi * g[j]);
				g[j] = g[i] - tempr;
				g[j + 1] = g[i + 1] - tempi;
				g[i] += tempr;
				g[i + 1] += tempi;
			}
			wtemp = wr;
			wr = wr*wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		mmax = istep;
	}
	if (isign == -1)
	{
		for (int i = 1; i <= n; i++)
		{
			g[i] /= N;
		}
	}
}

*/
//===========================================================================


//===========================================================================

//1���� DFT

void DFT(double* re, double* im, int N, int dir)
{
	double* tr = (double*)malloc(N * sizeof(double));
	double* ti = (double*)malloc(N * sizeof(double));

	memcpy(tr, re, sizeof(double) * N);
	memcpy(ti, im, sizeof(double) * N);
	
	//for (int i = 0; i < N; i++)
	//{
	//	//printf("%d:\n%f %f    %f %f\n\n", i,re[i],tr[i],im[i],ti[i]);
	//}



	register int i, x;
	double sum_re, sum_im, temp;
	for (i = 0; i < N; i++)
	{
		sum_re = sum_im = 0;
		for (x = 0; x < N; x++)
		{
			temp = 2 * dir * M_PI * ((double)i * x / N);
			sum_re += (tr[x] * cos(temp) + ti[x] * sin(temp));
			sum_im += (ti[x] * cos(temp) - tr[x] * sin(temp));
		}
		re[i] = sum_re;
		im[i] = sum_im;

	}


	if (dir == -1) // IDFT
	{
		for (i = 0; i < N; i++)
		{
			re[i] /= (double)N;
			im[i] /= (double)N;
		}
	}

	free(tr);
	free(ti);

}



//===========================================================================
//Ram-Lak Filter

void RamLakFilter(void)
{

	int x_max = 512;
	float y_max = (float)x_max / 2;

	
	//---�֢�__�� Ȯ���� �ƴϾ���!!

	//��_____�ֵ� �ƴѰͰ���
	//for (int i = 0; i < x_max; i++)
	//{
	//	if (i >= 0 && i <= x_max / 4)	//���� 0�ƴ� �κ�
	//	{
	//		ramLak[i] = x_max / 4 - i;
	//	}

	//	else if (i >= x_max - (x_max / 4) && i < MAX_C)	//������ 0�ƴѺκ�
	//	{
	//		ramLak[i] = i-(x_max/4*3);
	//	}

	//	else//0�� ����, ��� �κκ�
	//	{
	//		ramLak[i] = 0;
	//	}
	//}

	//for (int i = 0; i < 512; i++)
	//{
	//	printf("index %d:  %f\n", i, ramLak[i]);
	//}




	////DFT�϶� �֤�____�Ӣ�
	for (int i = 0; i < x_max; i++)
	{
		if (i >= 0 && i <= x_max / 4-2)	//���� 0�ƴ� �κ�
		{
			//ramLak[i] = (double)i-1;
			ramLak[i] = (double)(i);
		}

		else if (i >= x_max+2 - (x_max / 4) && i < MAX_C)	//������ 0�ƴѺκ�
		{
			ramLak[i] = (double)(511 - i)+1;
			//ramLak[i] = (double)(511 - i);

		}

		else if (i == 0)
		{
		//	ramLak[i] = 0;
		}

		else//0�� ����, ��� �κκ�
		{
			ramLak[i] = 0;
		}
	}

	for (int i = 0; i < 512; i++)
	{
		printf("index %d:  %f\n", i, ramLak[i]);
	}


	//	////DFT�϶� �֤�____�Ӣ�, ó��0, ������Ī
	//for (int i = 0; i < x_max; i++)
	//{
	//	if (i >= 1 && i <= x_max / 4)	//���� 0�ƴ� �κ�
	//	{
	//		//ramLak[i] = (double)i/2;
	//		ramLak[i] = i - 1;
	//	}

	//	else if (i >= x_max - (x_max / 4) && i < MAX_C)	//������ 0�ƴѺκ�
	//	{
	//		//ramLak[i] = (double)(511 - i)/2;
	//		ramLak[i] = (double)(511 - i);

	//	}

	//	else//0�� ����, ��� �κκ�
	//	{
	//		ramLak[i] = 0;
	//	}
	//}

	//for (int i = 0; i < 512; i++)
	//{
	//	printf("index %d:  %f\n", i, ramLak[i]);
	//}


//
//	//FFT�϶�__�Ӣ٢֤�__
//	for (int i = 0; i < x_max; i++)
//	{
//		if (i >= (x_max) / 4 && i < (x_max) / 2)	//���� 0�ƴ� �κ�
//		{
//			ramLak[i] = x_max / 2 - i;
//		}
//	
//		else if (i >= (x_max) / 2 && i <= (x_max - (x_max / 4)))	//������ 0�ƴѺκ�
//		{
//			ramLak[i] = i - x_max / 2;
//		}
//		
//		else//0�� ����
//		{
//			ramLak[i] = 0;
//		}
//	}
//
//	for (int i = 0; i < 512; i++)
//	{
//	//	printf("index %d:  %f\n", i, ramLak[i]);
//	}
//
}

//===========================================================================
// signogram�� ������ Back-projections�Ͽ� �迭�� �����ϴ� �Լ�
void backProjection(int n)	//n�� ����
{

	double sinogramCpy[MAX_C];
	//double sinogramCpy2[MAX_C];
	double zeroFill[MAX_C];

	for (int k = 0; k < MAX_C; k++)
	{
		sinogramCpy[k] = (double)sinogramArr[n][k];//�Ǽ��κ��� ������
		zeroFill[k] = 0.0;	//����κ��� ������
		//sinogramFilter[k] = (float)0.0;
	}

	//printf("����********************************************************************************\n");
	/*
	for (int i = 0; i < 512; i++)
		printf("%d: %f %f\n", i, sinogramCpy[i],zeroFill[i]);
	*/
	//printf("DFT********************************************************************************\n");



	DFT(sinogramCpy, zeroFill, MAX_C, 1);
	//FFT(sinogramCpy, MAX_C/2, 1);


	/*
	for (int i = 0; i < MAX_C; i++)
		printf("%d: %f %f\n", i, sinogramCpy[i],zeroFill[i]);
	*/


//	filter

	for (int t = 1; t < MAX_C; t++)
	{
	
		sinogramCpy[t]*= ramLak[t];
		zeroFill[t]*= ramLak[t];
		//sinogramCpy2[t] = (double)sinogramFilter[t];

	}
	
	
	//printf("IDFT********************************************************************************\n");
	
	//IDFT
	DFT(sinogramCpy, zeroFill, MAX_C, -1);
	//FFT(sinogramCpy, MAX_C, -1);
	/*
	for (int i = 0; i < 512; i++)
		printf("%d: %f %f\n", i, sinogramCpy[i],zeroFill[i]);
		*/

	//����
	for (int p = 0; p < MAX_C; p++)
	{
		for (int q = 0; q < MAX_R; q++)
			//arr4[q][p] = fabs(sinogramCpy[p]);
			arr4[q][p] = sinogramCpy[p];
	}

	if (n == 1)
	{
		testsinogramfft = fopen("Phantomsinogramfft.raw", "wb");
		for (int r = 0; r < 360; r++)
		{
			for (int c = 0; c < MAX_C; c++)

			{
				fputc(arr4[r][c], testsinogramfft);
			}
		}
		fclose(testsinogramfft);
	}


}

//===========================================================================
// rotation ��갪�� ��ȯ�ϴ� �Լ� 
setRC calRotation2(int i, int j, double t)
{
	setRC n;
	n.rl = ((i - MAX_R / 2)*cos(t) - (j - MAX_C / 2)*sin(t)) + MAX_R / 2;	//���������� ������ �Լ�
	n.cl = ((i - MAX_R / 2)*sin(t) + (j - MAX_C / 2)*cos(t)) + MAX_C / 2;

	n.rr = (int)(n.rl);
	n.cc = (int)(n.cl);
	//n.rr = (int)(n.rl+0.5);	//�ݿø��� ���� 0.5�� ������
	//n.cc = (int)(n.cl+0.5);
	//printf("%d %d\n", n.rr, n.cc);
	return n;

}

//===========================================================================
//Original�� ����� ���� rotation��Ű�� �Լ�
void backRotation(double n)
{
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			if (calRotation2(i, j, n).rr >= 0 && calRotation2(i, j, n).rr <= 511 && calRotation2(i, j, n).cc >= 0 && calRotation2(i, j, n).cc <= 511) //�迭�� ������ �� 0~511 ���� ���� ������
			{
				arr5[calRotation2(i, j, n).rr][calRotation2(i, j, n).cc] = arr4[i][j];
			}
			else
			{

			}

		}
	}
}

//===========================================================================
//rotation�� �迭�� ����Ǿ��ִ� arr5�� arr6�� �����ϴ� �Լ�
void makeOriginPhoto(void) 
{	
	//arr6�� arr5�� ���� �����ؼ� ����
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			arr6[i][j] += arr5[i][j];
		}
	}

}
//===========================================================================
void make8Bit(void)
{
	//min�� max�� ã��
	min = 0;
	max = 0;

	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			if (min >= arr6[i][j])
				min = arr6[i][j];

			if (max <= arr6[i][j])
			{
				max = arr6[i][j];
				//printf("max: %d\n", max);
			}
		}
	}
	printf("%f %f", min, max);

	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			//printf("%d %d %f\n", i, j, arr6[i][j]);
			arr6[i][j] = (((float)arr6[i][j] - min) / (max - min) * 255.0);
			
			//printf("[%d][%d]: %d\n ", i,j,arr6[i][j]);
			//if (arr6[i][j] != 0)
				//printf("%d %d %d\n ", arr6[i][j], i,j);
		}
	}
	//printf("finished");
	
}
//===========================================================================
//BackProjection�� ������ ������ ��ģ ���� ������ ���
void printLastPhoto(void)
{
	setZero1();					//arr6�ʱ�ȭ, ó���� �ѹ��� �������ָ��

	for (int i = 0; i < 180; i++)
	{
		setZero2();				//arr4�ʱ�ȭ, sinogram ��ƿ��� ���� �ʱ�ȭ�����ָ� ��

		backProjection(i);				//sinogram ��ƿ�
		backRotation(M_PI / 180 * i);	//ȸ��
		makeOriginPhoto();				//����

	}

	//make8Bit();

	testLast = fopen("testPhantom.raw", "wb");
	
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			arr6[i][j] /= 180;
			fputc(arr6[i][j], testLast);
		}
	}
	fclose(testLast);
}


//===========================================================================
int main(void) {
	openFile();
	makeSinogramPlane();
	RamLakFilter();
	printLastPhoto();
	return 0;
}
//===========================================================================
