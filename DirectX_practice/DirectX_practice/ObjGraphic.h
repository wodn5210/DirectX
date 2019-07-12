#pragma once
#include <d3dx9.h>
#include <vector>

using namespace std;

typedef int TYPE;
const TYPE TRIANGLE = 0;
const TYPE CUBE = 1;
const TYPE MESH = 2;

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct MYINDEX
{
	WORD _0, _1, _2;
};


class ObjGraphic
{
private:
	TYPE type;
	
	D3DCOLORVALUE _material;
	vector<CUSTOMVERTEX> _vtx;
	vector<MYINDEX> _idx;
	
	WORD info_fvf;

	D3DXMATRIXA16 matWorld, translation, rotation, scale;
public:
	virtual void drawObj() = 0;
	virtual void create(D3DXVECTOR3 center, D3DCOLORVALUE material = { 1, 1, 1, 1 }) = 0;
};