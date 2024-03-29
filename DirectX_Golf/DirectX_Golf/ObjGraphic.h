#pragma once
#include "define.h"

class ObjGraphic
{
protected:
	//미니맵 배율
	float m_big = 2.0f;
	LPDIRECT3DDEVICE9 m_device;

	D3DXMATRIXA16 m_matWorld, m_translation, m_rotation, m_scale, m_bigScale;
	D3DXVECTOR3 m_center;
	//m_translation의 x, y, z이동부분이 센터라고봐야함
public:
	ObjGraphic()
	{
		m_device = NULL;
		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_translation);
		D3DXMatrixIdentity(&m_rotation);
		D3DXMatrixIdentity(&m_scale);
		D3DXMatrixIdentity(&m_bigScale);
		m_bigScale._11 = m_scale._11 * m_big;
		m_bigScale._22 = m_scale._22 * m_big;
		m_bigScale._33 = m_scale._33 * m_big;
	}



	void SetTranslation(D3DXMATRIXA16& translation)
	{
		m_translation = translation;
		m_center.x = translation._41;
		m_center.y = translation._42;
		m_center.z = translation._43;
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
		D3DXMatrixIdentity(&m_scale);
		m_scale = scale;
		m_bigScale._11 = m_scale._11 * m_big;
		m_bigScale._22 = m_scale._22 * m_big;
		m_bigScale._33 = m_scale._33 * m_big;
	}
	void SetScale(D3DXVECTOR3& scale)
	{
		D3DXMatrixIdentity(&m_scale);
		m_scale._11 = scale.x;
		m_scale._22 = scale.y;
		m_scale._33 = scale.z;
		m_bigScale._11 = m_scale._11 * m_big;
		m_bigScale._22 = m_scale._22 * m_big;
		m_bigScale._33 = m_scale._33 * m_big;
	}
	void SetRotate(D3DXMATRIXA16* rotate)
	{
		m_rotation = *rotate;
	}

	D3DXVECTOR3* GetCenter()
	{
		return &m_center;
	}
};