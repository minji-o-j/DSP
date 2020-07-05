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
- [겪었던 문제와 해결 방법](#-겪었던-문제와-해결-방법)  
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


<br>

<br>

## ◼ 겪었던 문제와 해결 방법  

![image](https://user-images.githubusercontent.com/45448731/86535963-fd6e2680-bf1e-11ea-9c0b-21da71465e0a.png)
> Head.bmp (원본)  
<br>

![image](https://user-images.githubusercontent.com/45448731/86536150-51c5d600-bf20-11ea-98d3-d9e5790bdfae.png)  
> 선이 나타난 Sinogram  
<br>

#### ◼ 처음에 Sinogram을 만드는 과정에서 **데이터가 손실되는 문제**가 발생  
- 손실되는 데이터가 가운데 축을 중심으로 대칭  
- Sinogram을 만들수록(그림 하단으로 갈 수록) 손상된 영역이 더욱 넓어짐  
  <br>
  
#### ◻ 손상되는 이유  
  - 2차원 배열을 1°씩 돌리는 과정에서 index가 범위를 초과하여 data의 값이 제대로 들어가지 않기 때문  
  - 1°씩 반복해서 돌릴 때 원본 data를 돌린것이 아니라 이미 손상되어있는 data를 반복해서 돌렸기 때문  
  <br>

#### ◼ 수정 결과

![image](https://user-images.githubusercontent.com/45448731/86536163-6904c380-bf20-11ea-843f-765aaf1519a1.png)  
> Head.bmp에 대한 Sinogram
