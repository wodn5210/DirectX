#include "QuadTree.h"



QuadTree::QuadTree(int cx, int cy)
{
	int		i;
	m_pParent = NULL;
	m_nCenter = 0;
	m_x = cx;
	m_y = cy;

	for (i = 0; i < 4; i++)
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
	}


	m_nCorner[CORNER_TL] = 0;
	m_nCorner[CORNER_TR] = cx - 1;
	m_nCorner[CORNER_BL] = cx * (cy - 1);
	m_nCorner[CORNER_BR] = cx * cy - 1;
	m_nCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] +
		m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 4;
	m_bCulled = FALSE;
	m_fRadius = 0.0f;
}
QuadTree::QuadTree(QuadTree* pParent)
{
	int		i;
	m_pParent = pParent;
	m_nCenter = 0;
	m_x = m_pParent->GetX();
	m_y = m_pParent->GetY();

	for (i = 0; i < 4; i++)
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
		m_nCorner[i] = 0;
	}
	m_bCulled = FALSE;
	m_fRadius = 0.0f;
}
QuadTree::~QuadTree()
{
	for (int i = 0; i < 4; i++) 
		delete m_pChild[i];
}

BOOL QuadTree::_SetCorners(int TL, int TR, int BL, int BR)
{

	m_nCorner[CORNER_TL] = TL;
	m_nCorner[CORNER_TR] = TR;
	m_nCorner[CORNER_BL] = BL;
	m_nCorner[CORNER_BR] = BR;
	m_nCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] +
		m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 4;
	return TRUE;
}
QuadTree* QuadTree::_AddChild(int TL, int TR, int BL, int BR)
{
	QuadTree* pChild;

	pChild = new QuadTree(this);
	pChild->_SetCorners(TL, TR, BL, BR);

	return pChild;
}
BOOL QuadTree::_SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// 상단변 가운데
	nTopEdgeCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
	// 하단변 가운데 
	nBottomEdgeCenter = (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
	// 좌측변 가운데
	nLeftEdgeCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
	// 우측변 가운데
	nRightEdgeCenter = (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
	// 한가운데
	nCentralPoint = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] +
		m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 4;

	// 더이상 분할이 불가능한가? 그렇다면 SubDivide() 종료
	if ((m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1)
	{
		return FALSE;
	}

	// 4개의 자식노드 추가
	m_pChild[CORNER_TL] = _AddChild(m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint);
	m_pChild[CORNER_TR] = _AddChild(nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter);
	m_pChild[CORNER_BL] = _AddChild(nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter);
	m_pChild[CORNER_BR] = _AddChild(nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR]);

	return TRUE;
}

