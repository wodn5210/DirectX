#pragma once
#include <d3dx9.h>
#include "define.h"

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};
struct MYINDEX
{
	WORD _0, _1, _2;
};

class Triangle
{
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	WORD _fvf;
	D3DXMATRIXA16 _matWorld, _translation, _rotation, _scale;

public:
	Triangle(LPDIRECT3DDEVICE9 m_device = NULL);
	~Triangle();

	void DrawObj();
	HRESULT Create(D3DXVECTOR3 pos[3]);

	HRESULT _InitVB(D3DXVECTOR3 pos[3]);
	HRESULT _InitIB();
	HRESULT _InitMtrl();

	void SetTranslation(D3DXMATRIXA16& translation)
	{
		_translation = translation;
	}
	void SetScale(D3DXMATRIXA16& scale)
	{
		_scale = scale;
	}


};

