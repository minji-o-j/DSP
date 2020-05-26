#define _USE_MATH_DEFINES
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "hh.h"

//===========================================================================
float arr[MAX_R][MAX_C];			//원래raw값을 담을 배열, 8비트
float arr2[MAX_R][MAX_C];		//rotation한 raw값을 담을 배열, 8비트
float arr3[MAX_R][MAX_C];
float oneDSinogram[MAX_C];		//one Dimension Sinogram, 1차원 sinogram배열, 16비트
float ramLak[MAX_C];			//ram-Lak filter 값 담음
float sinogramFilter[MAX_C];	//필터씌운 sinogram
float sinogramArr[360][MAX_C];	//sinogram 값 담음, 16비트
float arr4[MAX_R][MAX_C];		//sinogram 사진을 나누어 저장할 2차원 배열 (backProjection에서 이용)
float arr5[MAX_R][MAX_C];		//Oritoation한 raw값을 담을 배열
float arr6[MAX_R][MAX_C];		// rotation한 값을 누적시킬 배열
float min;
float max;


FILE *fp;//사진 불러옴
FILE *testsinogram;						//sinogram 출력
FILE *testLast;							//원본 사진(필터 씌우기 전) 출력

//===========================================================================
//파일을 불러오는 함수
void openFile(void)
{
	//fp = fopen("Head001.raw", "rb");	//8비트짜리임, 0~255
	//fp = fopen("testCircle.raw", "rb");	//8비트짜리임, 0~255
	//fp = fopen("testSquare.raw", "rb");	//8비트짜리임, 0~255
	fp = fopen("Phantom.raw", "rb");	//8비트짜리임, 0~255

	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			arr[i][j] = fgetc(fp);
			//printf("%d ", arr[i][j]);	//출력실험
		}
		//printf("\n");					//출력실험
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
//rotation 계산값 반환하는 함수 ************중심을 기준으로 돌리기 위해 원점 중심으로 이동, 다시 좌표 원래대로 만들어주기
setLC calRotation(int i, int j,double t) {

	setLC n;

	//왼쪽으로 돌림
	n.rd = ((i - MAX_R / 2)*cos(t) + (j - MAX_C / 2)*sin(t)) + MAX_R / 2;
	n.cd = ((-1)*(i - MAX_R / 2)*sin(t) + (j - MAX_C / 2)*cos(t)) + MAX_C / 2;

	n.r = (int)(n.rd);
	n.c = (int)(n.cd);

	return n; 
}

//===========================================================================
//rotation시키는 함수
void rotation(double n)
{

	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			if (calRotation(i,j,n).r>=0&& calRotation(i, j, n).r<=511&& calRotation(i, j, n).c>=0&& calRotation(i, j, n).c<=511)	//index가 0~511 범위에 벗어나지 않으면
			{
				arr2[calRotation(i, j, n).r][calRotation(i, j, n).c] = arr[i][j];
			}

		}
	}

//배열복사
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			arr3[i][j] = arr2[i][j];
		}
	}
}

//===========================================================================
//1차원 sinogram파일로 저장하는 함수
void makeSinogram(void)	//**sinogram은 16bits grayscale로
{
	//열별로 모두 더함
	for (int i = 0; i < MAX_C; i++)
	{
		for (int j = 0; j < MAX_R; j++)
		{
			oneDSinogram[i] += arr3[j][i];
		}
		//oneDSinogram[i] /= 512;		//(pow(2,9))
	
	}
	//printf("\n\n");
}

