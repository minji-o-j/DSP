#pragma once
#define MAX_R 512	//row ��
#define MAX_C 512	//column �� 
//#define MAX_R 724	//row ��
//#define MAX_C 724	//column �� 


typedef struct setLC {
	double rd; 
	double cd;
	int r;
	int c;//�迭��ȣ�� int������
}setLC;

typedef struct setRC {
	double rl;
	double cl;
	int rr;
	int cc;
}setRC;
