#pragma once
#include <d3dx9.h>

//프러스텀 컬링 여유있게
#define PLANE_EPSILON 5.0f

/*
프러스팀 내부만 렌더링 목적
프러스텀 내부 판단 목적
*/

class Frustum
{
private:
	D3DXVECTOR3	m_vtx[8];	// 프러스텀을 구성할 정점 8개
	D3DXVECTOR3	m_vPos;		// 현재 카메라의 월드좌표
	D3DXPLANE	m_plane[6];	// 프러스텀을 구성하는 6개의 평면

public:
};

