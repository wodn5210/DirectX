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
���� Ŭ����
���̸��� �̿��ؼ� ��������
*/



class ObjTerrain
	:public ObjGraphic
{
private:
	//������ �ε����� ����ִ� ���� ���۹���
	//�� data type���� �� ��ȯ�� �� ����ؼ� �����ؾ��Ѵ�

	int m_x, m_z;		// ���̸��� x, y, z(y�� �ִ���̰�) ũ��
	float m_y;
	D3DXVECTOR3 m_mapscale;		// x, y, z�� scale
	TERRAIN_VTX* m_pHeightMap;

	vector<LPDIRECT3DTEXTURE9> m_vTex;
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	int m_nTriangles;	// ����ȭ �� �ﰢ�� ��



	QuadTree* m_pQuadTree;	
	ObjFrustum* m_pFrustum;

public:
	ObjTerrain();
	~ObjTerrain();

	HRESULT Create(LPDIRECT3DDEVICE9 device, ObjFrustum* pFrustum, D3DXVECTOR3* scale,
		string heightmap_dir, vector<string> texture_dir);
	HRESULT		DrawMain();
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

