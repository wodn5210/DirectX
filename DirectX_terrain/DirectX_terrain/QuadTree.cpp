#include "QuadTree.h"



QuadTree::QuadTree(int cx, int cy)
{
	int		i;
	m_pParent = NULL;
	m_nCenter = 0;
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

	// ��ܺ� ���
	nTopEdgeCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
	// �ϴܺ� ��� 
	nBottomEdgeCenter = (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
	// ������ ���
	nLeftEdgeCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
	// ������ ���
	nRightEdgeCenter = (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
	// �Ѱ��
	nCentralPoint = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] +
		m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 4;

	// ���̻� ������ �Ұ����Ѱ�? �׷��ٸ� SubDivide() ����
	if ((m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1)
	{
		return FALSE;
	}

	// 4���� �ڽĳ�� �߰�
	m_pChild[CORNER_TL] = _AddChild(m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint);
	m_pChild[CORNER_TR] = _AddChild(nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter);
	m_pChild[CORNER_BL] = _AddChild(nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter);
	m_pChild[CORNER_BR] = _AddChild(nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR]);

	return TRUE;
}

int	QuadTree::_GenTriIndex(int nTris, VOID* pIndex, 
	TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio)
{
	// �ø��� ����� �׳� ����
	if (m_bCulled)
	{
		m_bCulled = FALSE;

		return nTris;
	}


	if (_IsVisible(pHeightMap, pFrustum->GetPos(), fLODRatio))
	{
		unsigned short* p = ((unsigned short*)pIndex) + nTris * 3;


		*p++ = m_nCorner[0];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[2];
		nTris++;
		*p++ = m_nCorner[2];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[3];
		nTris++;

		return nTris;
	}


	if (m_pChild[CORNER_TL]) nTris = m_pChild[CORNER_TL]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
	if (m_pChild[CORNER_TR]) nTris = m_pChild[CORNER_TR]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
	if (m_pChild[CORNER_BL]) nTris = m_pChild[CORNER_BL]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
	if (m_pChild[CORNER_BR]) nTris = m_pChild[CORNER_BR]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);

	return nTris;
}

BOOL QuadTree::Create(TERRAIN_VTX* pHeightMap)
{	// ����Ʈ�� ����
	_BuildQuadTree(pHeightMap);
	// �̿���� ����
	//_BuildNeighborNode(this, pHeightMap, m_nCorner[CORNER_TR] + 1);

	return TRUE;
}

int QuadTree::GenerateIndex(VOID* pIb, TERRAIN_VTX* pHeightMap, Frustum* pFrustum, float fLODRatio)
{
	_FrustumCull(pHeightMap, pFrustum);
	return _GenTriIndex(0, pIb, pHeightMap, pFrustum, fLODRatio);
}

