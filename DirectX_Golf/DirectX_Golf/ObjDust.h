#pragma once
#include "ObjGraphic.h"
#include "define.h"

using namespace std;

class ObjDust :
	public ObjGraphic
{
public:
	ObjDust() {};
	ObjDust(int numParticles);
	~ObjDust();

	bool Create(LPDIRECT3DDEVICE9 device, string path);
	void Update();
	void Render();

	//리셋하면 처음부터 다시 - 공 충격받으면 부르면 된다
	void Reset(D3DXVECTOR3 center);
private:
	

	void _ResetParticle(Attribute* attribute);
	void _AddParticle();


	void _PreRender();	
	void _PostRender();

	bool _IsEmpty();
	bool _IsDead();

	void _RemoveDeadParticles();

private:
	LPDIRECT3DDEVICE9		m_device;
	LPDIRECT3DTEXTURE9		m_pTex;

	D3DXVECTOR3 m_min, m_max;
	float m_size;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	std::list<Attribute>	m_particles;
	int                     m_maxParticles;

	DWORD m_vbSize;      
	DWORD m_vbOffset;   
	DWORD m_vbBatchSize; 
};

