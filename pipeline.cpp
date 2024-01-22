#include "pipeline.h"

#include "geometry.h"
#include "tgaimage.h"

Matrix ModelView;
Matrix Viewport;
Matrix Projection;


void viewport(int x, int y, int w, int h)
{
	Viewport = Matrix::identity();
	Viewport[0][3] = x + w / 2.f;
	Viewport[1][3] = y + h / 2.f;
	Viewport[2][3] = 255.f / 2.f;

	Viewport[0][0] = w / 2.f;
	Viewport[1][1] = h / 2.f;
	Viewport[2][2] = 255.f / 2.f;
}

void projection(float coeff)
{
	Projection = Matrix::identity();
	Projection[3][2] = coeff;
}

void lookat(vec3f eye, vec3f center, vec3f up)
{
	vec3f z = (eye - center).normalize();
	vec3f x = (up ^ z).normalize();
	vec3f y = (z ^ x).normalize();
	ModelView = Matrix::identity();
	for (int i = 0; i < 3; i++)
	{
		ModelView[0][i] = x[i];
		ModelView[1][i] = y[i];
		ModelView[2][i] = z[i];
		ModelView[i][3] = -center[i];
	}
}


// 叉积判断点是否在三角形内
vec3f CrossProduct(vec2i* pts, vec2i P)
{
	// 构建出三角形 ABC 三条边的向量
	vec2i AB(pts[1].x - pts[0].x, pts[1].y - pts[0].y);
	vec2i BC(pts[2].x - pts[1].x, pts[2].y - pts[1].y);
	vec2i CA(pts[0].x - pts[2].x, pts[0].y - pts[2].y);

	// 三角形三个顶点和 P 链接形成的向量
	vec2i AP(P.x - pts[0].x, P.y - pts[0].y);
	vec2i BP(P.x - pts[1].x, P.y - pts[1].y);
	vec2i CP(P.x - pts[2].x, P.y - pts[2].y);

	return vec3f(AB ^ AP, BC ^ BP, CA ^ CP);
}


// 二维空间中  重心坐标判断点是否在三角形内
vec3f barycentric(vec2i* pts, vec2i p)
{
	vec3i x = vec3i(pts[1].x - pts[0].x, pts[2].x - pts[0].x, p.x - pts[0].x);
	vec3i y = vec3i(pts[1].y - pts[0].y, pts[2].y - pts[0].y, p.y - pts[0].y);

	vec3i u = x ^ y;

	if (std::abs(u.z) < 1)	return vec3f(-1, 1, 1);

	return vec3f(1. - (u.x + u.y) / (float)u.z, u.x / (float)u.z, u.y / (float)u.z);
}


vec3f barycentric(vec3f A, vec3f B, vec3f C, vec3f P)
{
	vec3f s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}

	vec3f u = s[0] ^ s[1];
	if (std::abs(u[2]) > 1e-2)
	{
		return vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}
	return vec3f(-1, 1, 1);
}



// 光栅化 画线
void Line(int x0, int y0, int x1, int y1, TGAImage& Image, TGAColor Color)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			Image.set(y, x, Color);
		}
		else
		{
			Image.set(x, y, Color);
		}
		error2 += derror2;
		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}


// 三角形光栅化
void triangle(vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer)
{
	vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	for (int i = 0; i < 3; i++) 
	{
		for (int j = 0; j < 2; j++) 
		{
			bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
			bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
		}
	}

	vec2i p;
	TGAColor color;
	for(p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for(p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			vec3f bc_screen = barycentric(proj<3>(pts[0] / pts[0][3]), proj<3>(pts[1] / pts[1][3]), proj<3>(pts[2] / pts[2][3]), proj<3>(p));
			float z = pts[0][2] * bc_screen.x + pts[1][2] * bc_screen.y + pts[2][2] * bc_screen.z;
			float w = pts[0][3] * bc_screen.x + pts[1][3] * bc_screen.y + pts[2][3] * bc_screen.z;
			int fragment_depth = std::max(0, std::min(255, int(z / w + .5)));
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || zbuffer.get(p.x, p.y)[0] > fragment_depth)	continue;
			bool discard = shader.fragment(bc_screen, color);
			if(!discard)
			{
				zbuffer.set(p.x, p.y, TGAColor(fragment_depth));
				image.set(p.x, p.y, color);
			}
		}
	}
}