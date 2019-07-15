#pragma once
//#include "ObjGraphic.h"
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct MYINDEX
{
	WORD _0, _1, _2;
};

class ObjTriangle 
{
private:
	LPDIRECT3DVERTEXBUFFER9 _pVB;
	LPDIRECT3DINDEXBUFFER9	_pIB;

public:
	ObjTriangle();
	~ObjTriangle();
	
	void drawObj(LPDIRECT3DDEVICE9 _pd3dDevice);
	HRESULT create(LPDIRECT3DDEVICE9 _pd3dDevice,
		D3DXVECTOR3 center = { 0, 0, 0 });
	HRESULT InitVB(LPDIRECT3DDEVICE9 _pd3dDevice, D3DXVECTOR3 center = { 0, 0, 0 });
	HRESULT InitIB(LPDIRECT3DDEVICE9 _pd3dDevice);
	HRESULT InitMtrl(D3DCOLORVALUE material = { 1, 1, 0, 1 });

	D3DCOLORVALUE _material;
	WORD _fvf;
	D3DXMATRIXA16 matWorld, translation, rotation, scale;


};