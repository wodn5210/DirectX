#pragma once

struct TERRAIN_VTX
{
	enum _FVF { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};
struct TRI_IDX
{
	WORD _0, _1, _2;
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
	// 2�� ������ �ϴ� ���� n�� �αװ��� ���Ѵ�.
	static int	Log2(int n)
	{
		for (int i = 1; i < 64; i++)
		{
			n = n >> 1;
			if (n == 1) return i;
		}

		return 1;
	}

	// 2^n���� ���Ѵ�
	static int	Pow2(int n)
	{
		int val = 1;
		val = val << n;
		return val;
	}
};