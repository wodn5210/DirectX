#include "ObjDust.h"


ObjDust::ObjDust(int numParticles)
{
	m_center = D3DXVECTOR3(-10.0f, -10.0f, -10.0f);
	m_size = 0.02f;
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;



	for (int i = 0; i < numParticles; i++)
		_AddParticle();
}
ObjDust::~ObjDust()
{
	if (m_pTex != NULL)
		m_pTex->Release();
	if (m_pVB != NULL)
		m_pVB->Release();
}

bool ObjDust::Create(LPDIRECT3DDEVICE9 device, string path)
{
	m_device = device;

	HRESULT hr = 0;

	hr = m_device->CreateVertexBuffer(
		m_vbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT, 
		&m_pVB,
		0);
	if (FAILED(hr))
	{
		printf("¸ÁÇÔ\n");
		return false;
	}
	hr = D3DXCreateTextureFromFile(device, path.c_str(), &m_pTex);
	if (FAILED(hr))
	{
		printf("¸ÁÇÔ\n");
		return false;
	}
	return true;
}

void ObjDust::Reset(D3DXVECTOR3 center)
{
	m_center = center;
	m_min = m_center - D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_max = m_center + D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	std::list<Attribute>::iterator i;
	for (i = m_particles.begin(); i != m_particles.end(); i++)
	{
		_ResetParticle(&(*i));
	}
}

void ObjDust::_ResetParticle(Attribute* attribute)
{
	attribute->_isAlive = true;
	D3DXVECTOR3 local_min = D3DXVECTOR3(-0.05f, 0.0f, -0.05f);
	D3DXVECTOR3 local_max = D3DXVECTOR3(0.05f, 0.3f, 0.05f);


	MATH::GetRandomVector(&attribute->_velocity, &local_min, &local_max);

	attribute->_acceleration = D3DXVECTOR3(0.0f, -GRAVITY, 0.0f);
	attribute->_position = m_center;
	attribute->_color = D3DCOLOR_XRGB(255, 255, 255);
	attribute->_age = 0.0f;
	attribute->_lifeTime = 10.0f;
}

void ObjDust::_AddParticle()
{
	Attribute attribute;
	_ResetParticle(&attribute);

	m_particles.push_back(attribute);
}

void ObjDust::Update()
{
	std::list<Attribute>::iterator i;
	for (i = m_particles.begin(); i != m_particles.end(); i++)
	{
		if (i->_isAlive)
		{
			
			if (i->_position.y < 0.0f) 
			{
				i->_isAlive = false;
			}

			i->_velocity += i->_acceleration*4;
			i->_position += i->_velocity;

		}
	}
}

void ObjDust::_PreRender()
{
	m_device->SetRenderState(D3DRS_LIGHTING, false);
	m_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_device->SetRenderState(D3DRS_POINTSIZE, MATH::FtoDw(m_size));
	m_device->SetRenderState(D3DRS_POINTSIZE_MIN, MATH::FtoDw(0.0f));


	m_device->SetRenderState(D3DRS_POINTSCALE_A, MATH::FtoDw(0.0f));
	m_device->SetRenderState(D3DRS_POINTSCALE_B, MATH::FtoDw(0.0f));
	m_device->SetRenderState(D3DRS_POINTSCALE_C, MATH::FtoDw(1.0f));


	m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
void ObjDust::Render()
{
	HRESULT hr = 0;

	if (!m_particles.empty())
	{

		_PreRender();

		m_device->SetTexture(0, m_pTex);
		m_device->SetFVF(Particle::FVF);
		m_device->SetStreamSource(0, m_pVB, 0, sizeof(Particle));



		//
		// render batches one by one
		//

		// start at beginning if we're at the end of the vb
		if (m_vbOffset >= m_vbSize)
			m_vbOffset = 0;

		Particle* v = 0;

		hr = m_pVB->Lock(
			m_vbOffset * sizeof(Particle),
			m_vbBatchSize * sizeof(Particle),
			(void**)& v,
			m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		if (FAILED(hr))
		{
			printf("¸ÁÇÔ\n");
		}

		DWORD numParticlesInBatch = 0;

		std::list<Attribute>::iterator i;
		for (i = m_particles.begin(); i != m_particles.end(); i++)
		{
			if (i->_isAlive)
			{

				v->p = i->_position;
				v->c = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; 

				if (numParticlesInBatch == m_vbBatchSize)
				{
					
					m_pVB->Unlock();

					m_device->DrawPrimitive(
						D3DPT_POINTLIST,
						m_vbOffset,
						m_vbBatchSize);

					m_vbOffset += m_vbBatchSize;

					if (m_vbOffset >= m_vbSize)
						m_vbOffset = 0;

					hr = m_pVB->Lock(
						m_vbOffset * sizeof(Particle),
						m_vbBatchSize * sizeof(Particle),
						(void**)& v,
						m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
					if (FAILED(hr))
					{
						printf("¸ÁÇÔ\n");
					}
					numParticlesInBatch = 0;
				}
			}
		}
		
		m_pVB->Unlock();

		if (numParticlesInBatch)
		{
			hr = m_device->DrawPrimitive(
				D3DPT_POINTLIST,
				m_vbOffset,
				numParticlesInBatch);
			if (FAILED(hr))
			{
				printf("¸ÁÇÔ\n");
			}
		}

		m_vbOffset += m_vbBatchSize;

		m_device->SetTexture(0, NULL);
		_PostRender();
	}

}
void ObjDust::_PostRender()
{
	m_device->SetRenderState(D3DRS_LIGHTING, true);
	m_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

bool ObjDust::_IsEmpty()
{
	return m_particles.empty();
}
bool ObjDust::_IsDead()
{
	std::list<Attribute>::iterator i;
	for (i = m_particles.begin(); i != m_particles.end(); i++)
	{

		if (i->_isAlive)
			return false;
	}

	return true;
}
void ObjDust::_RemoveDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = m_particles.begin();

	while (i != m_particles.end())
	{
		if (i->_isAlive == false)
		{
			i = m_particles.erase(i);
		}
		else
		{
			i++; 
		}
	}
}