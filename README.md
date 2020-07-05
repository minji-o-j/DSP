![image](https://img.shields.io/github/license/minji-o-j/DSP?style=flat-square)
![HitCount](http://hits.dwyl.com/minji-o-j/DSP.svg) 
---

# 👉hadamard 변환
- [Code (C언어)](https://github.com/minji-o-j/DSP/tree/master/hadamard)  
---

#  👉Radon-Transform 을 통한 CT(Computed Tomography)기술
- [Code (C언어)](https://github.com/minji-o-j/DSP/blob/master/CT/DSP_%EC%B5%9C%EC%A2%85/Project1/test1.cpp) 
- [CT의 원리](#-CT의-원리)  
- [구현 결과](#-구현-결과)  
<br>

## ◼ CT의 원리
![image](https://user-images.githubusercontent.com/45448731/86435959-2edacc80-bd3c-11ea-9862-41ded0e5428e.png)  

> Radon-Transform을 이용한 CT 기술  
```
1. 원본 이미지를 θ°씩 회전시키며 나온 1차원 데이터가 합쳐진 Sinogram을 제작한다.
2. Sinogram 이미지를 1차원씩 불러와 주파수 영역으로 변환한 후 Filter을 적용한다.
3. Back-Projection(Inverse-Radon Transform)을 한다.
```
<br>

<br>

<br>

<br>

![image](https://user-images.githubusercontent.com/45448731/86508873-789be380-be1e-11ea-8cb6-61b8ca4fcfb5.png)  

> Sinogram with Radon-Transform

<br>

<br>

<br>

<br>

![image](https://user-images.githubusercontent.com/45448731/86513533-e60f3a80-be45-11ea-93f1-b4830d9c8820.png)
> Back-Projection의 원리 
<br>

<br>

## ◼ 구현 결과
