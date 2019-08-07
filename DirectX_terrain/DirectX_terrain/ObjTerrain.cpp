#include "ObjTerrain.h"
#include "Dib.h"

ObjTerrain::ObjTerrain() {

	m_mapscale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}
ObjTerrain::~ObjTerrain() {
	for (unsigned int i = 0; i < m_vTex.size(); i++)
		m_vTex[i]->Release();
	delete m_pQuadTree;
	delete m_pHeightMap;
}

HRESULT ObjTerrain::Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3* scale, float fLODRatio,
	string heightmap_dir, vector<string> vTexture_dir)
{
	m_device = device;
	m_mapscale = *scale;

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

HRESULT ObjTerrain::_CreateHeightMap(string fileName)
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
			v.p.x = (float)((x - m_x / 2) * m_mapscale.x);
			v.p.z = -(float)((z - m_z / 2) * m_mapscale.z);

			v.p.y = (float)(*(DIB_DATAXY_INV(pDIB, x, z))) * m_mapscale.y;
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
HRESULT ObjTerrain::_CreateTexture(vector<string> vFileName)
{
	//****�̰� �ȵɼ��� ���� ����� 1������ ����. 
	for (unsigned int i = 0; i < vFileName.size(); i++) {
		LPDIRECT3DTEXTURE9 buf;
		D3DXCreateTextureFromFile(m_device, (LPCSTR)(vFileName[i].c_str()), &buf);
		m_vTex.push_back(buf);
	}

	return S_OK;
}
HRESULT ObjTerrain::_CreateVIB()
{
	VOID* pVertices;
	// VB����
	if (FAILED(m_device->CreateVertexBuffer(m_x * m_z * sizeof(TERRAIN_VTX),
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
	if (FAILED(m_device->CreateIndexBuffer((m_x - 1) * (m_z - 1) * 2 * sizeof(TRI_IDX),
		0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL)))
	{
		return E_FAIL;
	}


	return S_OK;
}
HRESULT ObjTerrain::_CreateQuadTree()
{
	m_pQuadTree->Create(m_pHeightMap);
	return S_OK;
}
HRESULT ObjTerrain::_Render()
{
	D3DXMatrixIdentity(&m_matWorld);
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	//m_device->SetTexture(0, m_vTex[0]);								// 0�� �ؽ��� ���������� �ؽ��� ����(�����)
	//m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	//m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);		// 0�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���

	//m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_device->SetTexture(0, m_vTex[0]);								// 0�� �ؽ��� ���������� �ؽ��� ����(�����)
	m_device->SetTexture(1, m_vTex[1]);								// 1�� �ؽ��� ���������� �ؽ��� ����(������)
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	m_device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);		// 0�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���
	m_device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);		// 1�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���

	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);		// MODULATE�� ���´�.
	m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);		// �ؽ�ó
	m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);		// ������
	m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);	// MODULATE2�� ���´�.
	m_device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);		// �ؽ�ó
	m_device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);		// �����


	m_device->SetStreamSource(0, m_pVB, 0, sizeof(TERRAIN_VTX));
	m_device->SetFVF(TERRAIN_VTX::FVF);
	m_device->SetIndices(m_pIB);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_x * m_z, 0, m_nTriangles);

	m_device->SetTexture(0, NULL);
	m_device->SetTexture(1, NULL);
	return S_OK;
}
HRESULT	ObjTerrain::DrawMain(ObjFrustum* pObjFrustum)
{
	VOID* pIndices;
	if (FAILED(m_pIB->Lock(0, (m_x - 1) * (m_z - 1) * 2 * sizeof(TRI_IDX), (void**)& pIndices, 0)))
		return E_FAIL;
	//���� Ʈ������ �ε��� ä���
	m_nTriangles = m_pQuadTree->GenerateIndex(pIndices, m_pHeightMap, pObjFrustum, m_fLODRatio);

	m_pIB->Unlock();
	_SetMaterial();
	_Render();

	return S_OK;
}

HRESULT ObjTerrain::DrawMap()
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

HRESULT	ObjTerrain::_SetMaterial()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}

VOID ObjTerrain::MeshPicking(Ray ray, float& dist, D3DXVECTOR3 pos[3])
{
	//Terrian�� QuadTree�־ �̰� �̿��ϸ� ȿ�����ϵ�
	//���⼭ Object�� Transform�ִٸ� ray�� pos�� dir�� ����ȯ ������ -> ���� ��ǥ��
	//������ �׷� �ʿ䰡 ����

	// �ǻ��ڵ�: Transform(ray, ray, worldMatInverse);

	m_pQuadTree->SearchInTree(ray, dist, pos, m_pHeightMap);


}


float ObjTerrain::GetHeight(D3DXVECTOR3* center, D3DXVECTOR3 pos[3])
{
	float x = center->x + m_x / 2;
	float z = (center->z * -1) + m_z / 2;


	int id_x[4] = { (int)x, (int)x + 1, (int)x, (int)x + 1 };
	int id_z[4] = { (int)z, (int)z, (int)z + 1, (int)z + 1 };

	D3DXVECTOR3* vt[4] =
	{
		&m_pHeightMap[id_x[0] + id_z[0] * m_x].p,	&m_pHeightMap[id_x[1] + id_z[1] * m_x].p,
		&m_pHeightMap[id_x[2] + id_z[2] * m_x].p,	&m_pHeightMap[id_x[3] + id_z[3] * m_x].p
	};

	float dx = x - (int)x;
	float dz = z - (int)z;

	float height;

	//���� �ﰢ�� �׸���
	if (dz < 1.0f - dx)
	{
		float uy = vt[1]->y - vt[0]->y;
		float vy = vt[2]->y - vt[0]->y;

		height = vt[0]->y + MATH::Lerp(0.0f, uy, dx) + MATH::Lerp(0.0f, vy, dz);
		pos[0] = *vt[0];	pos[1] = *vt[1];	pos[2] = *vt[2];
	}
	else
	{
		float uy = vt[2]->y - vt[3]->y;
		float vy = vt[1]->y - vt[3]->y;

		height = vt[3]->y + MATH::Lerp(0.0f, uy, 1.0f - dx) + MATH::Lerp(0.0f, vy, 1.0f - dz);
		pos[0] = *vt[2];	pos[1] = *vt[1];	pos[2] = *vt[3];
	}

	//float A = 
	return height;
}