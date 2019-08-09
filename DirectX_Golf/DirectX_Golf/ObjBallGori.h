#pragma once
#include "ObjGraphic.h"
#include <queue>
using namespace std;

class ObjBallGori :
	public ObjGraphic
{
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	LPDIRECT3DTEXTURE9		m_pTex;


	GORI_VTX m_vtx[22];



public:
	ObjBallGori();
	~ObjBallGori();

	void DrawMain();
	HRESULT ReadyRender(D3DXVECTOR3* pos, D3DXVECTOR3* dir);
	//공위치와 dir로 

	HRESULT Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3* pos);
	HRESULT Init(D3DXVECTOR3* pos);

private:
	HRESULT _InitVB(D3DXVECTOR3* pos);
	HRESULT _InitIB();
	HRESULT _InitTexture();
	HRESULT _InitMtrl();

};

