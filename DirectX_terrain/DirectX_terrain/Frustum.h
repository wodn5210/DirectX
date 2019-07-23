#pragma once
#include <d3dx9.h>

//프러스텀 컬링 여유있게
#define PLANE_EPSILON 5.0f

/*
프러스팀 내부만 렌더링 목적
프러스텀 내부 판단 목적
*/
struct FRS_IDX
{
	WORD _0, _1, _2;
};


class Frustum
{
private:
	enum { _FVF = D3DFVF_XYZ};

	LPDIRECT3DDEVICE9       g_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 g_pVB;
	LPDIRECT3DINDEXBUFFER9	g_pIB;

	//프러스텀은 (-1, -1, 0) ~ (1, 1, 1)까지 직육면체로 구성됨
	//그래서 점8개와 6개 평면 생성
	D3DXVECTOR3 m_projVTX[8];

	D3DXVECTOR3	m_vtx[8];	// 프러스텀을 구성할 정점 8개
	D3DXVECTOR3	m_vPos;		// 현재 카메라의 월드좌표
	D3DXPLANE	m_plane[6];	// 프러스텀을 구성하는 6개의 평면

public:
	Frustum(LPDIRECT3DDEVICE9 pDev = NULL);

	// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
	BOOL	Make(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3* pos);

	//프러스텀 안에 있으면 True
	BOOL	IsIn(D3DXVECTOR3* pv);

	//경계구가 프러스텀 안에 있음?
	BOOL	IsInSphere(D3DXVECTOR3* pv, float radius);

	// 프러스텀을 화면에 그려준다.
	BOOL	Draw();


	HRESULT InitIB();
	HRESULT InitVB();

	//카메라 위치
	D3DXVECTOR3* GetPos() { return &m_vPos; }
	VOID SetPos(D3DXVECTOR3* pos) { m_vPos = *pos; }
};

