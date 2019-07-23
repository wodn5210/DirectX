#pragma once
#include <d3dx9.h>

//�������� �ø� �����ְ�
#define PLANE_EPSILON 5.0f

/*
�������� ���θ� ������ ����
�������� ���� �Ǵ� ����
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

	//���������� (-1, -1, 0) ~ (1, 1, 1)���� ������ü�� ������
	//�׷��� ��8���� 6�� ��� ����
	D3DXVECTOR3 m_projVTX[8];

	D3DXVECTOR3	m_vtx[8];	// ���������� ������ ���� 8��
	D3DXVECTOR3	m_vPos;		// ���� ī�޶��� ������ǥ
	D3DXPLANE	m_plane[6];	// ���������� �����ϴ� 6���� ���

public:
	Frustum(LPDIRECT3DDEVICE9 pDev = NULL);

	// ī�޶�(view) * ��������(projection)����� �Է¹޾� 6���� ����� �����.
	BOOL	Make(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3* pos);

	//�������� �ȿ� ������ True
	BOOL	IsIn(D3DXVECTOR3* pv);

	//��豸�� �������� �ȿ� ����?
	BOOL	IsInSphere(D3DXVECTOR3* pv, float radius);

	// ���������� ȭ�鿡 �׷��ش�.
	BOOL	Draw();


	HRESULT InitIB();
	HRESULT InitVB();

	//ī�޶� ��ġ
	D3DXVECTOR3* GetPos() { return &m_vPos; }
	VOID SetPos(D3DXVECTOR3* pos) { m_vPos = *pos; }
};

