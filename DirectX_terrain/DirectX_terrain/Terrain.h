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
지형 클래스
높이맵을 이용해서 구현하자
*/



class Terrain
{
private:
	int center[3];			// 지형을 설치할 중점
	int m_x, m_z;		// 높이맵의 x, y, z(z는 최대높이값) 크기
	float m_y;
	D3DXVECTOR3 m_scale;		// x, y, z축 scale
	TERRAIN_VTX* m_pHeightMap;
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	vector<LPDIRECT3DTEXTURE9> m_vTex;
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	int m_nTriangles;	// 가시화 할 삼각형 수
	float m_fLODRatio;	// LOD 보정값


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

