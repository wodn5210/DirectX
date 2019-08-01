#pragma once
#include "ObjGraphic.h"
#include "Terrain.h"
#include "define.h"


class ObjBall :
	public ObjGraphic
{
private:

	LPD3DXMESH m_lpMesh;
	Terrain* m_pTerrain;


	D3DXVECTOR3 m_vAcc;		// 가속도 벡터
	D3DXVECTOR3 m_vSpeed;	// 속도 벡터
	LPDIRECT3DTEXTURE9 m_tex;
	
	float m_r;				// 반지름
	float m_weight;			// 무게

	
	HRESULT _InitMtrl();

public:
	ObjBall(LPDIRECT3DDEVICE9 device, Terrain* terrain);
	~ObjBall();

	void DrawMain();
	void DrawMap();
	HRESULT Create(D3DXVECTOR3 pos, float r);
	
	float GetR() { return m_r; }

	void Reset() 
	{ 
		D3DXVECTOR3 origin = D3DXVECTOR3(44, 1, -50);
		SetCenter(origin); 
		m_vSpeed = D3DXVECTOR3(0, 0, 0);
		m_vAcc = D3DXVECTOR3(0, 0, 0);
	}

	//물리적 요소를 반영한 움직임 하자
	//현재 위치의 지형vertex
	void MovePhysical(D3DXVECTOR3 terrain[3] = 0);
	VOID SetBallSpdAdd(D3DXVECTOR3 spd) { m_vSpeed += spd; };
	VOID SetBallJump(float energy, D3DXVECTOR3* pView);
	
};