int	QuadTree::_GenTriIndex(int nTris, VOID* pIndex, 
	TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio)
{
	m_VisibleIdx.clear();
	// 컬링된 노드라면 그냥 리턴 - 화면 밖이라 그릴 필요가 없다
	if (m_bCulled)
	{
		m_bCulled = FALSE;

		return nTris;
	}


	//출력이 불가능하다는것은 거리가 가까워서 더 미세한 레벨의 Mesh로 렌더링 해야한다는것
	if (_IsVisible(pHeightMap, pFrustum->GetPos(), fLODRatio))
	{
		unsigned short* p = ((unsigned short*)pIndex) + nTris * 3;

		// 만약 최하위 노드라면 부분분할(subdivide)이 불가능하므로 그냥 출력하고 리턴한다.
		if (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1)
		{

			*p++ = m_nCorner[0];	*p++ = m_nCorner[1];	*p++ = m_nCorner[2];	nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			*p++ = m_nCorner[2];	*p++ = m_nCorner[1];	*p++ = m_nCorner[3];	nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			
			return nTris;
		}

		

		BOOL	b[4];
		// 상단 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_UP]) b[EDGE_UP] = m_pNeighbor[EDGE_UP]->_IsVisible(pHeightMap, pFrustum->GetPos(), fLODRatio);
		// 하단 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_DN]) b[EDGE_DN] = m_pNeighbor[EDGE_DN]->_IsVisible(pHeightMap, pFrustum->GetPos(), fLODRatio);
		// 좌측 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_LT]) b[EDGE_LT] = m_pNeighbor[EDGE_LT]->_IsVisible(pHeightMap, pFrustum->GetPos(), fLODRatio);
		// 우측 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_RT]) b[EDGE_RT] = m_pNeighbor[EDGE_RT]->_IsVisible(pHeightMap, pFrustum->GetPos(), fLODRatio);

		// 이웃노드들이 모두다 출력가능하다면 현재노드와 이웃노드들이 같은 LOD이므로 
		// 부분분할이 필요없다.
		if (b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT])
		{
			*p++ = m_nCorner[0];	*p++ = m_nCorner[1];	*p++ = m_nCorner[2];	nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			*p++ = m_nCorner[2];	*p++ = m_nCorner[1];	*p++ = m_nCorner[3];	nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			return nTris;
		}

		WORD		n;

		if (!b[EDGE_UP]) // 상단 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;

			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = n; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TR]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}
		else	// 상단 부분분할이 필요없을 경우
		{
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = m_nCorner[CORNER_TR]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}

		if (!b[EDGE_DN]) // 하단 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;

			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = n; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BL]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}
		else	// 하단 부분분할이 필요없을 경우
		{
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = m_nCorner[CORNER_BL]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}

		if (!b[EDGE_LT]) // 좌측 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;

			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL]; *p++ = n; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TL]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}
		else	// 좌측 부분분할이 필요없을 경우
		{
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL]; *p++ = m_nCorner[CORNER_TL]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}

		if (!b[EDGE_RT]) // 우측 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;

			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR]; *p++ = n; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });

			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BR]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}
		else	// 우측 부분분할이 필요없을 경우
		{
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR]; *p++ = m_nCorner[CORNER_BR]; nTris++;
			m_VisibleIdx.push_back({ *(p - 3), *(p - 2), *(p - 1) });
		}

		return nTris;	// 이 노드 아래의 자식노드는 탐색할 필요없으므로 리턴!
	}

	//출력 불가능한경우 한단계 더 미세한 Child를 이용해서 렌더링한다. (128 -> 64 -> 32 -> 16 -> 8 -> 4 -> 2 -> 1)
	if (m_pChild[CORNER_TL]) nTris = m_pChild[CORNER_TL]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
	if (m_pChild[CORNER_TR]) nTris = m_pChild[CORNER_TR]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
	if (m_pChild[CORNER_BL]) nTris = m_pChild[CORNER_BL]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
	if (m_pChild[CORNER_BR]) nTris = m_pChild[CORNER_BR]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);

	return nTris;
}

BOOL QuadTree::Create(TERRAIN_VTX* pHeightMap)
{	// 쿼드트리 구축
	_BuildQuadTree(pHeightMap);
	// 이웃노드 구축
	_BuildNeighborNode(this, pHeightMap);

	return TRUE;
}

int QuadTree::GenerateIndex(VOID* pIb, TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio)
{
	_FrustumCull(pHeightMap, pFrustum);
	return _GenTriIndex(0, pIb, pHeightMap, pFrustum, fLODRatio);
}
int QuadTree::GenerateMapIdx(VOID* pIb, TERRAIN_VTX* pHeightMap)
{
	//최상위 삼각형 2개만 찾으면 된다
	int nTris = 0;
	unsigned short* p = ((unsigned short*)pIb);
	*p++ = m_nCorner[0];	*p++ = m_nCorner[1];	*p++ = m_nCorner[2];	nTris++;
	*p++ = m_nCorner[2];	*p++ = m_nCorner[1];	*p++ = m_nCorner[3];	nTris++;

	return nTris;
}

