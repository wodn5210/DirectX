#pragma once
#include <d3dx9.h>
#include <vector>
#include <algorithm>

#include "define.h"
#include "Frustum.h"
#include "Ray.h"
#include <stdio.h>

using namespace std;
/*
지형처리 효율적 쿼드트리
*/
class QuadTree
{
	enum		CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
	enum			QuadLocation {
		FRUSTUM_OUT = 0,			// 프러스텀에서 완전벗어남
		FRUSTUM_PARTIALLY_IN = 1,	// 프러스텀에 부분포함
		FRUSTUM_COMPLETELY_IN = 2,	// 프러스텀에 완전포함
	};
	
	enum { EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT };

private:


	int m_x, m_y;

	QuadTree* m_pChild[4];
	QuadTree* m_pNeighbor[4];
	QuadTree* m_pParent;

	int m_nCenter;			//중앙 Index - 최소단위(1)인 경우에는 m_nCenter 보장 못함 어차피 사용안하더라
	WORD m_nCorner[4];		//코너들의 Index
	bool m_bCulled;
	float m_fRadius;

	//쿼드트리에서 가시화 하는 Index 저장해둔다
	vector <TRI_IDX> m_VisibleIdx;

public:
	QuadTree(int cx, int cy);
	QuadTree(QuadTree* pParent);
	~QuadTree();

	int GetX() { return m_x; }
	int GetY() { return m_y; }


	BOOL Create(TERRAIN_VTX* pHeightMap);
	int GenerateIndex(VOID* pIb, TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio);
	int GenerateMapIdx(VOID* pIb, TERRAIN_VTX* pHeightMap);
	void GetCorner(int& _0, int& _1, int& _2, int& _3)
	{
		_0 = m_nCorner[0]; _1 = m_nCorner[1]; _2 = m_nCorner[2]; _3 = m_nCorner[3];
	}

	VOID SearchInTree(Ray ray, float& dist, D3DXVECTOR3 pos[3], TERRAIN_VTX* pHeightMap);
	// 쿼드트리를 검색해서 4개 코너값과 일치하는 노드를 찾는다.
	QuadTree* FindNode(TERRAIN_VTX* pHeightMap, int _0, int _1, int _2, int _3);
private:
	BOOL _SetCorners(int TL, int TR, int BL, int BR);
	QuadTree* _AddChild(int TL, int TR, int BL, int BR);
	BOOL _SubDivide();

	int	_GenTriIndex(int nTris, VOID* pIndex, TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio);

	void _FrustumCull(TERRAIN_VTX* pHeightMap, Frustum* pFrustum);
	int _IsInFrustum(TERRAIN_VTX* pHeightMap, Frustum* pFrustum);

	// 이웃노드를 만든다.(삼각형 찢어짐 방지용)
	void		_BuildNeighborNode(QuadTree* pRoot, TERRAIN_VTX* pHeightMap);
	
	// 4개 방향(상단,하단,좌측,우측)의 이웃노드 인덱스를 구한다.
	int			_GetNodeIndex(int ed, int& _0, int& _1, int& _2, int& _3);
	// 쿼드트리를 만든다.(Build()함수에서 불린다)
	BOOL		_BuildQuadTree(TERRAIN_VTX* pHeightMap);

	//QuadTree* _GetTree


	float		_GetDistance(D3DXVECTOR3* pv1, D3DXVECTOR3* pv2)
	{
		return D3DXVec3Length(&(*pv2 - *pv1));
	}

	// 카메라와 현재 노드와의 거리값을 기준으로 LOD값을 구한다.
	int			_GetLODLevel(TERRAIN_VTX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		//float d = _GetDistance((D3DXVECTOR3*)(pHeightMap + m_nCenter), pCamera);
		float d = _GetDistance(&((pHeightMap + m_nCenter)->p), pCamera);
		return max((int)(d * fLODRatio), 1);
	}

	// 현재 노드가 LOD등급으로 볼때  출력이 가능한 노드인가?
	BOOL		_IsVisible(TERRAIN_VTX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		int lv = _GetLODLevel(pHeightMap, pCamera, fLODRatio);
		return ((m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= lv);
	}
};