#pragma once
#include <math.h>

namespace MathConsts
{
	float const PI = 3.14159265359f;
	float const DegToRad = PI / 180.f;
}
extern float RandFloat();
struct Quaternion;

struct Vec2
{
	union
	{
		struct
		{
			float x, y;
		};
		float data[2];
	};


	Vec2()
		: x(0.f)
		, y(0.f)
	{}
	Vec2(Vec2 const& v)
		: x(v.x)
		, y(v.y)
	{}
	Vec2(float const x, float const y)
		: x(x)
		, y(y)
	{}
	Vec2(float const x)
		: x(x)
		, y(x)
	{}

	void Set(float const x, float const y)
	{
		this->x = x;
		this->y = y;
	}

	inline float Magnitude2() const
	{
		return x * x + y * y;
	}

	void Normalize()
	{
		float const radius2 = x * x + y * y;
		if (0.f < radius2)
		{
			float const invRadius = 1.f / sqrtf(radius2);
			x *= invRadius;
			y *= invRadius;
		}
	}

	Vec2 GetNormalized() const
	{
		float const radius2 = x * x + y * y;
		if (0.f < radius2)
		{
			float const invRadius = 1.f / sqrtf(radius2);
			return Vec2(x * invRadius, y * invRadius);
		}

		return Vec2(1.f, 0.f);
	}

	Vec2 operator*(Vec2 const v) const
	{
		return Vec2(x * v.x, y * v.y);
	}

	Vec2 operator/(Vec2 const v) const
	{
		return Vec2(x / v.x, y / v.y);
	}

	Vec2 operator*(float const a) const
	{
		return Vec2(x * a, y * a);
	}

	Vec2 operator+(Vec2 const v) const
	{
		return Vec2(x + v.x, y + v.y);
	}

	Vec2 operator-(Vec2 const v) const
	{
		return Vec2(x - v.x, y - v.y);
	}

	void operator=(float const a)
	{
		x = a;
		y = a;
	}

	void operator*=(float const a)
	{
		x *= a;
		y *= a;
	}

	void operator/=(float const a)
	{
		x /= a;
		y /= a;
	}

	void operator+=(Vec2 const v)
	{
		x += v.x;
		y += v.y;
	}

	void operator-=(Vec2 const v)
	{
		x -= v.x;
		y -= v.y;
	}

	bool operator==( Vec2 const other ) const
	{
		return x == other.x && y == other.y;
	}

	static Vec2 GetRandomInCircle()
	{
		float const x = RandFloat() * 2.f - 1.f;
		float const y = RandFloat() * -2.f + 1.f;

		Vec2 position(x, y);
		float const magnitude2 = position.Magnitude2();
		if (1.f < magnitude2)
		{
			position /= magnitude2;
		}

		return position;
	}

	static Vec2 GetRandomOnCircle()
	{
		float const x = RandFloat() * 2.f - 1.f;
		float const y = RandFloat() * -2.f + 1.f;

		Vec2 position(x, y);
		position.Normalize();

		return position;
	}
};

struct Vec2i
{
	union
	{
		struct
		{
			int x, y;
		};
		int data[2];
	};

	Vec2i()
		: x(0)
		, y(0)
	{}
	Vec2i(Vec2i const& v)
		: x(v.x)
		, y(v.y)
	{}
	Vec2i(int const x, int const y)
		: x(x)
		, y(y)
	{}
	void operator -=(Vec2i const& v)
	{
		x -= v.x;
		y -= v.y;
	}

	operator Vec2() const
	{
		return Vec2((float)x, (float)y);
	}
};

struct Vec3
{
	union
	{
		struct
		{
			float x, y, z;
		};
		float data[3];
	};