void QuadTree::_FrustumCull(TERRAIN_VTX* pHeightMap, Frustum* pFrustum)
{
	int ret;

	ret = _IsInFrustum(pHeightMap, pFrustum);
	switch (ret)
	{
	case FRUSTUM_COMPLETELY_IN:	// 프러스텀에 완전포함, 하위노드 검색 필요없음
		m_bCulled = FALSE;
		return;
	case FRUSTUM_PARTIALLY_IN:		// 프러스텀에 일부포함, 하위노드 검색 필요함
		m_bCulled = FALSE;
		break;
	case FRUSTUM_OUT:				// 프러스텀에서 완전벗어남, 하위노드 검색 필요없음
		m_bCulled = TRUE;
		return;
	}
	if (m_pChild[0]) m_pChild[0]->_FrustumCull(pHeightMap, pFrustum);
	if (m_pChild[1]) m_pChild[1]->_FrustumCull(pHeightMap, pFrustum);
	if (m_pChild[2]) m_pChild[2]->_FrustumCull(pHeightMap, pFrustum);
	if (m_pChild[3]) m_pChild[3]->_FrustumCull(pHeightMap, pFrustum);
}

int QuadTree::_IsInFrustum(TERRAIN_VTX* pHeightMap, Frustum* pFrustum)
{

	//Mesh detail이 1인경우엔 m_nCenter가 정상적이지 않아서 중점계산 해야된다.
	D3DXVECTOR3* vec[4] = 
	{ 
		&((pHeightMap + m_nCorner[0])->p), 
		&((pHeightMap + m_nCorner[1])->p),
		&((pHeightMap + m_nCorner[2])->p), 
		&((pHeightMap + m_nCorner[3])->p) 
	};
	D3DXVECTOR3 center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++) 
	{
		center.x += vec[i]->x;
		center.y += vec[i]->y;
		center.z += vec[i]->z;
	}
	center.x /= 4.0f; center.y /= 4.0f; center.z /= 4.0f;

	BOOL bInSphere = pFrustum->IsInSphere(&center, m_fRadius);
	//BOOL bInSphere = pFrustum->IsInSphere((D3DXVECTOR3*)(pHeightMap + m_nCenter), m_fRadius);
	

	//원 통째로 밖에 나가있다 -> out
	if (!bInSphere) 
		return FRUSTUM_OUT;

	// 쿼드트리의 4정점 프러스텀 경계 테스트
	for (int i = 0; i < 4; i++)
	{
		//하나라도 밖에 있다 -> 부분 내부
		if (pFrustum->IsIn(vec[i]) == FALSE)
			return FRUSTUM_PARTIALLY_IN;
	}


	//모두내부
	return FRUSTUM_COMPLETELY_IN;


}


// 이웃노드를 만든다.(삼각형 찢어짐 방지용)
void QuadTree::_BuildNeighborNode(QuadTree* pRoot, TERRAIN_VTX* pHeightMap)
{
	int				n;
	int				_0, _1, _2, _3;

	for (int i = 0; i < 4; i++)
	{
		_0 = m_nCorner[0];
		_1 = m_nCorner[1];
		_2 = m_nCorner[2];
		_3 = m_nCorner[3];
		// 이웃노드의 4개 코너값을 얻는다.
		n = _GetNodeIndex(i, _0, _1, _2, _3);
		// 코너값으로 이웃노드의 포인터를 얻어온다.
		if (n >= 0) m_pNeighbor[i] = pRoot->_FindNode(pHeightMap, _0, _1, _2, _3);
	}

	// 자식노드로 재귀호출
	if (m_pChild[0])
	{
		m_pChild[0]->_BuildNeighborNode(pRoot, pHeightMap);
		m_pChild[1]->_BuildNeighborNode(pRoot, pHeightMap);
		m_pChild[2]->_BuildNeighborNode(pRoot, pHeightMap);
		m_pChild[3]->_BuildNeighborNode(pRoot, pHeightMap);
	}
}

