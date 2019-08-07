#pragma once
#include "ObjGraphic.h"
#include "ObjTerrain.h"
#include "define.h"


class ObjBall :
	public ObjGraphic
{
public:
	enum STATE { ACTIVE, STOP, MAPOUT };
private:
	LPD3DXMESH m_lpMesh;
	ObjTerrain* m_pTerrain;


	D3DXVECTOR3 m_vAcc;		// ���ӵ� ����
	D3DXVECTOR3 m_vSpeed;	// �ӵ� ����
	LPDIRECT3DTEXTURE9 m_tex;
	
	float m_r;				// ������
	float m_weight;			// ����

	
	HRESULT _InitMtrlMain();
	HRESULT _InitMtrlMap();
	HRESULT _InitMesh();

	D3DXVECTOR3 m_origin;

	STATE m_nowState;
public:


	ObjBall();
	~ObjBall();

	void DrawMain();
	void DrawMap();
	HRESULT Create(LPDIRECT3DDEVICE9 device, ObjTerrain* terrain, D3DXVECTOR3 pos, float r);
	
	float GetR() { return m_r; }
	STATE GetState(){ return m_nowState; }

	void Reset() 
	{ 
		D3DXVECTOR3 origin = D3DXVECTOR3(44.0f, 0.2f, -50.0f);
		SetCenter(m_origin); 
		m_vSpeed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_vAcc = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	//������ ��Ҹ� �ݿ��� ������ ����
	//���� ��ġ�� ����vertex
	STATE MovePhysical(D3DXVECTOR3 terrain[3] = 0);
	VOID SetBallSpdAdd(D3DXVECTOR3 spd) { m_vSpeed += spd; };
	VOID SetBallJump(float energy, D3DXVECTOR3* pView);
	
};

