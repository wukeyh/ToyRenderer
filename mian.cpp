
#include "tgaimage.h"
#include "model.h"
//#include "platform/win32.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const int width = 800;
const int height = 800;

const vec3f light_dir(0, 0, -1);

void Line(int x0, int y0, int x1, int y1, TGAImage& Image, TGAColor Color)
{
	bool steep = false;
	if(std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if(x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for(int x = x0; x <= x1; x++)
	{
		if(steep)
		{
			Image.set(y, x, Color);
		}
		else
		{
			Image.set(x, y, Color);
		}
		error2 += derror2;
		if(error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

// 重心坐标判断点是否在三角形内
vec3f barycentric(vec2i* pts, vec2i p)
{
	vec3i x = vec3i(pts[1].x - pts[0].x, pts[2].x - pts[0].x, p.x - pts[0].x);
	vec3i y = vec3i(pts[1].y - pts[0].y, pts[2].y - pts[0].y, p.y - pts[0].y);

	vec3i u = x ^ y;

	if(std::abs(u.z) < 1)	return vec3f(-1, 1, 1);

	return vec3f(1. - (u.x + u.y) / (float)u.z, u.x / (float)u.z, u.y / (float)u.z);
}


vec3f barycentric(vec3f A, vec3f B, vec3f C, vec3f P)
{
		vec3f s[2];
	for(int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}

	vec3f u = s[0] ^ s[1];
	if(std::abs(u[2]) > 1e-2)
	{
		return vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}
	return vec3f(-1, 1, 1);
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

void triangle(vec2i* pts, TGAImage& Image, TGAColor Color)
{
	vec2i bboxmin(Image.get_width() - 1, Image.get_height() - 1);
	vec2i bboxmax(0, 0);
	vec2i clamp(Image.get_width() - 1, Image.get_height() - 1);
	for(int i = 0; i < 3; i++)
	{
		bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
		bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));
		
		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
	}

	vec2i p;
	for(p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for(p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//vec3f bc_screen = barycentric(pts, vec2i(p));
			vec3f bc_screen = CrossProduct(pts, p);
			if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)	continue;
			Image.set(p.x, p.y, Color);
		}
	}
}

// 做了简单的正交投影
vec3f world2screen(vec3f v)
{
	return vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

// 三角形光栅化
void triangle(vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
	vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	vec2f clamp(image.get_width() - 1, image.get_height() - 1);
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}

	vec3f P;
	for(P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
	{
		for(P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
		{
			vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)	continue;
			P.z = 0;
			for(int i = 0; i < 3; i++)
			{
				P.z += pts[i][2] * bc_screen[i];
			}
			if(zbuffer[int(P.x + P.y * width)] < P.z)
			{
				zbuffer[int(P.x + P.y * width)] = P.z;
				//image.set(P.x, P.y, color);
				//image.set(P.x, P.y, color);
			}
		}
	}

}

// 三角形光栅化
void triangle(vec3f* pts, vec2f* tex_coords, std::shared_ptr<Model> model, float* zbuffer, TGAImage& image, float indensity)
{
	vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	vec2f clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	TGAColor color;
	vec3f P;
	vec2f tex_coord;
	for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
		{
			vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)	continue;
			P.z = 0;
			tex_coord = vec2f(0, 0);
			for (int i = 0; i < 3; i++)
			{
				P.z += pts[i][2] * bc_screen[i];
			}
			tex_coord = tex_coords[0] * bc_screen.x + tex_coords[1] * bc_screen.y + tex_coords[2] * bc_screen.z;
			if (zbuffer[int(P.x + P.y * width)] < P.z)
			{
				zbuffer[int(P.x + P.y * width)] = P.z;
				color = model->diffuse(tex_coord);
				color = TGAColor(color.r * indensity, color.g * indensity, color.b * indensity, 255);
				image.set(P.x, P.y, color);
			}
		}
	}

}



template<typename T>
T clamp(T value, T min_value, T max_value) {
	return std::max(min_value, std::min(max_value, value));
}

// 深度纹理
void depthTexture(float* zbuffer, TGAImage& image, int width, int height)
{
	
	for(int i = 0; i < width * height; i++)
	{
		if (zbuffer[i] > -std::numeric_limits<float>::max())
		{
			float depth = zbuffer[i];
			depth = depth * 0.5 + 0.5f;
			image.set(i % width, i / width, TGAColor(depth * 255.f, depth * 255.f, depth * 255.f, 255));
		}
	}
}


void renderloop()
{
	/*while(window)
	{

	}*/
}


int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);

	std::shared_ptr<Model> model = std::make_shared<Model>("obj/african_head.obj");
	
	//std::shared_ptr<float> zbuffer = std::make_shared<float>(width * height, -std::numeric_limits<float>::max());

	float* zbuffer = new float[width * height];
	for(int i = 0; i < width * height; i++)
	{
		zbuffer[i] = -std::numeric_limits<float>::max();
	}

	//vec2i pts[3] = { vec2i(10,10), vec2i(100, 30), vec2i(190, 160) };
	//vec2i pts[3] = { vec2i(10, 10), vec2i(150, 30), vec2i(70, 160) };

	//triangle(pts, image, red);

	for(int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		vec3f screen_coords[3];
		vec3f world_coords[3];
		vec3f normal[3];
		vec2f tex_coords[3];
		for(int j = 0; j < 3; j++)
		{
			vec3f v = model->vert(face[j]);
			tex_coords[j] = model->uv(i, j);
			screen_coords[j] = world2screen(v);
			world_coords[j] = v;
		}
		vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if(intensity > 0)
		{
			//triangle(screen_coords, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
			//triangle(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
			//triangle(screen_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
			//triangle(screen_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));)
			triangle(screen_coords, tex_coords, model, zbuffer, image, intensity);
		}

		
	}

	//depthTexture(zbuffer, image, width, height);

	delete[] zbuffer;

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output/lesson3_5.tga");
	return 0;
}
