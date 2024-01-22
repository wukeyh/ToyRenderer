
#include "tgaimage.h"
#include "model.h"
#include "pipeline.h"
//#include "platform/win32.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const int width = 800;
const int height = 800;

const vec3f light_dir(1, 1, 1);
const vec3f camera(0, -1, 3);
const vec3f center(0, 0, 0);
const vec3f up(0, 1, 0);


vec3f m2v(Matrix m)
{
	return vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Matrix v2m(vec3f v)
{
	Matrix m;
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.0f;
	return m;
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
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
	std::shared_ptr<Model> model = std::make_shared<Model>("obj/african_head.obj");
	
	//std::shared_ptr<float> zbuffer = std::make_shared<float>(width * height, -std::numeric_limits<float>::max());
	/*
	float* zbuffer = new float[width * height];
	for(int i = 0; i < width * height; i++)
	{
		zbuffer[i] = -std::numeric_limits<float>::max();
	}*/

	//vec2i pts[3] = { vec2i(10,10), vec2i(100, 30), vec2i(190, 160) };
	//vec2i pts[3] = { vec2i(10, 10), vec2i(150, 30), vec2i(70, 160) };

	//triangle(pts, image, red);
	/*
	Matrix Model = Matrix::identity();
	Matrix ModelView = lookat(camera, center, vec3f(0, 1, 0));
	Matrix Projection = Matrix::identity();
	Matrix Viewport = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);*/
	//Projection[3][2] = -1.f / (camera - center).norm();

	/*
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
			normal[j] = model->normal(i, j);
			tex_coords[j] = model->uv(i, j);
			screen_coords[j] = world2screen(v);
			//screen_coords[j] = vec3f(Viewport * Projection * ModelView * Matrix(v));
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

		
	}*/

	lookat(camera, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (camera - center).norm());

	Shader shader;
	shader.payload.model = model.get();

	shader.payload.light.direction = light_dir;

	for(int i = 0; i < model->nfaces(); i++)
	{
		vec4f screen_coords[3];
		for(int j = 0; j < 3; j++)
		{
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, image, zbuffer);
	}
	

	image.flip_vertically();	// i want to have the origin at the left bottom corner of the image
	zbuffer.flip_vertically();	// i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output/lesson6_2.tga");
	zbuffer.write_tga_file("output/lesson6_3_zbuffer.tga");
	return 0;
}
