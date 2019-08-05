#pragma once
#include "ObjGraphic.h"
#include "FIleReadObj.h"
#include "define.h"
#include <string>
#include <vector>

using namespace std;

class ObjTree2 :
	public ObjGraphic
{
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB_trunk;
	LPDIRECT3DINDEXBUFFER9	m_pIB_leaves;

	vector<TREE2_VTX> m_vt;
	vector<TRI_IDX> m_idx_leaves, m_idx_trunk;

public:
	ObjTree2(LPDIRECT3DDEVICE9 device = NULL);
	~ObjTree2();

	void DrawMain();

	HRESULT Create(D3DXVECTOR3 center, string txt);


private:
	HRESULT _InitVB();
	HRESULT _InitIB_trunk();
	HRESULT _InitIB_leaves();
	HRESULT _InitMtrl_trunk();
	HRESULT _InitMtrl_leaves();
};

