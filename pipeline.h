#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include "shader/shader.h"


extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f);					//	coeff = -1/c
void lookat(vec3f eye, vec3f center, vec3f up);

vec3f CrossProduct(vec2i* pts, vec2i P);
vec3f barycentric(vec2i* pts, vec2i p);
vec3f barycentric(vec3f A, vec3f B, vec3f C, vec3f P);

void triangle(vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer);