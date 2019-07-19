#pragma once
#include <d3dx9.h>

/*
지형처리 효율적 쿼드트리
*/
class QuadTree
{
	enum		CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
private:
	QuadTree* m_pChild[4];
	int m_nCenter;			//중앙 Index
	int m_nCorner[4];		//코너들의 Index
	

public:
	QuadTree(int cx, int cy);
	QuadTree(QuadTree* pParent);
	~QuadTree();

	BOOL Create();
	int GenerateIndex(VOID* pIb);

private:
	BOOL _SetCorners(int TL, int TR, int BL, int BR);
	QuadTree* _AddChild(int TL, int TR, int BL, int BR);
	BOOL _SubDivide();
	BOOL _IsVisible() { return (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1); }
	int	_GenTriIndex(int nTris, LPVOID pIndex);
};