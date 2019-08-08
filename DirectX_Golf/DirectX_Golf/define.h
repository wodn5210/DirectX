#pragma once
#define GRAVITY 0.0098f
#define EPSILON 0.00005f

#include <stdio.h>
#include <vector>
#include <string>
#include <list>
#include <d3dx9.h>

struct TRI_IDX
{
	WORD _0, _1, _2;
};
struct TRI_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL) };
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
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
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR2	t;
};
struct BNT_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR2	t;
};
struct TREE1_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR2	t;
};
struct TREE2_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
};

struct BAR_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ) };
	D3DXVECTOR3	p;
};
struct GORI_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ) };
	D3DXVECTOR3 p;
};
struct Particle
{
	enum _FVF {FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE) };
	D3DXVECTOR3 p;
	D3DCOLOR	c;
};
struct Attribute
{
	Attribute()
	{
		_lifeTime = 0.0f;
		_age = 0.0f;
		_isAlive = true;
	}
	D3DXVECTOR3 _position;
	D3DXVECTOR3 _velocity;
	D3DXVECTOR3 _acceleration;
	float       _lifeTime;     
	float       _age;         
	D3DXCOLOR   _color;       
	bool        _isAlive;
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

	static float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound) // bad input
			return lowBound;

		// get random float in [0, 1] interval
		float f = (rand() % 10000) * 0.0001f;

		// return float in [lowBound, highBound] interval. 
		return (f * (highBound - lowBound)) + lowBound;
	}
	static void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
	{
		out->x = GetRandomFloat(min->x, max->x);
		out->y = GetRandomFloat(min->y, max->y);
		out->z = GetRandomFloat(min->z, max->z);
	}
	static DWORD FtoDw(float r)
	{
		return *((DWORD*)&r);
	}
};
