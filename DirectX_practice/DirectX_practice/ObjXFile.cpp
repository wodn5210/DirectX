#include "ObjXFile.h"

ObjXFile::ObjXFile()
{

}
ObjXFile::~ObjXFile()
{
	if (g_pMeshMaterials != NULL)
		delete[] g_pMeshMaterials;

	if (g_pMeshTextures)
	{
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			if (g_pMeshTextures[i])
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	if (g_pMesh != NULL)
		g_pMesh->Release();
}


void ObjXFile::DrawObj(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	D3DXMatrixScaling(&scale, 70, 70, 70);
	matWorld = scale;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		//�κ����� �޽��� ������ �ؽ��� ����
		g_pd3dDevice->SetMaterial(&g_pMeshMaterials[i]);
		g_pd3dDevice->SetTexture(0, g_pMeshTextures[i]);

		//�κ����� �޽� ���
		g_pMesh->DrawSubset(i);
	}
}
HRESULT ObjXFile::Create(LPDIRECT3DDEVICE9 g_pd3dDevice, D3DXVECTOR3 center)
{
	//���� �ӽ� ���� ����
	LPD3DXBUFFER pD3DXMtrlBuffer;

	//tiger.x���� �޽������� �������� �д´�
	if (FAILED(D3DXLoadMeshFromX(L"./src/Tiger.x", D3DXMESH_SYSTEMMEM,
		g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh)))
	{
		MessageBox(NULL, L"Could not find Tiger.x", L"Mesh.exe", MB_OK);
		return E_FAIL;
	}

	//���������� �ؽ��� ������ ���� �̾Ƴ���.
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	//���� ������ŭ ���� ����ü �迭 ����
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	//���� ������ŭ �ؽ��� �迭 ����
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;					//���� ���� ����
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;		//�ֺ����� ������ Diffuse ������
		g_pMeshTextures[i] = NULL;

		if (d3dxMaterials[i].pTextureFilename != NULL && lstrlen((LPCWSTR)d3dxMaterials[i].pTextureFilename) > 0)
		{
			//�ؽ��ĸ� ���Ͽ��� �ε��Ѵ�
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
				L"./src/Tigerr.bmp", &g_pMeshTextures[i])))
			{
				MessageBox(NULL, L"Could not find texture map", L"Mesh.exe", MB_OK);
				return E_FAIL;
			}
		}
	}

	//�ӽû��� �������� ����
	pD3DXMtrlBuffer->Release();

	return S_OK;
}