void QuadTree::_FrustumCull(TERRAIN_VTX* pHeightMap, Frustum* pFrustum)
{
	int ret;

	ret = _IsInFrustum(pHeightMap, pFrustum);
	switch (ret)
	{
	case FRUSTUM_COMPLETELY_IN:	// �������ҿ� ��������, ������� �˻� �ʿ����
		m_bCulled = FALSE;
		return;
	case FRUSTUM_PARTIALLY_IN:		// �������ҿ� �Ϻ�����, ������� �˻� �ʿ���
		m_bCulled = FALSE;
		break;
	case FRUSTUM_OUT:				// �������ҿ��� �������, ������� �˻� �ʿ����
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

	//Mesh detail�� 1�ΰ�쿣 m_nCenter�� ���������� �ʾƼ� ������� �ؾߵȴ�.
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
	

	//�� ��°�� �ۿ� �����ִ� -> out
	if (!bInSphere) 
		return FRUSTUM_OUT;

	// ����Ʈ���� 4���� �������� ��� �׽�Ʈ
	BOOL bVtx[4];
	for (int i = 0; i < 4; i++)
	{
		//bVtx[i] = pFrustum->IsIn(vec[i]);

		//�ϳ��� �ۿ� �ִ� -> �κ� ����
		if (pFrustum->IsIn(vec[i]) == FALSE)
			return FRUSTUM_PARTIALLY_IN;
	}


	//��γ���
	return FRUSTUM_COMPLETELY_IN;


}


// �̿���带 �����.(�ﰢ�� ������ ������)
void QuadTree::_BuildNeighborNode(QuadTree* pRoot, TERRAIN_VTX* pHeightMap, int cx)
{
	int				n;
	int				_0, _1, _2, _3;

	for (int i = 0; i < 4; i++)
	{
		_0 = m_nCorner[0];
		_1 = m_nCorner[1];
		_2 = m_nCorner[2];
		_3 = m_nCorner[3];
		// �̿������ 4�� �ڳʰ��� ��´�.
		n = _GetNodeIndex(i, cx, _0, _1, _2, _3);
		// �ڳʰ����� �̿������ �����͸� ���´�.
		if (n >= 0) m_pNeighbor[i] = pRoot->_FindNode(pHeightMap, _0, _1, _2, _3);
	}

	// �ڽĳ��� ���ȣ��
	if (m_pChild[0])
	{
		m_pChild[0]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[1]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[2]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[3]->_BuildNeighborNode(pRoot, pHeightMap, cx);
	}
}

// ����Ʈ���� �˻��ؼ� 4�� �ڳʰ��� ��ġ�ϴ� ��带 ã�´�.
QuadTree* QuadTree::_FindNode(TERRAIN_VTX* pHeightMap, int _0, int _1, int _2, int _3)
{
	QuadTree* p = NULL;
	// ��ġ�ϴ� ����� ��尪�� ����
	if ((m_nCorner[0] == _0) && (m_nCorner[1] == _1) && (m_nCorner[2] == _2) && (m_nCorner[3] == _3))
		return this;

	// �ڽ� ��尡 �ְ�?
	if (m_pChild[0])
	{
		RECT	rc;
		POINT	pt;
		int n = (_0 + _1 + _2 + _3) / 4;

		// ���� �ʻ󿡼��� ��ġ
		pt.x = (int)pHeightMap[n].p.x;
		pt.y = (int)pHeightMap[n].p.z;

		// 4���� �ڳʰ��� �������� �ڽĳ���� �� ���������� ��´�.
		SetRect(&rc, (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.z);
		// pt���� ���������ȿ� �ִٸ� �ڽĳ��� ����.
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

// 4�� ����(���,�ϴ�,����,����)�� �̿���� �ε����� ���Ѵ�.
int	QuadTree::_GetNodeIndex(int ed, int cx, int& _0, int& _1, int& _2, int& _3)
{
	int		_a, _b, _c, _d, gap;
	_a = _0;
	_b = _1;
	_c = _2;
	_d = _3;
	gap = _b - _a;	// ���� ����� �¿�����

	switch (ed)
	{
	case EDGE_UP:	// ���� ���� �̿������ �ε���
		_0 = _a - cx * gap;
		_1 = _b - cx * gap;
		_2 = _a;
		_3 = _b;
		break;
	case EDGE_DN:	// �Ʒ� ���� �̿������ �ε���
		_0 = _c;
		_1 = _d;
		_2 = _c + cx * gap;
		_3 = _d + cx * gap;
		break;
	case EDGE_LT:	// ���� ���� �̿������ �ε���
		_0 = _a - gap;
		_1 = _a;
		_2 = _c - gap;
		_3 = _c;
		break;
	case EDGE_RT:	// ���� ���� �̿������ �ε���
		_0 = _b;
		_1 = _b + gap;
		_2 = _d;
		_3 = _d + gap;
		break;
	}

	float n = (_0 + _1 + _2 + _3) / 4.0f;	// ��� �ε���
	if (!MATH::IsInRange(n, 0, cx * cx - 1))
		return -1;

	return n;
}

// ����Ʈ���� �����.(Build()�Լ����� �Ҹ���)
BOOL	QuadTree::_BuildQuadTree(TERRAIN_VTX* pHeightMap)
{
	if (_SubDivide())
	{
		D3DXVECTOR3 v = (pHeightMap + m_nCorner[CORNER_TL])->p -
			(pHeightMap + m_nCorner[CORNER_BR])->p;
		(pHeightMap + m_nCenter)->p;

		m_fRadius = D3DXVec3Length(&v) / 2.0f;
		m_pChild[CORNER_TL]->Create(pHeightMap);
		m_pChild[CORNER_TR]->Create(pHeightMap);
		m_pChild[CORNER_BL]->Create(pHeightMap);
		m_pChild[CORNER_BR]->Create(pHeightMap);
	}
	return TRUE;
}