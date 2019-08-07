#pragma once
#include "CamMain.h"
#include <stdio.h>
class CamBall :
	public CamMain
{
private:
	D3DXVECTOR3 m_ballEye;
	D3DXVECTOR3* m_pBallPos;
	D3DXVECTOR3 m_ballLookat;

	float m_angleX;

public:
	CamBall();

	VOID SetView();
	VOID RotateLocalY(float angle);
	VOID RotateLocalX(float angle);

	VOID SetPressUp();
	VOID SetPressDown();
	VOID SetPressLeft();
	VOID SetPressRight();

	VOID SetBallPosP(D3DXVECTOR3* pBallPos) { m_pBallPos = pBallPos; };
	void ResetView()
	{
		SetView();
		m_device->SetViewport(&m_vp);
		//m_ballEye = D3DXVECTOR3(0, 1, -2);
	};
};

