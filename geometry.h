#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__


#include <ostream>

template <size_t DIM, typename T>
struct vec
{
	vec() { for (size_t i = DIM; i--; data[i] = T()); }
	T& operator[](const size_t i) { return data[i]; }
	const T& operator[](const size_t i) const { return data[i]; }
private:
	T data[DIM];
};

typedef vec<4, float> vec4f;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	vec2() : u(0), v(0) {}
	vec2(t _u, t _v) : u(_u), v(_v) {}
	inline vec2<t> operator +(const vec2<t>& V) const { return vec2<t>(u + V.u, v + V.v); }
	inline vec2<t> operator -(const vec2<t>& V) const { return vec2<t>(u - V.u, v - V.v); }
	inline vec2<t> operator *(float f)          const { return vec2<t>(u * f, v * f); }
	inline t	   operator *(const vec2<t>& V) const { return u * V.u + v * V.v; }
	inline t       operator ^(const vec2<t>& V) const { return x * V.v - y * V.u;}
	inline t&      operator[](const int i) { return raw[i]; }
	float norm() const { return std::sqrt(u * u + v * v); }
	vec2<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, vec2<t>& v);
};

template <class t> struct vec3 {
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	vec3() : x(0), y(0), z(0) {}
	vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	inline vec3<t> operator ^(const vec3<t>& v) const { return vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline vec3<t> operator +(const vec3<t>& v) const { return vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline vec3<t> operator -(const vec3<t>& v) const { return vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline vec3<t> operator *(float f)          const { return vec3<t>(x * f, y * f, z * f); }
	inline t       operator *(const vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }
	inline t&      operator[](const int i) { return raw[i]; }
	//inline 
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, vec3<t>& v);
};

typedef vec2<float> vec2f;
typedef vec2<int>   vec2i;
typedef vec3<float> vec3f;
typedef vec3<int>   vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t Rows, size_t Cols, typename T>
class mat
{
	public:
	mat() : data{ 0 } {}
	mat(const mat<Rows, Cols, T>& m) : data{ 0 }
	{
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				data[i][j] = m.data[i][j];
			}
		}
	}
	mat(const T* m) : data{ 0 }
	{
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				data[i][j] = m[i * Cols + j];
			}
		}
	}
	mat(const T m[Rows][Cols]) : data{ 0 }
	{
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				data[i][j] = m[i][j];
			}
		}
	}
	~mat() {}

	inline mat<Rows, Cols, T> operator +(const mat<Rows, Cols, T>& m) const
	{
		mat<Rows, Cols, T> result;
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				result.data[i][j] = data[i][j] + m.data[i][j];
			}
		}
		return result;
	}

	inline mat<Rows, Cols, T> operator -(const mat<Rows, Cols, T>& m) const
	{
		mat<Rows, Cols, T> result;
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				result.data[i][j] = data[i][j] - m.data[i][j];
			}
		}
		return result;
	}

	mat<Rows, Cols, T> operator *(const mat<Rows, Cols, T>& m) const
	{
		mat<Rows, Cols, T> result;
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				for (size_t k = 0; k < Cols; k++)
				{
					result.data[i][j] += data[i][k] * m.data[k][j];
				}
			}
		}
		return result;

	}

	mat<Rows, Cols, T> operator *(const T& t) const
	{
		mat<Rows, Cols, T> result;
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				result.data[i][j] = data[i][j] * t;
			}
		}
		return result;
	}

	mat<Rows, Cols, T> operator /(const T& t) const
	{
		mat<Rows, Cols, T> result;
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				result.data[i][j] = data[i][j] / t;
			}
		}
		return result;
	}

	mat<Rows, Cols, T>& operator =(const mat<Rows, Cols, T>& m)
	{
		if (this != &m)
		{
			for (size_t i = 0; i < Rows; i++)
			{
				for (size_t j = 0; j < Cols; j++)
				{
					data[i][j] = m.data[i][j];
				}
			}
		}
		return *this;
	}
private:
	T data[Rows][Cols];
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
T max(T a, T b) { return a > b ? a : b; }

template <typename T>
T min(T a, T b) { return a < b ? a : b; }

template <typename T, typename T1>
T lerp(T a, T b, T1 t) { return a + (b - a) * t; }

template <typename T>
void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

#endif //__GEOMETRY_H__