#include "Terrain.h"
#include "Dib.h"

Terrain::Terrain() {
	center[0] = center[1] = center[2] = 0;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}
Terrain::~Terrain() {
	for (unsigned int i = 0; i < m_vTex.size(); i++)
		m_vTex[i]->Release();
	delete m_pQuadTree;
	delete m_pHeightMap;
}

HRESULT Terrain::Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3* scale, float fLODRatio,
	string heightmap_dir, vector<string> vTexture_dir)
{
	m_pd3dDevice = device;
	m_scale = *scale;

	m_fLODRatio = fLODRatio;

	if (FAILED(_CreateHeightMap(heightmap_dir)))
	{
		return E_FAIL;
	}
	if (FAILED(_CreateTexture(vTexture_dir)))
	{
		return E_FAIL;
	}
	if (FAILED(_CreateVIB()))
	{
		return E_FAIL;
	}
	m_pQuadTree = new QuadTree(m_x, m_z);
	if (FAILED(_CreateQuadTree()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Terrain::_CreateHeightMap(string fileName)
{
	int		n;
	LPBYTE	pDIB = DibLoadHandle((LPSTR)fileName.c_str());
	if (!pDIB) return E_FAIL;

	m_x = DIB_CX(pDIB);
	m_z = DIB_CY(pDIB);


	// m_x�� m_z�� (2^n+1)�� �ƴѰ�� E_FAIL�� ��ȯ
	n = MATH::Log2(m_x);
	if ((MATH::Pow2(n) + 1) != m_x) return E_FAIL;
	n = MATH::Log2(m_z);
	if ((MATH::Pow2(n) + 1) != m_z) return E_FAIL;


	m_pHeightMap = new TERRAIN_VTX[m_x * m_z];

	TERRAIN_VTX v;
	for (int z = 0; z < m_z; z++)
	{
		for (int x = 0; x < m_x; x++)
		{
			v.p.x = (float)((x - m_x / 2) * m_scale.x);
			v.p.z = -(float)((z - m_z / 2) * m_scale.z);

			v.p.y = (float)(*(DIB_DATAXY_INV(pDIB, x, z))) * m_scale.y;
			//m_y �ִ����
			if (m_y < v.p.y)
				m_y = v.p.y;
			D3DXVec3Normalize(&v.n, &v.p);
			v.t.x = (float)x / (float)(m_x - 1);
			v.t.y = (float)z / (float)(m_z - 1);
			m_pHeightMap[x + z * m_z] = v;
		}
	}

	DibDeleteHandle(pDIB);

	return S_OK;
}
HRESULT Terrain::_CreateTexture(vector<string> vFileName)
{
	//****�̰� �ȵɼ��� ���� ����� 1������ ����. 
	for (unsigned int i = 0; i < vFileName.size(); i++) {
		LPDIRECT3DTEXTURE9 buf;
		D3DXCreateTextureFromFile(m_pd3dDevice, (LPCSTR)(vFileName[i].c_str()), &buf);
		m_vTex.push_back(buf);
	}

	return S_OK;
}
HRESULT Terrain::_CreateVIB()
{
	VOID* pVertices;
	// VB����
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(m_x * m_z * sizeof(TERRAIN_VTX),
		0, TERRAIN_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVB->Lock(0, m_x * m_z * sizeof(TERRAIN_VTX), (void**)& pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, m_pHeightMap, m_x * m_z * sizeof(TERRAIN_VTX));
	m_pVB->Unlock();

	// IB����
	if (FAILED(m_pd3dDevice->CreateIndexBuffer((m_x - 1) * (m_z - 1) * 2 * sizeof(TRI_IDX),
		0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL)))
	{
		return E_FAIL;
	}


	return S_OK;
}
HRESULT Terrain::_CreateQuadTree()
{
	m_pQuadTree->Create(m_pHeightMap);
	return S_OK;
}
HRESULT Terrain::_Render()
{
	//m_pd3dDevice->SetTexture(0, m_vTex[0]);								// 0�� �ؽ��� ���������� �ؽ��� ����(�����)
	//m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);		// 0�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���

	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pd3dDevice->SetTexture(0, m_vTex[0]);								// 0�� �ؽ��� ���������� �ؽ��� ����(�����)
	m_pd3dDevice->SetTexture(1, m_vTex[1]);								// 1�� �ؽ��� ���������� �ؽ��� ����(������)
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);		// 0�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);		// 1�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);		// MODULATE�� ���´�.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);		// �ؽ�ó
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);		// ������
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);	// MODULATE2�� ���´�.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);		// �ؽ�ó
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);		// �����


	m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(TERRAIN_VTX));
	m_pd3dDevice->SetFVF(TERRAIN_VTX::FVF);
	m_pd3dDevice->SetIndices(m_pIB);
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_x * m_z, 0, m_nTriangles);

	m_pd3dDevice->SetTexture(0, NULL);
	return S_OK;
}
HRESULT	Terrain::DrawMain(Frustum* pFrustum)
{
	VOID* pIndices;
	if (FAILED(m_pIB->Lock(0, (m_x - 1) * (m_z - 1) * 2 * sizeof(TRI_IDX), (void**)& pIndices, 0)))
		return E_FAIL;
	//���� Ʈ������ �ε��� ä���
	m_nTriangles = m_pQuadTree->GenerateIndex(pIndices, m_pHeightMap, pFrustum, m_fLODRatio);

	m_pIB->Unlock();
	_SetMaterial();
	_Render();

	return S_OK;
}

