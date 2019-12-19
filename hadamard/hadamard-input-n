#include <stdio.h>
#include <stdlib.h>
//=========================================================================
int n=0;
int*set;
int*copy;
int stage = 1;
typedef struct inttwo
{
	int a;
	int b;
}IT;


//=========================================================================
bool check2k(int a)
{
	int result = a & (a - 1);//조건검사 a가 2의 k승
	//printf("%d",a);
	//printf("\n n: %d result: %d",n,result);

	if (result == 0 && a>=1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//-------------------------------------------------
int inputN(void)
{
	printf("\nInput n (n=2^k): ");
	scanf_s("%d", &n);
	//printf("%d", n);
	return n;
}
//-------------------------------------------------
void newnnOrNot()
{
	inputN();
	while (check2k(n) == false)
	{
		printf("n=2^k!!\n");
		inputN();
	}
	//return 0;
}
//-------------------------------------------------
IT hadamard2(int x, int y)
{
	IT t;
	t.a = x + y;
	t.b = x - y;
	return t;
}
//-------------------------------------------------
void hadamard(int b[],int p)
{
	copy = (int*)malloc(sizeof(int)*n);
	int start = 0;			//시작index=0
	int startcount = 1;		//묶음으로 있는 개수를 셈

	for (int j = 0; j < n; j++)
	{
		copy[j] = b[j];		//배열 b를 바꾸기 위해 원래 b에 있던 내용 복사
	}

	for (int i = 0; i < n / 2; i++)
	{
		b[start] = hadamard2(copy[start], copy[start+p]).a;
		b[start + p] = hadamard2(copy[start], copy[start+p]).b;
		
		if (startcount<p)
		{
			start++;
			startcount++;//모래시계(?) 시작부분을 세어줌
		}
		else 
		{
			start += (p+1);
			startcount = 1;
		}
	}

	printf("\nSTAGE %d: ",stage);
	for (int i = 0; i < n; i++)
	{
		printf("%d ", b[i]);
	}
	printf("\n");
	

	if (p < n/2)				//stage 반복 정해줌, p가 배열의 크기보다 작으면
	{
		stage++;
		hadamard(b, p * 2);
	}
}
//=========================================================================
int main(void) 
{
	newnnOrNot();
	set = (int*)malloc(sizeof(int)*n);
	printf("\n***\n");
	printf("하다마드 변환을 시킬 %d개의 원소를 입력하세요: ", n);
	for (int j = 0; j < n; j++)
	{
		scanf("%d", &set[j]);
	}

	printf("\nx: ");

	for (int i = 0; i < n; i++)
	{
		printf("%d ", set[i]);
	}
	printf("\n");

	hadamard(set,1);

	free(set);
	free(copy);
	return 0;
}
//=========================================================================
