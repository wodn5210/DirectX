#pragma once
#define GRAVITY 0.0098f
#define PI 3.141592f
#define Epsilon 0.00005f
#include <vector>
#include <string>

struct TRI_IDX
{
	WORD _0, _1, _2;
};

struct TERRAIN_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};
struct BALL_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};
struct SKY_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ| D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR2	t;
};
struct TREE_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR2	t;
};

struct BAR_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ) };
	D3DXVECTOR3	p;

};


class MATH {
public:
	static BOOL IsInRect(RECT* rc, POINT pt)
	{
		if ((rc->left <= pt.x) && (pt.x <= rc->right) &&
			(rc->bottom <= pt.y) && (pt.y <= rc->top))
			return TRUE;

		return FALSE;
	}

	static BOOL IsInRange(float value, float r0, float r1)
	{
		if (r0 <= value && value <= r1)
			return TRUE;
		return FALSE;
	}
	// 2를 밑으로 하는 숫자 n의 로그값을 구한다.
	static int	Log2(int n)
	{
		for (int i = 1; i < 64; i++)
		{
			n = n >> 1;
			if (n == 1) return i;
		}

		return 1;
	}

	// 2^n값을 구한다
	static int	Pow2(int n)
	{
		int val = 1;
		val = val << n;
		return val;
	}

	static float Lerp(float a, float b, float t)
	{
		return a - (a * t) + (b * t);
	}
};
