#include <stdio.h>
#include <stdlib.h>
//=========================================================================
int n=0;
//typedef struct set {
//	int OneSet[1024];
//	int count;
//} SET;
typedef struct inttwo
{
	int a;
	int b;
}IT;

int k = 16;

IT hadamard2(int a, int b)
{
	IT t;
	t.a = a + b;
	t.b = a - b;
	return t;
}

void hadamard(int b[],int p)
{

	int copy[16];
	int start = 0;			//시작index=0
	int startcount = 1;		//묶음으로 있는 개수를 셈

	for (int j = 0; j < k; j++)
	{
		copy[j] = b[j];		//배열 b를 바꾸기 위해 원래 b에 있던 내용 복사
	}

	for (int i = 0; i < k / 2; i++)
	{
		b[start] = hadamard2(copy[start], copy[start+p]).a;
		b[start + p] = hadamard2(copy[start], copy[start+p]).b;
		
		if (startcount<p)
		{
			start++;
			startcount++;
		}
		else 
		{
			start += (p+1);
			startcount = 1;
		}
	}

	printf("\n");
	for (int i = 0; i < k; i++)
	{
		printf("%d ", b[i]);
	}
	printf("\n");
	
	

	if (p < k/2)				//stage 반복 정해줌, p가 배열의 크기보다 작으면
	{
		hadamard(b, p * 2);
	}
}


//=========================================================================
int main(void) 
{
	//newnnOrNot();
	int set[16] = { 3,1,6,4,3,7,20,1,-4,6,5,8,11,2,0,1 };
	printf("\n");

	for (int i = 0; i < k; i++)
	{
		printf("%d ", set[i]);
	}
	printf("\n");

	hadamard(set,1);

	return 0;
}
//=========================================================================
