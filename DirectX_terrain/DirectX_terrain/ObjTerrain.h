#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>

#include "ObjGraphic.h"
#include "QuadTree.h"
#include "ObjFrustum.h"
#include "Ray.h"

#include "define.h"
using namespace std;

/*
지형 클래스
높이맵을 이용해서 구현하자
*/



class ObjTerrain
	:public ObjGraphic
{
private:
	//정점과 인덱스가 들어있는 변수 시작번지
	//각 data type으로 형 변환을 잘 고려해서 접근해야한다

	int m_x, m_z;		// 높이맵의 x, y, z(z는 최대높이값) 크기
	float m_y;
	D3DXVECTOR3 m_mapscale;		// x, y, z축 scale
	TERRAIN_VTX* m_pHeightMap;
	//LPDIRECT3DDEVICE9		m_pd3dDevice;
	vector<LPDIRECT3DTEXTURE9> m_vTex;
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	int m_nTriangles;	// 가시화 할 삼각형 수
	float m_fLODRatio;	// LOD 보정값


	QuadTree* m_pQuadTree;	

public:
	ObjTerrain();
	~ObjTerrain();

	HRESULT Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3* scale, float fLODRatio,
		string heightmap_dir, vector<string> texture_dir);
	HRESULT		DrawMain(ObjFrustum* pObjFrustum);
	HRESULT		DrawMap();

	VOID MeshPicking(Ray ray, float& dist, D3DXVECTOR3 pos[3]);

	float GetHeight(D3DXVECTOR3* center, D3DXVECTOR3 pos[3]);
	int GetX() { return m_x; };
	int GetZ() { return m_z; };
private:
	HRESULT _CreateHeightMap(string fileName);
	HRESULT _CreateTexture(vector<string> vFileName);
	HRESULT _CreateVIB();
	HRESULT _CreateQuadTree();
	HRESULT _Render();
	HRESULT _SetMaterial();
};

