#pragma once
#include "ObjGraphic.h"
#include "define.h"
using namespace std;
class ObjSkyBox :
	public ObjGraphic
{
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	LPDIRECT3DTEXTURE9		m_pTex[6];

	D3DXMATRIXA16 forTXT[6];

public:
	ObjSkyBox(LPDIRECT3DDEVICE9 device = NULL);
	~ObjSkyBox();

	void DrawMain();

	HRESULT Create();

	HRESULT _InitVB();
	HRESULT _InitIB();
	HRESULT _InitTXT();
	HRESULT _InitMtrl();
};

