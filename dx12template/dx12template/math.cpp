#include "math.h"
#include <cstdlib>

Matrix3x3 Mul(Matrix3x3 const& a, Matrix3x3 const& b)
{
	return Matrix3x3
	(
		a.m_a00 * b.m_a00 + a.m_a01 * b.m_a10 + a.m_a02 * b.m_a20, a.m_a00 * b.m_a01 + a.m_a01 * b.m_a11 + a.m_a02 * b.m_a21, a.m_a00 * b.m_a02 + a.m_a01 * b.m_a12 + a.m_a02 * b.m_a22,
		a.m_a10 * b.m_a00 + a.m_a11 * b.m_a10 + a.m_a12 * b.m_a20, a.m_a10 * b.m_a01 + a.m_a11 * b.m_a11 + a.m_a12 * b.m_a21, a.m_a10 * b.m_a02 + a.m_a11 * b.m_a12 + a.m_a12 * b.m_a22,
		a.m_a20 * b.m_a00 + a.m_a21 * b.m_a10 + a.m_a22 * b.m_a20, a.m_a20 * b.m_a01 + a.m_a21 * b.m_a11 + a.m_a22 * b.m_a21, a.m_a20 * b.m_a02 + a.m_a21 * b.m_a12 + a.m_a22 * b.m_a22
	);
}

float RandFloat()
{
	return ((float)rand()) / ((float)RAND_MAX);
}

