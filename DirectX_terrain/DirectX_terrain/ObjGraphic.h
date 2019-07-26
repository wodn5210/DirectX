#pragma once
#include <d3dx9.h>
#include <stdio.h>

class ObjGraphic
{
protected:
	//πÃ¥œ∏  πË¿≤
	float m_big = 2.0f;
	LPDIRECT3DDEVICE9 m_device;

	WORD m_fvf;
	D3DXMATRIXA16 m_matWorld, m_translation, m_rotation, m_scale, m_bigScale;
	D3DXVECTOR3 m_center;

public:
	ObjGraphic()
	{
		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_translation);
		D3DXMatrixIdentity(&m_rotation);
		D3DXMatrixIdentity(&m_scale);
		D3DXMatrixIdentity(&m_bigScale);
		m_bigScale._11 = m_scale._11 * m_big;
		m_bigScale._22 = m_scale._22 * m_big;
		m_bigScale._33 = m_scale._33 * m_big;
	}
	virtual void DrawMain() = 0;
	virtual void DrawMap() = 0;


	void SetTranslation(D3DXMATRIXA16& translation)
	{
		m_translation = translation;
	}
	void SetCenter(D3DXVECTOR3& center)
	{
		D3DXMatrixIdentity(&m_translation);
		m_center = center;
		m_translation._41 = m_center.x;
		m_translation._42 = m_center.y;
		m_translation._43 = m_center.z;
	}
	void SetScale(D3DXMATRIXA16& scale)
	{
		D3DXMatrixIdentity(&m_translation);
		m_scale = scale;
		m_bigScale._11 = m_scale._11 * m_big;
		m_bigScale._22 = m_scale._22 * m_big;
		m_bigScale._33 = m_scale._33 * m_big;
	}

	D3DXVECTOR3* GetCenter()
	{
		return &m_center;
	}
};