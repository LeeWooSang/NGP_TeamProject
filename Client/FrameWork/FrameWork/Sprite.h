#pragma once
#include <Windows.h>

 struct Data
{
	HBITMAP Bitmap;					// 이미지 자체
	int width;						// 이미지의 가로너비
	int height;						// 이미지의 세로높이
	int x;
	int y;
};
class Sprite
{
private:
	int  Index;						// 사용자가 배열에 로드한 스프라이트 이미지의 총 개수
	Data BmpData[30];				// 애니메이션에 필요한 이미지'들'을 담을 수 있는 배열
public:
	Sprite();
	~Sprite();

public:
	void Entry(int Num, const char* path, int x, int y);		// 이미지 로드

public:
	void Render(HDC* hdc, int Num);		// 애니메이션을 그리기 위한 렌더
	void Render(HDC* hdc, int Num, float wPer, float hPer);
	void Render(HDC* hdc, int Num, UINT color);
	void Render(HDC* hdc, int Num, float a);
	void Render(HDC* hdc, int Num, UINT color, float a);

public:
	void setLocation(int Num, int x, int y);
	void setLocation(int x, int y);

	void Move(int x, int y);
public:
	int getX(int Num);
	int getY(int Num);
	int getWidth(int Num);
	int getHeight(int Num);
	int getIndex();
};

