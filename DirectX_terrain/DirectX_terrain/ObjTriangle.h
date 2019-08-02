#pragma once
#include "ObjGraphic.h"
#include "define.h"
#include "FIleReadObj.h"

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};
struct MYINDEX
{
	WORD _0, _1, _2;
};

class ObjTriangle : public ObjGraphic
{
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;


public:
	ObjTriangle(LPDIRECT3DDEVICE9 m_device = NULL);
	~ObjTriangle();

	void DrawMain();
	void DrawMap();
	HRESULT Create(D3DXVECTOR3 pos[3]);

	HRESULT _InitVB(D3DXVECTOR3 pos[3]);
	HRESULT _InitIB();
	HRESULT _InitMtrl();

	


};

