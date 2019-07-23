#pragma once
#include <d3dx9.h>
#pragma once
#include "define.h"
#include "Frustum.h"
#include <stdio.h>

/*
����ó�� ȿ���� ����Ʈ��
*/
class QuadTree
{
	enum		CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
	enum			QuadLocation {
		FRUSTUM_OUT = 0,			// �������ҿ��� �������
		FRUSTUM_PARTIALLY_IN = 1,	// �������ҿ� �κ�����
		FRUSTUM_COMPLETELY_IN = 2,	// �������ҿ� ��������
	};
	
	enum { EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT };

private:
	QuadTree* m_pChild[4];
	QuadTree* m_pNeighbor[4];
	QuadTree* m_pParent;

	int m_nCenter;			//�߾� Index - �ּҴ���(1)�� ��쿡�� m_nCenter ���� ���� ������ �����ϴ���
	int m_nCorner[4];		//�ڳʵ��� Index
	bool m_bCulled;
	float m_fRadius;



public:
	QuadTree(int cx, int cy);
	QuadTree(QuadTree* pParent);
	~QuadTree();

	BOOL Create(TERRAIN_VTX* pHeightMap);
	int GenerateIndex(VOID* pIb, TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio);
	void GetCorner(int& _0, int& _1, int& _2, int& _3)
	{
		_0 = m_nCorner[0]; _1 = m_nCorner[1]; _2 = m_nCorner[2]; _3 = m_nCorner[3];
	}

private:
	BOOL _SetCorners(int TL, int TR, int BL, int BR);
	QuadTree* _AddChild(int TL, int TR, int BL, int BR);
	BOOL _SubDivide();

	int	_GenTriIndex(int nTris, VOID* pIndex, TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio);

	void _FrustumCull(TERRAIN_VTX* pHeightMap, Frustum* pFrustum);
	int _IsInFrustum(TERRAIN_VTX* pHeightMap, Frustum* pFrustum);

	// �̿���带 �����.(�ﰢ�� ������ ������)
	void		_BuildNeighborNode(QuadTree* pRoot, TERRAIN_VTX* pHeightMap, int cx);
	// ����Ʈ���� �˻��ؼ� 4�� �ڳʰ��� ��ġ�ϴ� ��带 ã�´�.
	QuadTree* _FindNode(TERRAIN_VTX* pHeightMap, int _0, int _1, int _2, int _3);
	// 4�� ����(���,�ϴ�,����,����)�� �̿���� �ε����� ���Ѵ�.
	int			_GetNodeIndex(int ed, int cx, int& _0, int& _1, int& _2, int& _3);
	// ����Ʈ���� �����.(Build()�Լ����� �Ҹ���)
	BOOL		_BuildQuadTree(TERRAIN_VTX* pHeightMap);



	float		_GetDistance(D3DXVECTOR3* pv1, D3DXVECTOR3* pv2)
	{
		return D3DXVec3Length(&(*pv2 - *pv1));
	}

	// ī�޶�� ���� ������ �Ÿ����� �������� LOD���� ���Ѵ�.
	int			_GetLODLevel(TERRAIN_VTX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		//float d = _GetDistance((D3DXVECTOR3*)(pHeightMap + m_nCenter), pCamera);
		float d = _GetDistance(&((pHeightMap + m_nCenter)->p), pCamera);
		return max((int)(d * fLODRatio), 1);
	}

	// ���� ��尡 LOD������� ����  ����� ������ ����ΰ�?
	BOOL		_IsVisible(TERRAIN_VTX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		int lv = _GetLODLevel(pHeightMap, pCamera, fLODRatio);
		return ((m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= lv);
	}
};