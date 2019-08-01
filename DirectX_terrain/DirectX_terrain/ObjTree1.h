#pragma once
#include "ObjGraphic.h"
#include "define.h"

using namespace std;

class ObjTree1 :
	public ObjGraphic
{
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	LPDIRECT3DTEXTURE9		m_pTex;


public:
	ObjTree1(LPDIRECT3DDEVICE9 device = NULL);
	~ObjTree1();

	void DrawMain(D3DXMATRIXA16* matbill);

	HRESULT Create(D3DXVECTOR3 center, string txt);


private:
	HRESULT _InitVB();
	HRESULT _InitIB();
	HRESULT _InitTXT(string txt);
	HRESULT _InitMtrl();
};

