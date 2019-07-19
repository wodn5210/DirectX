#pragma once
#include <d3dx9.h>

/*
����ó�� ȿ���� ����Ʈ��
*/
class QuadTree
{
	enum		CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
private:
	QuadTree* m_pChild[4];
	int m_nCenter;			//�߾� Index
	int m_nCorner[4];		//�ڳʵ��� Index
	

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