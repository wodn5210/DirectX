#pragma once
#include "ObjGraphic.h"
class ObjTriangle :
	public ObjGraphic
{
private:


public:
	void drawObj();
	void create(D3DXVECTOR3 center, D3DCOLORVALUE material = { 1, 1, 1, 1 });
};

