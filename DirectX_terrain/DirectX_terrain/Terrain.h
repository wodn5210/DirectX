#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>
#include "QuadTree.h"
#include "Frustum.h"

#pragma once
#include "define.h"
using namespace std;

/*
���� Ŭ����
���̸��� �̿��ؼ� ��������
*/



class Terrain
{
private:
	int center[3];			// ������ ��ġ�� ����
	int m_x, m_z;		// ���̸��� x, y, z(z�� �ִ���̰�) ũ��
	float m_y;
	D3DXVECTOR3 m_scale;		// x, y, z�� scale
	TERRAIN_VTX* m_pHeightMap;
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	vector<LPDIRECT3DTEXTURE9> m_vTex;
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	int m_nTriangles;	// ����ȭ �� �ﰢ�� ��
	float m_fLODRatio;	// LOD ������


	QuadTree* m_pQuadTree;	

public:
	Terrain();
	~Terrain();

	HRESULT Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3* scale, float fLODRatio,
		string heightmap_dir, vector<string> texture_dir);
	HRESULT		Draw(Frustum* pFrustum);


private:
	HRESULT _CreateHeightMap(string fileName);
	HRESULT _CreateTexture(vector<string> vFileName);
	HRESULT _CreateVIB();
	HRESULT _CreateQuadTree();
	HRESULT _Render();
};

