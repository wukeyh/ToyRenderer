#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const int width = 800;
const int height = 800;

const Vec3f light_dir(0, 0, -1);

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
Vec3f barycentric(Vec2i* pts, Vec2i p)
{
	Vec3i x = Vec3i(pts[1].x - pts[0].x, pts[2].x - pts[0].x, p.x - pts[0].x);
	Vec3i y = Vec3i(pts[1].y - pts[0].y, pts[2].y - pts[0].y, p.y - pts[0].y);

	Vec3i u = x ^ y;

	if(std::abs(u.z) < 1)	return Vec3f(-1, 1, 1);

	return Vec3f(1. - (u.x + u.y) / (float)u.z, u.x / (float)u.z, u.y / (float)u.z);
}


// 叉积判断点是否在三角形内
Vec3f CrossProduct(Vec2i* pts, Vec2i P)
{
	// 构建出三角形 ABC 三条边的向量
	Vec2i AB(pts[1].x - pts[0].x, pts[1].y - pts[0].y);
	Vec2i BC(pts[2].x - pts[1].x, pts[2].y - pts[1].y);
	Vec2i CA(pts[0].x - pts[2].x, pts[0].y - pts[2].y);

	// 三角形三个顶点和 P 链接形成的向量
	Vec2i AP(P.x - pts[0].x, P.y - pts[0].y);
	Vec2i BP(P.x - pts[1].x, P.y - pts[1].y);
	Vec2i CP(P.x - pts[2].x, P.y - pts[2].y);

	return Vec3f(AB ^ AP, BC ^ BP, CA ^ CP);
}

void triangle(Vec2i* pts, TGAImage& Image, TGAColor Color)
{
	Vec2i bboxmin(Image.get_width() - 1, Image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(Image.get_width() - 1, Image.get_height() - 1);
	for(int i = 0; i < 3; i++)
	{
		bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
		bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
	}

	Vec2i p;
	for(p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for(p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//Vec3f bc_screen = barycentric(pts, Vec2i(p));
			Vec3f bc_screen = CrossProduct(pts, p);
			if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)	continue;
			Image.set(p.x, p.y, Color);
		}
	}
}

/*
void trangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
	if(t0.y == t1.y && t0.y == t2.y) return; // I don't care about degenerate triangles
	if(t0.y > t1.y) std::swap(t0, t1);
	if(t0.y > t2.y) std::swap(t0, t2);
	if(t1.y > t2.y) std::swap(t1, t2);
	int total_height = t2.y - t0.y;
	for(int i = 0; i < total_height; i++)
	{
		bool second_half = 
	}
}*/


int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);

	std::shared_ptr<Model> model = std::make_shared<Model>("obj/african_head.obj");

	//Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
	//Vec2i pts[3] = { Vec2i(10, 10), Vec2i(150, 30), Vec2i(70, 160) };

	//triangle(pts, image, red);

	for(int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		Vec3f world_coords[3];
		for(int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
			world_coords[j] = v;
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if(intensity > 0)
		{
			//triangle(screen_coords, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
			triangle(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output/lesson2_4.tga");
	return 0;
}
