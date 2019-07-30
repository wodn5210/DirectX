#pragma once
#include "ObjGraphic.h"
#include "Terrain.h"
#include "define.h"


class ObjBall :
	public ObjGraphic
{
private:
	ID3DXMesh* m_pMesh;
	Terrain* m_pTerrain;
	
	D3DXVECTOR3 m_vAcc;		// ���ӵ� ����
	D3DXVECTOR3 m_vSpeed;	// �ӵ� ����
	
	
	float m_r;				// ������
	float m_weight;			// ����

	


public:
	ObjBall(LPDIRECT3DDEVICE9 device, Terrain* terrain);
	~ObjBall();

	void DrawMain();
	void DrawMap();
	HRESULT Create(D3DXVECTOR3 pos, float r);
	HRESULT _InitMtrl();
	float GetR() { return m_r; }

	void Reset() 
	{ 
		D3DXVECTOR3 origin = D3DXVECTOR3(20, 50, 20); 
		SetCenter(origin); 
		m_vSpeed = D3DXVECTOR3(0, 0, 0);
		m_vAcc = D3DXVECTOR3(0, 0, 0);
	}

	//������ ��Ҹ� �ݿ��� ������ ����
	//���� ��ġ�� ����vertex
	void MovePhysical(D3DXVECTOR3 terrain[3] = 0);
	void SetBallAccAdd(D3DXVECTOR3 acc) { m_vAcc += acc; };
	VOID SetBallSpdAdd(D3DXVECTOR3 spd) { m_vSpeed += spd; };
};