// 쿼드트리를 검색해서 4개 코너값과 일치하는 노드를 찾는다.
QuadTree* QuadTree::_FindNode(TERRAIN_VTX* pHeightMap, int _0, int _1, int _2, int _3)
{
	QuadTree* p = NULL;
	// 일치하는 노드라면 노드값을 리턴
	if ((m_nCorner[0] == _0) && (m_nCorner[1] == _1) && (m_nCorner[2] == _2) && (m_nCorner[3] == _3))
		return this;

	// 자식 노드가 있가?
	if (m_pChild[0])
	{
		RECT	rc;
		POINT	pt;
		int n = (_0 + _1 + _2 + _3) / 4;

		// 현재 맵상에서의 위치
		pt.x = (int)pHeightMap[n].p.x;
		pt.y = (int)pHeightMap[n].p.z;

		// 4개의 코너값을 기준으로 자식노드의 맵 점유범위를 얻는다.
		SetRect(&rc, (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.z);
		// pt값이 점유범위안에 있다면 자식노드로 들어간다.
		if (MATH::IsInRect(&rc, pt))
			return m_pChild[0]->_FindNode(pHeightMap, _0, _1, _2, _3);

		SetRect(&rc, (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].p.z);
		if (MATH::IsInRect(&rc, pt))
			return m_pChild[1]->_FindNode(pHeightMap, _0, _1, _2, _3);

		SetRect(&rc, (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].p.z);
		if (MATH::IsInRect(&rc, pt))
			return m_pChild[2]->_FindNode(pHeightMap, _0, _1, _2, _3);

		SetRect(&rc, (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].p.z);
		if (MATH::IsInRect(&rc, pt))
			return m_pChild[3]->_FindNode(pHeightMap, _0, _1, _2, _3);
	}

	return NULL;
}

// 4개 방향(상단,하단,좌측,우측)의 이웃노드 인덱스를 구한다.
int	QuadTree::_GetNodeIndex(int ed, int& _0, int& _1, int& _2, int& _3)
{
	int		_a, _b, _c, _d, gap;
	_a = _0;
	_b = _1;
	_c = _2;
	_d = _3;
	gap = _b - _a;	// 현재 노드의 좌우폭값

	switch (ed)
	{
	case EDGE_UP:	// 위쪽 방향 이웃노드의 인덱스
		_0 = _a - m_x * gap;
		_1 = _b - m_x * gap;
		_2 = _a;
		_3 = _b;
		break;
	case EDGE_DN:	// 아래 방향 이웃노드의 인덱스
		_0 = _c;
		_1 = _d;
		_2 = _c + m_x * gap;
		_3 = _d + m_x * gap;
		break;
	case EDGE_LT:	// 좌측 방향 이웃노드의 인덱스
		_0 = _a - gap;
		_1 = _a;
		_2 = _c - gap;
		_3 = _c;
		break;
	case EDGE_RT:	// 우측 방향 이웃노드의 인덱스
		_0 = _b;
		_1 = _b + gap;
		_2 = _d;
		_3 = _d + gap;
		break;
	}

	float n = (_0 + _1 + _2 + _3) / 4.0f;	// 가운데 인덱스

	//맵 벗어나는거나 유효숫자 아니면
	if (!MATH::IsInRange(n, 0, m_x * m_y - 1) || (n - (int)n != 0))
		return -1;


	return (int)n;
}

// 쿼드트리를 만든다.(Build()함수에서 불린다)
BOOL	QuadTree::_BuildQuadTree(TERRAIN_VTX* pHeightMap)
{
	if (_SubDivide())
	{
		D3DXVECTOR3 v = (pHeightMap + m_nCorner[CORNER_TL])->p -
			(pHeightMap + m_nCorner[CORNER_BR])->p;
		(pHeightMap + m_nCenter)->p;

		m_fRadius = D3DXVec3Length(&v) / 2.0f;
		m_pChild[CORNER_TL]->_BuildQuadTree(pHeightMap);
		m_pChild[CORNER_TR]->_BuildQuadTree(pHeightMap);
		m_pChild[CORNER_BL]->_BuildQuadTree(pHeightMap);
		m_pChild[CORNER_BR]->_BuildQuadTree(pHeightMap);
	}
	return TRUE;
}

VOID QuadTree::SearchInTree(Ray ray, float& dist, D3DXVECTOR3 pos[3], TERRAIN_VTX* pHeightMap)
{
	if (m_bCulled)
	{
		return;
	}
	float u, v, buf_dist;
	
	//Cull도 안되있는데 visible한 idx없다면 하위노드 탐색하자
	if (m_VisibleIdx.size() == 0) 
	{

		//현재 쿼드트리 대각선 절반으로 잘라서 맞는곳 찾기
		
		//왼위 삼각형
		if (D3DXIntersectTri(&pHeightMap[m_nCorner[CORNER_TL]].p, &pHeightMap[m_nCorner[CORNER_TR]].p,
			&pHeightMap[m_nCorner[CORNER_BL]].p, ray.GetPos(), ray.GetDir(), &u, &v, &buf_dist))
		{
			if (m_pChild[CORNER_TL]) { m_pChild[CORNER_TL]->SearchInTree(ray, dist, pos, pHeightMap); }
			if (m_pChild[CORNER_TR]) { m_pChild[CORNER_TR]->SearchInTree(ray, dist, pos, pHeightMap); }
			if (m_pChild[CORNER_BL]) { m_pChild[CORNER_BL]->SearchInTree(ray, dist, pos, pHeightMap); }
		}
		//오른아래 삼각형
		else if (D3DXIntersectTri(&pHeightMap[m_nCorner[CORNER_TR]].p, &pHeightMap[m_nCorner[CORNER_BL]].p,
			&pHeightMap[m_nCorner[CORNER_BR]].p, ray.GetPos(), ray.GetDir(), &u, &v, &buf_dist))
		{
			if (m_pChild[CORNER_TR]) { m_pChild[CORNER_TR]->SearchInTree(ray, dist, pos, pHeightMap); }
			if (m_pChild[CORNER_BL]) { m_pChild[CORNER_BL]->SearchInTree(ray, dist, pos, pHeightMap); }
			if (m_pChild[CORNER_BR]) { m_pChild[CORNER_BR]->SearchInTree(ray, dist, pos, pHeightMap); }
		}
	}
	//가시화중인 삼각형 있다는것! 
	else
	{
		//printf("%d\n", m_VisibleIdx.size());
		for (int i = 0; i < m_VisibleIdx.size(); i++)
		{
			TRI_IDX now = m_VisibleIdx[i];
			//printf("%d %d %d\n", now._0, now._1, now._2);
			D3DXVECTOR3* v0 = &pHeightMap[now._0].p;
			D3DXVECTOR3* v1 = &pHeightMap[now._1].p;
			D3DXVECTOR3* v2 = &pHeightMap[now._2].p;
			
			
			
			if (D3DXIntersectTri(v0, v1, v2, ray.GetPos(), ray.GetDir(), &u, &v, &buf_dist))
			{
				//printf("%.3f\n", buf_dist);
				//값 갱신 - dist와 pos
				if (0 < buf_dist && buf_dist < dist) 
				{
				/*	printf("%f\n", buf_dist);
					printf("%d %.3f %.3f %f\n", now._0, v0->x, v0->y, v0->z);
					printf("%d %.3f %.3f %f\n", now._1, v1->x, v1->y, v1->z);
					printf("%d %.3f %.3f %f\n\n", now._2, v2->x, v2->y, v2->z);*/
					//printf("찾음\n");
					pos[0] = *v0;	pos[1] = *v1;	pos[2] = *v2;	dist = buf_dist;
				}
			}
		}

	}
	return;
}