HRESULT Terrain::DrawMap()
{
	VOID* pIndices;
	if (FAILED(m_pIB->Lock(0, (m_x - 1) * (m_z - 1) * 2 * sizeof(TRI_IDX), (void**)& pIndices, 0)))
		return E_FAIL;
	//���� Ʈ������ �ε��� ä���
	m_nTriangles = m_pQuadTree->GenerateMapIdx(pIndices, m_pHeightMap);

	m_pIB->Unlock();
	_SetMaterial();
	_Render();
	return S_OK;
}

HRESULT	Terrain::_SetMaterial()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.0f;
	m_pd3dDevice->SetMaterial(&mtrl);
	return S_OK;
}

VOID Terrain::MeshPicking(Ray ray, float& dist, D3DXVECTOR3 pos[3])
{
	//Terrian�� QuadTree�־ �̰� �̿��ϸ� ȿ�����ϵ�
	//���⼭ Object�� Transform�ִٸ� ray�� pos�� dir�� ����ȯ ������ -> ���� ��ǥ��
	//������ �׷� �ʿ䰡 ����

	// �ǻ��ڵ�: Transform(ray, ray, worldMatInverse);

	m_pQuadTree->SearchInTree(ray, dist, pos, m_pHeightMap);


}

BOOL Terrain::IsBallCollision(D3DXVECTOR3* center, float r)
{
	//intersectTri�� �Ÿ����� ������ִϱ� pos[3]�� ������ �ǰ� dist < r�� �ϸ� �ɵ�
	WORD dx[3] = { -1, 0, 1 };
	WORD dy[3] = { -1, 0, 1 };
	WORD dz[3] = { -1, 0, 1 };

	vector<D3DXVECTOR3> dir;
	for (int z = 0; z < 3; z++)
	{
		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < 3; x++)
			{
				//direction�� 0, 0, 0�� �Ұ���
				if (z == 1 && y == 1 && x == 1)
					continue;

				D3DXVECTOR3 buf = D3DXVECTOR3(dx[x], dy[y], dz[z]);
				D3DXVec3Normalize(&buf, &buf);
				dir.push_back(buf);
			}
		}
	}

	int x = (int)center->x + m_x / 2;
	int y = (int)(center->z * -1) + m_z / 2;
	int _0 = x + y * m_z;

	//�� Index �� ������
	int idx[4][4];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			idx[i][j] = _0 + (j - 1) + (m_x) * (i - 1);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			D3DXVECTOR3* vt[4] =
			{ 
				&m_pHeightMap[idx[i][j]].p,		&m_pHeightMap[idx[i][j + 1]].p,
				&m_pHeightMap[idx[i + 1][j]].p,	&m_pHeightMap[idx[i + 1][j + 1]].p 
			};

			for (int k = 0; k < dir.size(); k++) {
				Ray ray;
				ray.Create(m_pd3dDevice, center, &dir[k]);
				float u, v, dist = FLT_MAX;
				if (D3DXIntersectTri(vt[0], vt[1], vt[2], ray.GetPos(), ray.GetDir(), &u, &v, &dist))
				{
					//�� ���� - dist�� pos
					if (0 < dist && dist < r)
					{
						printf("dd");
						return false;
					}
				}

				dist = FLT_MAX;
				if (D3DXIntersectTri(vt[2], vt[1], vt[3], ray.GetPos(), ray.GetDir(), &u, &v, &dist))
				{
					//�� ���� - dist�� pos
					if (0 < dist && dist < r)
					{
						printf("dd");
						return false;
					}
				}
			}
			
		}
	}



	/*QuadTree* now = m_pQuadTree->FindNode(m_pHeightMap, _0, _1, _2, _3);
	if (now != NULL)
	{
		printf("dd\n");
	}*/
	//TL, TL+1�� ��� ã�� �ֺ� �̿� 5�� ã�� (8���⿡�� �ڿ��� ����)
	//8���⿡ ���ؼ� 27���� ���⺤�ͷ�

	return true;
}