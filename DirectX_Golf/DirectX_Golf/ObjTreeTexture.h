#pragma once
#include "ObjGraphic.h"
#include "define.h"

using namespace std;

class ObjTreeTexture :
	public ObjGraphic
{
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	LPDIRECT3DTEXTURE9		m_pTex;

	D3DXMATRIXA16* m_matBillBoard;

public:
	ObjTreeTexture();
	~ObjTreeTexture();

	void DrawMain();

	HRESULT Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3 center, string txt);
	VOID SetBillBoard(D3DXMATRIXA16* matbill) { m_matBillBoard = matbill; }

private:
	HRESULT _InitVB();
	HRESULT _InitIB();
	HRESULT _InitTXT(string txt);
	HRESULT _InitMtrl();
};

