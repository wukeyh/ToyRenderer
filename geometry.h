#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__


#include <cassert>
#include <ostream>


#define PI 3.1415926535897932384626433832795f

template <size_t Rows, size_t Cols, typename T> class mat;

template <size_t DIM, typename T>
struct vec
{
	vec() { for (size_t i = DIM; i--; data[i] = T()); }
	vec(const vec<DIM, T>& v) { for (size_t i = DIM; i--; data[i] = v[i]); }
	T& operator[](const size_t i) { return data[i]; }
	const T& operator[](const size_t i) const { return data[i]; }
private:
	T data[DIM];
};

typedef vec<4, float> vec4f;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> struct vec<2, T> {
	union {
		struct { T u, v; };
		struct { T x, y; };
		T raw[2];
	};
	vec() : u(T()), v(T()) {}
	vec(T _u, T _v) : u(_u), v(_v) {}

	template <class U> vec<2, T>(const vec<2, U>& v);

	/*inline vec<2, T> operator +(const vec<2, T>& V) const { return vec2<T>(u + V.u, v + V.v); }
	inline vec<2, T> operator -(const vec<2, T>& V) const { return vec2<T>(u - V.u, v - V.v); }
	inline vec<2, T> operator *(float f)          const { return vec2<T>(u * f, v * f); }
	inline T	   operator *(const vec<2, T>& V) const { return u * V.u + v * V.v; }
	inline T       operator ^(const vec<2, T>& V) const { return x * V.v - y * V.u;}*/
		   T&      operator[](const int i) { return raw[i]; }
	const  T& operator[](const int i) const { return raw[i]; }
	float norm() const { return std::sqrt(u * u + v * v); }
	vec<2, T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, vec<2, T>& v);
};

template <typename T> struct vec<3, T> {
	union {
		struct { T x, y, z; };
		struct { T ivert, iuv, inorm; };
		T raw[3];
	};
	vec() : x(T()), y(T()), z(T()) {}
	vec(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	template <class U> vec<3, T>(const vec<3, U>& v);
	vec<3, T>(const vec<3, T>& v) : x(v.x), y(v.y), z(v.z) {}
	/*inline vec3<T> operator ^(const vec3<T>& v) const { return vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline vec3<T> operator +(const vec3<T>& v) const { return vec3<T>(x + v.x, y + v.y, z + v.z); }
	inline vec3<T> operator -(const vec3<T>& v) const { return vec3<T>(x - v.x, y - v.y, z - v.z); }
	inline vec3<T> operator *(float f)          const { return vec3<T>(x * f, y * f, z * f); }
	inline T       operator *(const vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }*/
	inline T&      operator[](const int i) { return raw[i]; }
	const T& operator[](const int i) const { return raw[i]; }
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	vec<3, T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, vec<3, T>& v);
};

