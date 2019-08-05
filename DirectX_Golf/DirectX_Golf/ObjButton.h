#pragma once
#include "ObjGraphic.h"


using namespace std;
class ObjButton :
	public ObjGraphic
{
private:
	


	LPDIRECT3DVERTEXBUFFER9 m_pVB = NULL;
	LPDIRECT3DINDEXBUFFER9	m_pIB = NULL;

	LPDIRECT3DTEXTURE9		m_pTex = NULL;

	D3DXMATRIXA16 m_matViewProj;

	BNT_VTX m_vtx[4];
	BNT_VTX m_drawVtx[4];


public:


	ObjButton();
	~ObjButton();

	void DrawMain();

	HRESULT Create(LPDIRECT3DDEVICE9 device, string path);
	HRESULT ReadyRender(D3DXMATRIXA16* pmatViewProj);

	VOID SetDrawPos(D3DXVECTOR2 lt, D3DXVECTOR2 rb);

	BNT_VTX* GetDrawVtx() { return m_drawVtx; };



private:
	HRESULT _InitVB();
	HRESULT _InitIB();
	HRESULT _InitTexture(string path);
	HRESULT _InitMtrl();
};

