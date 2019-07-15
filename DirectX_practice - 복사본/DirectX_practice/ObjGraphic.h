#pragma once
#include <d3dx9.h>
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
	D3DXMATRIXA16 matWorld, translation, rotation, scale;

public:
	virtual void drawObj(LPDIRECT3DDEVICE9 _pd3dDevice) = 0;
	virtual HRESULT create(LPDIRECT3DDEVICE9 _pd3dDevice, 
		D3DXVECTOR3 center = { 0, 0, 0 }) = 0;
};