typedef vec<2, float> vec2f;
typedef vec<2, int>   vec2i;
typedef vec<3, float> vec3f;
typedef vec<3, int>   vec3i;
/*
template <> template <> vec<3, int>::vec3<3, int>(const vec<3, float>& v);
template <> template <> vec3<float>::vec3(const vec3<int>& v);
*/
template <class t> std::ostream& operator<<(std::ostream& s, vec<2, t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, vec<3, t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<size_t LEN, typename T> T operator *(const vec<LEN, T>& v1, const vec<LEN, T>& v2)
{
	T res = T();
	for(size_t i = 0; i < LEN; i++)
	{
		res += v1[i] * v2[i];
	}
	return res;
}

template<size_t LEN, typename T> vec<LEN, T> operator +(const vec<LEN, T>& v1, const vec<LEN, T>& v2)
{
	vec<LEN, T> res;
	for(size_t i = 0; i < LEN; i++)
	{
		res[i] = v1[i] + v2[i];
	}
	return res;
}

template<size_t LEN, typename T> vec<LEN, T> operator -(const vec<LEN, T>& v1, const vec<LEN, T>& v2)
{
	vec<LEN, T> res;
	for(size_t i = 0; i < LEN; i++)
	{
	    res[i] = v1[i] - v2[i];
	}
	return res;
}

template<size_t LEN, typename T, typename U> vec<LEN, T> operator *(const vec<LEN, T>& v, const U& t)
{
	vec<LEN, T> res;
	for(size_t i = 0; i < LEN; i++)
	{
		res[i] = v[i] * t;
	}
	return res;
}

template<size_t LEN, typename T, typename U> vec<LEN, T> operator /(const vec<LEN, T>& v, const U& t)
{
	vec<LEN, T> res;
	for(size_t i = 0; i < LEN; i++)
	{
		res[i] = v[i] / t;
	}
	return res;
}

template<typename T> T operator ^(const vec<2, T>& v1, const vec<2, T>& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

template<typename T> vec<3, T> operator ^(const vec<3, T>& v1, const vec<3, T>& v2)
{
		return vec<3, T>(v1.y * v2.z - v1.z * v2.y,
						 v1.z * v2.x - v1.x * v2.z,
						 v1.x * v2.y - v1.y * v2.x);
}

template<size_t LEN, typename T> vec<LEN, T>& operator -=(vec<LEN, T>& v1, const vec<LEN, T>& v2)
{
	for(size_t i = 0; i < LEN; i++)
	{
		v1[i] -= v2[i];
	}
	return v1;
}

template<size_t LEN, typename T> vec<LEN, T>& operator +=(vec<LEN, T>& v1, const vec<LEN, T>& v2)
{
	for(size_t i = 0; i < LEN; i++)
	{
		v1[i] += v2[i];
	}
	return v1;
}

template<size_t LEN, typename T> vec<LEN, T>& operator *=(vec<LEN, T>& v, const T& t)
{
	for(size_t i = 0; i < LEN; i++)
	{
		v[i] *= t;
	}
	return v;
}

template<size_t LEN, typename T> vec<LEN, T>& operator /=(vec<LEN, T>& v, const T& t)
{
	for(size_t i = 0; i < LEN; i++)
	{
		v[i] /= t;
	}
	return v;
}

// 向量三维转四维 向量齐次化
template<size_t LEN, size_t DIM, typename T> vec<LEN, T> embed(const vec<DIM, T>& v, T fill = 1)
{
	vec<LEN, T> res;
	for (size_t i = 0; i < LEN; i++)
	{
		res[i] = (i < DIM ? v[i] : fill);
	}
	return res;
}


template<size_t LEN, size_t DIM, typename T> vec<LEN, T> proj(const vec<DIM, T>& v)
{
	vec<LEN, T> res;
	for(int i = 0; i < LEN; i++)
	{
		res[i] = v[i];
	}
	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<size_t DIM, typename T> struct dt {
	static T det(const mat<DIM, DIM, T>& src) {
		T ret = 0;
		for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
		return ret;
	}
};

template<typename T> struct dt<1, T> {
	static T det(const mat<1, 1, T>& src) {
		return src[0][0];
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<size_t Rows, size_t Cols, typename T>
class mat
{
	public:
	mat() {}

	vec<Cols, T>& operator[](const size_t i) { return data[i]; }
	const vec<Cols, T>& operator[](const size_t i) const { return data[i]; }
	/*
	mat(const mat<Rows, Cols, T>& m) : data{ 0 }
	{
		for (size_t i = 0; i < Rows; i++)
		{
			for (size_t j = 0; j < Cols; j++)
			{
				data[i][j] = m.data[i][j];
			}
		}
	}*/
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

	// 静态函数，创建单位矩阵
	static mat<Rows, Cols, T> identity()
	{
		mat<Rows, Cols, T> result;
		for (size_t i = 0; i < Rows; i++)
		{
			result.data[i][i] = 1;
		}
		return result;
	}

	vec<Rows, T> col(const size_t idx) const
	{
		assert(idx < Cols);
		vec<Rows, T> result;
		for(int i = 0; i < Rows; i++)
		{
			result[i] = data[i][idx];
		}
		return result;
	}

	void set_col(size_t idx, const vec<Rows, T>& v)
	{
		assert(idx < Cols);
		for(int i = 0; i < Rows; i++)
		{
			data[i][idx] = v[i];
		}
	}

	// 矩阵求逆 逆矩阵 = 转置矩阵 / 行列式  辅助函数

	T det() const {
		return dt<Cols, T>::det(*this);
	}

	mat<Rows - 1, Cols - 1, T> get_minor(size_t row, size_t col) const
	{
		mat<Rows - 1, Cols - 1, T> ret;
		for (size_t i = Rows - 1; i--; )
			for (size_t j = Cols - 1; j--; ret[i][j] = data[i < row ? i : i + 1][j < col ? j : j + 1]);
		return ret;
	}

	T cofactor(size_t row, size_t col) const {
		return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
	}

	mat<Rows, Cols, T> adjugate() const {
		mat<Rows, Cols, T> ret;
		for (size_t i = Rows; i--; )
			for (size_t j = Cols; j--; ret[i][j] = cofactor(i, j));
		return ret;
	}

	// 矩阵转置 交换行列
	mat<Rows, Cols, T> transpose() const
	{
		mat<Rows, Cols, T> result;
		for(int i = 0; i < Rows; i++)
		{
			for(int j = 0; j < Cols; j++)
			{
				result.data[i][j] = data[j][i];
			}
		}
		return result;
	}

	// 矩阵求逆转置矩阵 逆矩阵 = 转置矩阵 / 行列式
	mat<Cols, Rows, T> invert_transpose() const
	{
		mat<Rows, Cols, T> ret = adjugate();
		T tmp = ret[0] * data[0];
		return ret / tmp;
	}

private:
	vec<Cols, T> data[Rows];
};


template <size_t Rows, size_t Cols, typename T>
vec<Rows, T> operator *(const mat<Rows, Cols, T>& m, const vec<Cols, T>& v)
{
	vec<Rows, T> result;
	for (size_t i = 0; i < Rows; i++)
	{
		for (size_t j = 0; j < Cols; j++)
		{
			result[i] += m[i][j] * v[j];
		}
	}
	return result;
}


template <size_t R1, size_t C1, size_t C2, typename T>
mat<R1, C2, T> operator *(const mat<R1, C1, T>& m1, const mat<C1, C2, T>& m2)
{
	mat<R1, C2, T> result;
	for (size_t i = 0; i < R1; i++)
	{
		for (size_t j = 0; j < C2; j++)
		{
			for (size_t k = 0; k < C1; k++)
			{
				result[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return result;
}

// 三维空间下平移矩阵
template<typename T>
mat<4, 4, T> translate(float x, float y, float z)
{
	mat<4, 4, T> result = mat<4, 4, T>::identity();
	result[0][3] = x;
	result[1][3] = y;
	result[2][3] = z;
	return result;
}


// 三维空间下缩放矩阵
template<typename T>
mat<4, 4, T> scale(float x, float y, float z)
{
	mat<4, 4, T> result = mat<4, 4, T>::identity();
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = z;
	return result;
}


// 三维空间下绕x轴旋转矩阵
template<typename T>
mat<4, 4, T> rotate_x(float angle)
{
	mat<4, 4, T> result = mat<4, 4, T>::identity();
	float rad = angle * PI / 180.f;
	float cos = std::cos(rad);
	float sin = std::sin(rad);
	result[1][1] = cos;
	result[1][2] = -sin;
	result[2][1] = sin;
	result[2][2] = cos;
	return result;
}


// 三维空间下绕y轴旋转矩阵
template<typename T>
mat<4, 4, T> rotate_y(float angle)
{
	mat<4, 4, T> result = mat<4, 4, T>::identity();
	float rad = angle * PI / 180.f;
	float cos = std::cos(rad);
	float sin = std::sin(rad);
	result[0][0] = cos;
	result[0][2] = sin;
	result[2][0] = -sin;
	result[2][2] = cos;
	return result;
}


// 三维空间下绕z轴旋转矩阵
template<typename T>
mat<4, 4, T> rotate_z(float angle)
{
	mat<4, 4, T> result = mat<4, 4, T>::identity();
	float rad = angle * PI / 180.f;
	float cos = std::cos(rad);
	float sin = std::sin(rad);
	result[0][0] = cos;
	result[0][1] = -sin;
	result[1][0] = sin;
	result[1][1] = cos;
	return result;
}


typedef mat<4, 4, float> Matrix;


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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //__GEOMETRY_H__