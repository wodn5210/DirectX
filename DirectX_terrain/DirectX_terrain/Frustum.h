#pragma once
#include <d3dx9.h>

//�������� �ø� �����ְ�
#define PLANE_EPSILON 5.0f

/*
�������� ���θ� ������ ����
�������� ���� �Ǵ� ����
*/

class Frustum
{
private:
	D3DXVECTOR3	m_vtx[8];	// ���������� ������ ���� 8��
	D3DXVECTOR3	m_vPos;		// ���� ī�޶��� ������ǥ
	D3DXPLANE	m_plane[6];	// ���������� �����ϴ� 6���� ���

public:
};

