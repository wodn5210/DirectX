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
		//부분집합 메시의 재질과 텍스쳐 설정
		g_pd3dDevice->SetMaterial(&g_pMeshMaterials[i]);
		g_pd3dDevice->SetTexture(0, g_pMeshTextures[i]);

		//부분집합 메시 출력
		g_pMesh->DrawSubset(i);
	}
}
HRESULT ObjXFile::Create(LPDIRECT3DDEVICE9 g_pd3dDevice, D3DXVECTOR3 center)
{
	//재질 임시 보관 버퍼
	LPD3DXBUFFER pD3DXMtrlBuffer;

	//tiger.x에서 메쉬정보와 재질정보 읽는다
	if (FAILED(D3DXLoadMeshFromX(L"./src/Tiger.x", D3DXMESH_SYSTEMMEM,
		g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh)))
	{
		MessageBox(NULL, L"Could not find Tiger.x", L"Mesh.exe", MB_OK);
		return E_FAIL;
	}

	//재질정보와 텍스쳐 정보를 따로 뽑아낸다.
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	//재질 개수만큼 재질 구조체 배열 생성
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	//재질 개수만큼 텍스쳐 배열 생성
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;					//재질 정보 복사
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;		//주변광원 정보를 Diffuse 정보로
		g_pMeshTextures[i] = NULL;

		if (d3dxMaterials[i].pTextureFilename != NULL && lstrlen((LPCWSTR)d3dxMaterials[i].pTextureFilename) > 0)
		{
			//텍스쳐를 파일에서 로드한다
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
				L"./src/Tigerr.bmp", &g_pMeshTextures[i])))
			{
				MessageBox(NULL, L"Could not find texture map", L"Mesh.exe", MB_OK);
				return E_FAIL;
			}
		}
	}

	//임시생성 재질버퍼 해제
	pD3DXMtrlBuffer->Release();

	return S_OK;
}