	Vec3()
		: x(0.f)
		, y(0.f)
		, z(0.f)
	{}
	Vec3(Vec3 const& v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{}
	Vec3(float const x, float const y, float const z)
		: x(x)
		, y(y)
		, z(z)
	{}

	void Set(float const x, float const y, float const z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void operator=(float const a)
	{
		x = a;
		y = a;
		z = a;
	}

	Vec3 GetNormalized() const
	{
		float const mag = x * x + y * y + z * z;
		if (0.f < mag)
		{
			float const invMag = 1.f / sqrtf(mag);
			return Vec3(x * invMag, y * invMag, z * invMag);
		}

		return Vec3(1.f, 0.f, 0.f);
	}

	bool operator==( Vec3 const other ) const
	{
		return x == other.x && y == other.y && z == other.z;
	}
	bool operator!=( Vec3 const other ) const
	{
		return x != other.x && y != other.y && z != other.z;
	}
};

struct Vec4
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float data[4];
	};


	Vec4()
		: x(0.f)
		, y(0.f)
		, z(0.f)
		, w(0.f)
	{}

	Vec4(float const x, float const y, float const z, float const w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{}

	void Set(float const x, float const y, float const z, float const w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void operator=( Vec3 const& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = 1.f;
	}
};

struct Matrix3x3
{
	union
	{
		struct
		{
			float m_data[9];
		};
		struct
		{
			Vec3 m_x;
			Vec3 m_y;
			Vec3 m_z;
		};
		struct
		{
			float m_a00, m_a01, m_a02;
			float m_a10, m_a11, m_a12;
			float m_a20, m_a21, m_a22;
		};
	};

	Matrix3x3()
		: m_a00(1.f), m_a01(0.f), m_a02(0.f)
		, m_a10(0.f), m_a11(1.f), m_a12(0.f)
		, m_a20(0.f), m_a21(0.f), m_a22(1.f)
	{}

	Matrix3x3(float const a00, float const a01, float const a02, float const a10, float const a11, float const a12, float const a20, float const a21, float const a22)
		: m_a00(a00), m_a01(a01), m_a02(a02)
		, m_a10(a10), m_a11(a11), m_a12(a12)
		, m_a20(a20), m_a21(a21), m_a22(a22)
	{}

	Matrix3x3(Matrix3x3 const& mat)
		: m_a00(mat.m_a00), m_a01(mat.m_a01), m_a02(mat.m_a02)
		, m_a10(mat.m_a10), m_a11(mat.m_a11), m_a12(mat.m_a12)
		, m_a20(mat.m_a20), m_a21(mat.m_a21), m_a22(mat.m_a22)
	{}

	void SetTranslateRotationSize(Vec2 const translate, Vec2 const rotation, Vec2 const size)
	{
		m_a00 = +rotation.x * size.x;		m_a01 = -rotation.y * size.y;		m_a02 = translate.x;
		m_a10 = rotation.y * size.x;		m_a11 = rotation.x * size.y;		m_a12 = translate.y;
		m_a20 = 0.f;						m_a21 = 0.f;						m_a22 = 1.f;
	}

	void SetOrthogonalMatrix(float const negative, float const positive)
	{
		float const d = 1.f / (negative - positive);
		float const a = -2.f * d;
		float const b = ( negative + positive ) * d;

		m_a00 = a;		m_a01 = 0.f;	m_a02 = b;
		m_a10 = 0.f;	m_a11 = a;		m_a12 = b;
		m_a20 = 0.f;	m_a21 = 0.f;	m_a22 = 1.f;
	}

	static Matrix3x3 GetTranslateRotationSize(Vec2 const translate, Vec2 const rotation, Vec2 const size)
	{
		Matrix3x3 mat;
		mat.m_a00 = +rotation.x * size.x;		mat.m_a01 = -rotation.y * size.y;		mat.m_a02 = translate.x;
		mat.m_a10 = rotation.y * size.x;		mat.m_a11 = rotation.x * size.y;		mat.m_a12 = translate.y;
		mat.m_a20 = 0.f;						mat.m_a21 = 0.f;						mat.m_a22 = 1.f;

		return mat;
	}

	static Matrix3x3 GetOrthogonalMatrix(float const left, float const right, float const bottom, float const top)
	{
		Matrix3x3 mat;

		float const dX = 1.f / (left - right);
		float const aX = -2.f * dX;
		float const bX = (left + right) * dX;

		float const dY = 1.f / (bottom - top);
		float const aY = -2.f * dY;
		float const bY = (bottom + top) * dY;

		mat.m_a00 = aX;		mat.m_a01 = 0.f;	mat.m_a02 = bX;
		mat.m_a10 = 0.f;	mat.m_a11 = aY;		mat.m_a12 = bY;
		mat.m_a20 = 0.f;		mat.m_a21 = 0.f;		mat.m_a22 = 1.f;

		return mat;
	}
};

struct Matrix4x4
{
	union
	{
		struct
		{
			float m_data[16];
		};
		struct
		{
			Vec4 m_x;
			Vec4 m_y;
			Vec4 m_z;
			Vec4 m_w;
		};
		struct
		{
			float m_a00, m_a01, m_a02, m_a03;
			float m_a10, m_a11, m_a12, m_a13;
			float m_a20, m_a21, m_a22, m_a23;
			float m_a30, m_a31, m_a32, m_a33;
		};
	};

	Matrix4x4()
		: m_a00(1.f), m_a01(0.f), m_a02(0.f), m_a03(0.f)
		, m_a10(0.f), m_a11(1.f), m_a12(0.f), m_a13(0.f)
		, m_a20(0.f), m_a21(0.f), m_a22(1.f), m_a23(0.f)
		, m_a30(0.f), m_a31(0.f), m_a32(0.f), m_a33(1.f)
	{}

	Matrix4x4(
		float const _a00, float const _a01, float const _a02, float const _a03,
		float const _a10, float const _a11, float const _a12, float const _a13,
		float const _a20, float const _a21, float const _a22, float const _a23,
		float const _a30, float const _a31, float const _a32, float const _a33)
		: m_a00(_a00), m_a01(_a01), m_a02(_a02), m_a03(_a03)
		, m_a10(_a10), m_a11(_a11), m_a12(_a12), m_a13(_a13)
		, m_a20(_a20), m_a21(_a21), m_a22(_a22), m_a23(_a23)
		, m_a30(_a30), m_a31(_a31), m_a32(_a32), m_a33(_a33)
	{}

	Matrix4x4(Matrix3x3 const& mat3x3)
		: m_a00(mat3x3.m_a00),	m_a01(mat3x3.m_a01),	m_a02(mat3x3.m_a02),	m_a03(0.f)
		, m_a10(mat3x3.m_a10),	m_a11(mat3x3.m_a11),	m_a12(mat3x3.m_a12),	m_a13(0.f)
		, m_a20(mat3x3.m_a20),	m_a21(mat3x3.m_a21),	m_a22(mat3x3.m_a22),	m_a23(0.f)
		, m_a30(0.f),			m_a31(0.f),				m_a32(0.f),				m_a33(1.f)
	{}
	void Transpose()
	{
		float x;
		x = m_a01;
		m_a01 = m_a10;
		m_a10 = x;

		x = m_a02;
		m_a02 = m_a20;
		m_a20 = x;

		x = m_a03;
		m_a03 = m_a30;
		m_a30 = x;

		x = m_a12;
		m_a12 = m_a21;
		m_a21 = x;

		x = m_a13;
		m_a13 = m_a31;
		m_a31 = x;

		x = m_a32;
		m_a32 = m_a23;
		m_a23 = x;
	}

	//https://stackoverflow.com/a/1148405
	bool Inverse()
	{
		float inv[16], det;
		int i;

		inv[0] = m_data[5]  * m_data[10] * m_data[15] - 
			m_data[5]  * m_data[11] * m_data[14] - 
			m_data[9]  * m_data[6]  * m_data[15] + 
			m_data[9]  * m_data[7]  * m_data[14] +
			m_data[13] * m_data[6]  * m_data[11] - 
			m_data[13] * m_data[7]  * m_data[10];

		inv[4] = -m_data[4]  * m_data[10] * m_data[15] + 
			m_data[4]  * m_data[11] * m_data[14] + 
			m_data[8]  * m_data[6]  * m_data[15] - 
			m_data[8]  * m_data[7]  * m_data[14] - 
			m_data[12] * m_data[6]  * m_data[11] + 
			m_data[12] * m_data[7]  * m_data[10];

		inv[8] = m_data[4]  * m_data[9] * m_data[15] - 
			m_data[4]  * m_data[11] * m_data[13] - 
			m_data[8]  * m_data[5] * m_data[15] + 
			m_data[8]  * m_data[7] * m_data[13] + 
			m_data[12] * m_data[5] * m_data[11] - 
			m_data[12] * m_data[7] * m_data[9];

		inv[12] = -m_data[4]  * m_data[9] * m_data[14] + 
			m_data[4]  * m_data[10] * m_data[13] +
			m_data[8]  * m_data[5] * m_data[14] - 
			m_data[8]  * m_data[6] * m_data[13] - 
			m_data[12] * m_data[5] * m_data[10] + 
			m_data[12] * m_data[6] * m_data[9];

		inv[1] = -m_data[1]  * m_data[10] * m_data[15] + 
			m_data[1]  * m_data[11] * m_data[14] + 
			m_data[9]  * m_data[2] * m_data[15] - 
			m_data[9]  * m_data[3] * m_data[14] - 
			m_data[13] * m_data[2] * m_data[11] + 
			m_data[13] * m_data[3] * m_data[10];

		inv[5] = m_data[0]  * m_data[10] * m_data[15] - 
			m_data[0]  * m_data[11] * m_data[14] - 
			m_data[8]  * m_data[2] * m_data[15] + 
			m_data[8]  * m_data[3] * m_data[14] + 
			m_data[12] * m_data[2] * m_data[11] - 
			m_data[12] * m_data[3] * m_data[10];

		inv[9] = -m_data[0]  * m_data[9] * m_data[15] + 
			m_data[0]  * m_data[11] * m_data[13] + 
			m_data[8]  * m_data[1] * m_data[15] - 
			m_data[8]  * m_data[3] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[11] + 
			m_data[12] * m_data[3] * m_data[9];

		inv[13] = m_data[0]  * m_data[9] * m_data[14] - 
			m_data[0]  * m_data[10] * m_data[13] - 
			m_data[8]  * m_data[1] * m_data[14] + 
			m_data[8]  * m_data[2] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[10] - 
			m_data[12] * m_data[2] * m_data[9];

		inv[2] = m_data[1]  * m_data[6] * m_data[15] - 
			m_data[1]  * m_data[7] * m_data[14] - 
			m_data[5]  * m_data[2] * m_data[15] + 
			m_data[5]  * m_data[3] * m_data[14] + 
			m_data[13] * m_data[2] * m_data[7] - 
			m_data[13] * m_data[3] * m_data[6];

		inv[6] = -m_data[0]  * m_data[6] * m_data[15] + 
			m_data[0]  * m_data[7] * m_data[14] + 
			m_data[4]  * m_data[2] * m_data[15] - 
			m_data[4]  * m_data[3] * m_data[14] - 
			m_data[12] * m_data[2] * m_data[7] + 
			m_data[12] * m_data[3] * m_data[6];

		inv[10] = m_data[0]  * m_data[5] * m_data[15] - 
			m_data[0]  * m_data[7] * m_data[13] - 
			m_data[4]  * m_data[1] * m_data[15] + 
			m_data[4]  * m_data[3] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[7] - 
			m_data[12] * m_data[3] * m_data[5];

		inv[14] = -m_data[0]  * m_data[5] * m_data[14] + 
			m_data[0]  * m_data[6] * m_data[13] + 
			m_data[4]  * m_data[1] * m_data[14] - 
			m_data[4]  * m_data[2] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[6] + 
			m_data[12] * m_data[2] * m_data[5];

		inv[3] = -m_data[1] * m_data[6] * m_data[11] + 
			m_data[1] * m_data[7] * m_data[10] + 
			m_data[5] * m_data[2] * m_data[11] - 
			m_data[5] * m_data[3] * m_data[10] - 
			m_data[9] * m_data[2] * m_data[7] + 
			m_data[9] * m_data[3] * m_data[6];

		inv[7] = m_data[0] * m_data[6] * m_data[11] - 
			m_data[0] * m_data[7] * m_data[10] - 
			m_data[4] * m_data[2] * m_data[11] + 
			m_data[4] * m_data[3] * m_data[10] + 
			m_data[8] * m_data[2] * m_data[7] - 
			m_data[8] * m_data[3] * m_data[6];

		inv[11] = -m_data[0] * m_data[5] * m_data[11] + 
			m_data[0] * m_data[7] * m_data[9] + 
			m_data[4] * m_data[1] * m_data[11] - 
			m_data[4] * m_data[3] * m_data[9] - 
			m_data[8] * m_data[1] * m_data[7] + 
			m_data[8] * m_data[3] * m_data[5];

		inv[15] = m_data[0] * m_data[5] * m_data[10] - 
			m_data[0] * m_data[6] * m_data[9] - 
			m_data[4] * m_data[1] * m_data[10] + 
			m_data[4] * m_data[2] * m_data[9] + 
			m_data[8] * m_data[1] * m_data[6] - 
			m_data[8] * m_data[2] * m_data[5];

		det = m_data[0] * inv[0] + m_data[1] * inv[4] + m_data[2] * inv[8] + m_data[3] * inv[12];

		if (det == 0.f)
			return false;

		det = 1.0f / det;

		for ( i = 0; i < 16; ++i )
		{
			m_data[ i ] = inv[ i ] * det;
		}
		return true;
	}

	bool Inverse( Matrix4x4& dst )
	{
		float inv[16], det;
		int i;

		inv[0] = m_data[5]  * m_data[10] * m_data[15] - 
			m_data[5]  * m_data[11] * m_data[14] - 
			m_data[9]  * m_data[6]  * m_data[15] + 
			m_data[9]  * m_data[7]  * m_data[14] +
			m_data[13] * m_data[6]  * m_data[11] - 
			m_data[13] * m_data[7]  * m_data[10];

		inv[4] = -m_data[4]  * m_data[10] * m_data[15] + 
			m_data[4]  * m_data[11] * m_data[14] + 
			m_data[8]  * m_data[6]  * m_data[15] - 
			m_data[8]  * m_data[7]  * m_data[14] - 
			m_data[12] * m_data[6]  * m_data[11] + 
			m_data[12] * m_data[7]  * m_data[10];

		inv[8] = m_data[4]  * m_data[9] * m_data[15] - 
			m_data[4]  * m_data[11] * m_data[13] - 
			m_data[8]  * m_data[5] * m_data[15] + 
			m_data[8]  * m_data[7] * m_data[13] + 
			m_data[12] * m_data[5] * m_data[11] - 
			m_data[12] * m_data[7] * m_data[9];

		inv[12] = -m_data[4]  * m_data[9] * m_data[14] + 
			m_data[4]  * m_data[10] * m_data[13] +
			m_data[8]  * m_data[5] * m_data[14] - 
			m_data[8]  * m_data[6] * m_data[13] - 
			m_data[12] * m_data[5] * m_data[10] + 
			m_data[12] * m_data[6] * m_data[9];

		inv[1] = -m_data[1]  * m_data[10] * m_data[15] + 
			m_data[1]  * m_data[11] * m_data[14] + 
			m_data[9]  * m_data[2] * m_data[15] - 
			m_data[9]  * m_data[3] * m_data[14] - 
			m_data[13] * m_data[2] * m_data[11] + 
			m_data[13] * m_data[3] * m_data[10];

		inv[5] = m_data[0]  * m_data[10] * m_data[15] - 
			m_data[0]  * m_data[11] * m_data[14] - 
			m_data[8]  * m_data[2] * m_data[15] + 
			m_data[8]  * m_data[3] * m_data[14] + 
			m_data[12] * m_data[2] * m_data[11] - 
			m_data[12] * m_data[3] * m_data[10];

		inv[9] = -m_data[0]  * m_data[9] * m_data[15] + 
			m_data[0]  * m_data[11] * m_data[13] + 
			m_data[8]  * m_data[1] * m_data[15] - 
			m_data[8]  * m_data[3] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[11] + 
			m_data[12] * m_data[3] * m_data[9];

		inv[13] = m_data[0]  * m_data[9] * m_data[14] - 
			m_data[0]  * m_data[10] * m_data[13] - 
			m_data[8]  * m_data[1] * m_data[14] + 
			m_data[8]  * m_data[2] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[10] - 
			m_data[12] * m_data[2] * m_data[9];

		inv[2] = m_data[1]  * m_data[6] * m_data[15] - 
			m_data[1]  * m_data[7] * m_data[14] - 
			m_data[5]  * m_data[2] * m_data[15] + 
			m_data[5]  * m_data[3] * m_data[14] + 
			m_data[13] * m_data[2] * m_data[7] - 
			m_data[13] * m_data[3] * m_data[6];

		inv[6] = -m_data[0]  * m_data[6] * m_data[15] + 
			m_data[0]  * m_data[7] * m_data[14] + 
			m_data[4]  * m_data[2] * m_data[15] - 
			m_data[4]  * m_data[3] * m_data[14] - 
			m_data[12] * m_data[2] * m_data[7] + 
			m_data[12] * m_data[3] * m_data[6];

		inv[10] = m_data[0]  * m_data[5] * m_data[15] - 
			m_data[0]  * m_data[7] * m_data[13] - 
			m_data[4]  * m_data[1] * m_data[15] + 
			m_data[4]  * m_data[3] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[7] - 
			m_data[12] * m_data[3] * m_data[5];

		inv[14] = -m_data[0]  * m_data[5] * m_data[14] + 
			m_data[0]  * m_data[6] * m_data[13] + 
			m_data[4]  * m_data[1] * m_data[14] - 
			m_data[4]  * m_data[2] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[6] + 
			m_data[12] * m_data[2] * m_data[5];

		inv[3] = -m_data[1] * m_data[6] * m_data[11] + 
			m_data[1] * m_data[7] * m_data[10] + 
			m_data[5] * m_data[2] * m_data[11] - 
			m_data[5] * m_data[3] * m_data[10] - 
			m_data[9] * m_data[2] * m_data[7] + 
			m_data[9] * m_data[3] * m_data[6];

		inv[7] = m_data[0] * m_data[6] * m_data[11] - 
			m_data[0] * m_data[7] * m_data[10] - 
			m_data[4] * m_data[2] * m_data[11] + 
			m_data[4] * m_data[3] * m_data[10] + 
			m_data[8] * m_data[2] * m_data[7] - 
			m_data[8] * m_data[3] * m_data[6];

		inv[11] = -m_data[0] * m_data[5] * m_data[11] + 
			m_data[0] * m_data[7] * m_data[9] + 
			m_data[4] * m_data[1] * m_data[11] - 
			m_data[4] * m_data[3] * m_data[9] - 
			m_data[8] * m_data[1] * m_data[7] + 
			m_data[8] * m_data[3] * m_data[5];

		inv[15] = m_data[0] * m_data[5] * m_data[10] - 
			m_data[0] * m_data[6] * m_data[9] - 
			m_data[4] * m_data[1] * m_data[10] + 
			m_data[4] * m_data[2] * m_data[9] + 
			m_data[8] * m_data[1] * m_data[6] - 
			m_data[8] * m_data[2] * m_data[5];

		det = m_data[0] * inv[0] + m_data[1] * inv[4] + m_data[2] * inv[8] + m_data[3] * inv[12];

		if (det == 0.f)
			return false;

		det = 1.0f / det;

		for ( i = 0; i < 16; ++i )
		{
			dst.m_data[ i ] = inv[ i ] * det;
		}
		return true;
	}

	static Matrix4x4 Projection(float const hfov, float const aspect, float const nearPlane, float const farPlane)
	{
		float const scaleXY = 1.f / tanf(hfov * MathConsts::DegToRad);
		float const scaleZ = farPlane / (farPlane - nearPlane);

		return Matrix4x4
		(
			scaleXY,	0.f,				0.f,					0.f,
			0.f,		aspect * scaleXY,	0.f,					0.f,
			0.f,		0.f,				scaleZ,					1.f,
			0.f,		0.f,				-nearPlane * scaleZ,	0.f
		);
	}

	static Matrix4x4 GetTranslateRotationSize( Vec3 const& translate, Quaternion const& rotation, Vec3 const& scale );
};

struct Quaternion
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		float data[4];
	};
	Quaternion()
		: x(0.f)
		, y(0.f)
		, z(0.f)
		, w(0.f)
	{}
	Quaternion(float const _x, float const _y, float const _z, float const _w)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{}
	void Set(float const _x, float const _y, float const _z, float const _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	Vec3 operator*(Vec3 const& vector) const
	{
		float const x2 = x * x;
		float const y2 = y * y;
		float const z2 = z * z;
		float const w2 = w * w;

		float const xz = x * z;
		float const yw = y * w;
		float const xy = x * y;
		float const zw = z * w;
		float const yz = y * z;
		float const xw = x * w;

		return Vec3
		(
			vector.x * (+x2 - y2 - z2 + w2) + 2.f * (vector.y * (xy - zw) + vector.z * (xz + yw))
			, vector.y * (-x2 + y2 - z2 + w2) + 2.f * (vector.x * (xy + zw) + vector.z * (yz - xw))
			, vector.z * (-x2 - y2 + z2 + w2) + 2.f * (vector.x * (xz - yw) + vector.y * (yz + xw))
		);
	}

	Quaternion operator*( Quaternion const& q ) const
	{
		Quaternion rQ;
		rQ.w = q.w * w - q.x * x - q.y * y - q.z * z;
		rQ.x = q.x * w + q.w * x + q.z * y - q.y * z;
		rQ.y = q.y * w - q.z * x + q.w * y + q.x * z;
		rQ.z = q.z * w + q.y * x - q.x * y + q.w * z;

		return rQ;
	}

	Matrix4x4 ToMatrix4x4() const
	{
		float const x2 = x * x;
		float const y2 = y * y;
		float const z2 = z * z;
		float const w2 = w * w;

		float const xz = x * z;
		float const yw = y * w;
		float const xy = x * y;
		float const zw = z * w;
		float const yz = y * z;
		float const xw = x * w;

		return Matrix4x4
		(
			(+x2 - y2 - z2 + w2),	2.f * (xy + zw),		2.f * (xz - yw),		0.f,
			2.f * (xy - zw),		(-x2 + y2 - z2 + w2),	2.f * (yz + xw),		0.f,
			2.f * (xz + yw),		2.f * (yz - xw),		(-x2 - y2 + z2 + w2),	0.f,
			0.f,					0.f,					0.f,					1.f
		);
	}

	static Quaternion FromAngleAxis(float const angle, float const* axis)
	{
		float const halfAngle = 0.5f * angle;
		float const sinA = sinf(halfAngle);
		return Quaternion(axis[0] * sinA, axis[1] * sinA, axis[2] * sinA, cosf(halfAngle));
	}
};

POD_TYPE(Vec2)
POD_TYPE(Vec2i)
POD_TYPE(Vec3)
POD_TYPE(Vec4)
POD_TYPE(Matrix3x3)
POD_TYPE(Matrix4x4)
POD_TYPE(Quaternion)

extern Matrix3x3 Mul(Matrix3x3 const& a, Matrix3x3 const& b);
extern Vec4 Mul( Vec4 const& vector, Matrix4x4 const& matrix );
extern Matrix4x4 Mul( Matrix4x4 const& matrixA, Matrix4x4 const& matrixB );
extern Vec2 operator*( float const a, Vec2 const v );