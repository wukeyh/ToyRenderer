#pragma once

#include "..\tgaimage.h"
#include "..\geometry.h"


struct TGAColor;

class IShader
{
public:
	virtual ~IShader() {}
	virtual vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(vec3f bar, TGAColor& color) = 0;
};

