#pragma once
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <vector>

using namespace std;


struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct MYINDEX
{
	WORD _0, _1, _2;
};


class ObjGraphic
{
protected:	
	D3DCOLORVALUE _material;
	WORD _fvf;
	D3DXMATRIXA16 _matWorld, _translation, _rotation, _scale;

public:
	virtual void DrawObj(LPDIRECT3DDEVICE9 g_pd3dDevice) = 0;
	virtual HRESULT Create(LPDIRECT3DDEVICE9 g_pd3dDevice) = 0;

	void SetTranslation(D3DXMATRIXA16& translation)
	{
		_translation = translation;
	}
	void SetScale(D3DXMATRIXA16& scale)
	{
		_scale = scale;
	}

	void SetMaterial(D3DCOLORVALUE material)
	{
		_material = material;
	}
};