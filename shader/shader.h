#pragma once

#include "../model.h"
#include "..\tgaimage.h"
#include "..\geometry.h"


struct TGAColor;

struct light_t
{
	vec3f position;
	vec3f direction;
	vec3f intensity;
};


struct cubemap_t
{
	TGAImage* images[6];
};


typedef struct
{
	// light_transform for shadow mapping.
	Matrix model_transform;
	Matrix camera_view_transform;
	Matrix camera_projection_transform;
	Matrix light_view_transform;
	Matrix light_projection_transform;
	Matrix MVP_transform;

	light_t light;

	Model* model;

	vec3f normal[3];
	vec2f tex_coords[3];
	vec3f world_coords[3];
	vec4f screen_coords[3];
	
}payload_t;



class IShader
{
public:
	payload_t payload;
	virtual ~IShader() {}
	virtual vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(vec3f bar, TGAColor& color) = 0;
};

class Shader : public IShader
{
public:
	virtual vec4f vertex(int iface, int nthvert);
	virtual bool fragment(vec3f bar, TGAColor& color);
};;

class GouraudShader : public IShader
{
public:
	vec3f varying_intensity; // written by vertex shader, read by fragment shader
	virtual vec4f vertex(int iface, int nthvert);
	virtual bool fragment(vec3f bar, TGAColor& color);
};


class PhongShader : public IShader
{

};

class ToonShader : public IShader
{
public:
	vec3f varying_intensity; // written by vertex shader, read by fragment shader
	virtual vec4f vertex(int iface, int nthvert);
	virtual bool fragment(vec3f bar, TGAColor& color);
};

class DepthShader : public IShader
{

};

class NormalShader : public IShader
{

};