//===========================================================================
//2차원 sinogram파일 만드는 함수
void makeSinogramPlane(void)
{
	for (int i = 0; i < 360; i++)//180번(180도이므로)만 반복해도 되지만 강의자료와 비교해보기 위해서 360도 돌림
	{
		//printf("\n\n\n%d\n\n", i);
		makeSinogram();

		//sinogram한 1차원 배열 2차원 sinogram에 저장
		for (int j = 0; j < MAX_C; j++)
		{
			sinogramArr[i][j] = (float)oneDSinogram[j];
			//printf("%f\n", (float)oneDSinogram[j]);
		}

		//rotation해서 arr을 바꿈
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

//#########################################################################################################################################################################
//#########################################################################################################################################################################
//#########################################################################################################################################################################
//#########################################################################################################################################################################
//#########################################################################################################################################################################
//#########################################################################################################################################################################

//Inverse Radon-Transform 

//===========================================================================
//누적하는 배열인 arr6을 초기화시키는 함수
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
//sinogram 한줄씩 담을 arr4을 초기화시키는 함수
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
//1차원 DFT
void DFT(double* re, double* im, int N, int dir)
{
	double* tr = (double*)malloc(N * sizeof(double));	//실
	double* ti = (double*)malloc(N * sizeof(double));	//허

	memcpy(tr, re, sizeof(double) * N);
	memcpy(ti, im, sizeof(double) * N);
	
	//for (int i = 0; i < N; i++)
	//{
	//	printf("%d:\n%f %f    %f %f\n\n", i,re[i],tr[i],im[i],ti[i]);
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



	if (dir == -1) // IDFT할 때는 dir이 1임
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
	float y_max = (float)x_max/2;

	for (int i = 0; i < x_max; i++)
	{
		if (i >= 0 && i <= x_max / 4)	//왼쪽 0아닌 부분
		{
			ramLak[i] = i;
		}

		else if (i >= x_max - (x_max / 4) && i < MAX_C)	//오른쪽 0아닌부분
		{
			ramLak[i] = 512 - i;
		}
	
		else//0인 구간, 가운데 두부분
		{
			ramLak[i] = 0;
		}
	}

for (int i = 0; i < 512; i++)
{
//	printf("index %d:  %f\n", i, ramLak[i]);
}


	//for (int i = 0; i < x_max; i++)
	//{
	//	if (i >= (x_max) / 4 && i < (x_max) / 2)	//왼쪽 0아닌 부분
	//	{
	//		ramLak[i] = x_max / 2 - i;
	//	}
	//
	//	else if (i >= (x_max) / 2 && i <= (x_max - (x_max / 4)))	//오른쪽 0아닌부분
	//	{
	//		ramLak[i] = i - x_max / 2;
	//	}
	//	
	//	else//0인 구간
	//	{
	//		ramLak[i] = 0;
	//	}
	//}

	//for (int i = 0; i < 512; i++)
	//{
	////	printf("index %d:  %f\n", i, ramLak[i]);
	//}

}

//===========================================================================
// signogram한 사진을 Back-projections하여 배열에 저장하는 함수
void backProjection(int n)	//n은 각도
{

	double sinogramCpy[MAX_C];
	double sinogramCpy2[MAX_C];
	double zeroFill[MAX_C];

	for (int k = 0; k < MAX_C; k++)
	{
		sinogramCpy[k] = (double)sinogramArr[n][k];//실수부분이 담길거임
		zeroFill[k] = 0.0;	//허수부분이 담길거임
		sinogramFilter[k] = (float)0.0;
	}

	//printf("원본********************************************************************************\n");

	//for (int i = 0; i < 512; i++)
	//	printf("%d: %f %f\n", i, sinogramCpy[i], zeroFill[i]);

//	printf("DFT********************************************************************************\n");

	DFT(sinogramCpy, zeroFill, MAX_C, 1);


	//for (int i = 0; i < MAX_C; i++)
	//	printf("%d: %f %f\n", i, sinogramCpy[i], zeroFill[i]);
	


	//★질문: filter
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

	for (int t = 0; t < MAX_C; t++)
	{
	
		sinogramCpy[t]*= ramLak[t];	//실수부
		zeroFill[t]*= ramLak[t];	//허수부

		/*
		*저렇게 실수부와 허수부에 모두 곱해줘야 하는 건가요?? 
		*
		*dft 결과를 찾아보면 그래프에 가로축이 "주파수"라고 되어있는데, 저희는 실수부와 허수부로 나누어져 있잖아요
		*그러면 이 signal을 합쳐서 dft결과 그래프에 표시하려면 어떤 과정을 거쳐야하나요??
		*
		*만약에 signal을 계산하여 실수허수부분을 합치고, ram-lak filter을 씌우면
		*그 다음에 다시 실수와 허수 부분으로 나누어서 IDFT에 넣어야하는데 어떤식으로 하면 좋을지 알고 싶습니다.
		*
		*
		*
		*/
	
		
		sinogramCpy2[t] = (double)sinogramFilter[t];

	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	

	//printf("IDFT********************************************************************************\n");
	//IDFT
	DFT(sinogramCpy, zeroFill, MAX_C, -1);

	//for (int i = 0; i < 512; i++)
	//	printf("%d: %f %f\n", i, sinogramCpy[i], zeroFill[i]);


}

//===========================================================================
// rotation 계산값을 반환하는 함수 
setRC calRotation2(int i, int j, double t)
{
	setRC n;
	n.rl = ((i - MAX_R / 2)*cos(t) - (j - MAX_C / 2)*sin(t)) + MAX_R / 2;	//오른쪽으로 돌리는 함수
	n.cl = ((i - MAX_R / 2)*sin(t) + (j - MAX_C / 2)*cos(t)) + MAX_C / 2;

	n.rr = (int)(n.rl);
	n.cc = (int)(n.cl);
	//n.rr = (int)(n.rl+0.5);	//반올림을 위해 0.5를 더해줌
	//n.cc = (int)(n.cl+0.5);
	//printf("%d %d\n", n.rr, n.cc);
	return n;

}

//===========================================================================
//Original을 만들기 위해 rotation시키는 함수
void backRotation(double n)
{
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			if (calRotation2(i, j, n).rr >= 0 && calRotation2(i, j, n).rr <= 511 && calRotation2(i, j, n).cc >= 0 && calRotation2(i, j, n).cc <= 511) //배열을 돌렸을 시 0~511 범위 내에 있으면
			{
				arr5[calRotation2(i, j, n).rr][calRotation2(i, j, n).cc] = arr4[i][j];
			}

		}
	}
}

//===========================================================================
//rotation한 배열이 복사되어있는 arr5을 arr6에 누적하는 함수
void makeOriginPhoto(void) 
{	
	//arr6에 arr5의 값을 누적해서 더함
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
	//min과 max를 찾음
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
//BackProjection과 사이즈 조절을 거친 최종 사진을 출력
void printLastPhoto(void)
{
	setZero1();					//arr6초기화, 처음에 한번만 실행해주면됨

	for (int i = 0; i < 180; i++)
	{
		setZero2();				//arr4초기화, sinogram 담아오기 전에 초기화시켜주면 됨

		backProjection(i);				//sinogram 담아옴
		backRotation(M_PI / 180 * i);	//회전
		makeOriginPhoto();				//누적

	}

	make8Bit();

	testLast = fopen("testPhantom.raw", "wb");
	
	for (int i = 0; i < MAX_R; i++)
	{
		for (int j = 0; j < MAX_C; j++)
		{
			//arr6[i][j] /= 180;
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
