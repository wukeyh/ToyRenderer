
#include "shader.h"
#include "..\pipeline.h"


// GouraudShader

vec4f Shader::vertex(int iface, int nthvert)
{
	//payload.normal[nthvert] = payload.model->normal(iface, nthvert);
	payload.tex_coords[nthvert] = payload.model->uv(iface, nthvert);
	vec4f gl_Vertex = embed<4>(payload.model->vert(iface, nthvert));
	return Viewport * Projection * ModelView * gl_Vertex;
}

bool Shader::fragment(vec3f bar, TGAColor& color)
{
	vec2f uv = payload.tex_coords[0] * bar[0] + payload.tex_coords[1] * bar[1] + payload.tex_coords[2] * bar[2];					// interpolate uv for the current pixel
	vec3f normal = proj<3>((Projection * ModelView).invert_transpose() * embed<4>(payload.model->normal(uv))).normalize();
	vec3f light_dir = proj<3>((Projection * ModelView) * embed<4>(payload.light.direction)).normalize();
	vec3f reflected_light = (normal * (normal * light_dir * 2.f) - light_dir).normalize();
	float specular = std::pow(std::max(0.f, reflected_light.z), payload.model->specular(uv));
	float diffuse = std::max(0.f, normal * light_dir);
	TGAColor c = payload.model->diffuse(uv);
	color = c;
	for(int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * (diffuse + .6 * specular), 255);
	return false;
}

vec4f GouraudShader::vertex(int iface, int nthvert)
{
	varying_intensity[nthvert] = std::max(0.f, payload.model->normal(iface, nthvert) * payload.light.direction);
	vec4f gl_Vertex = embed<4>(payload.model->vert(iface, nthvert));
	return Viewport * Projection * ModelView * gl_Vertex;
}

bool GouraudShader::fragment(vec3f bar, TGAColor& color)
{
	float intensity = varying_intensity * bar;				// interpolate intensity for the current pixel
	color = TGAColor(255, 255, 255) * intensity;
	return false;											// no, we do not discard this pixel
}


// ToonShader

vec4f ToonShader::vertex(int iface, int nthvert)
{
	varying_intensity[nthvert] = std::max(0.f, payload.model->normal(iface, nthvert) * payload.light.direction);
	vec4f gl_Vertex = embed<4>(payload.model->vert(iface, nthvert));
	return Viewport * Projection * ModelView * gl_Vertex;
}

bool ToonShader::fragment(vec3f bar, TGAColor& color)
{
	float intensity = varying_intensity * bar;				// interpolate intensity for the current pixel
	if (intensity > .85f) intensity = 1;					// 根据光照强度，将颜色分成 5 个等级
	else if (intensity > .60f) intensity = .80f;
	else if (intensity > .45f) intensity = .60f;
	else if (intensity > .30f) intensity = .45f;
	else if (intensity > .15f) intensity = .30f;
	else intensity = 0;
	color = TGAColor(255, 155, 0) * intensity;
	return false;											// no, we do not discard this pixel
}
