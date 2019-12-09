#pragma once
#define MAX_R 512	//row 행
#define MAX_C 512	//column 열 
//#define MAX_R 724	//row 행
//#define MAX_C 724	//column 열 


typedef struct setLC {
	double rd; 
	double cd;
	int r;
	int c;//배열번호라서 int여야해
}setLC;

typedef struct setRC {
	double rl;
	double cl;
	int rr;
	int cc;
}setRC;
