#include <iostream>
#include <vector>

#include "geometry.h"
#include <cassert>

#pragma region 计算逆矩阵
// 计算3阶矩阵的行列式
float determinant(Mat3f& mat) {
	Vec3f& row0 = mat.rows[0];
	Vec3f& row1 = mat.rows[1];
	Vec3f& row2 = mat.rows[2];
	return row0[0] * (row1[1] * row2[2] - row1[2] * row2[1])
		- row0[1] * (row1[0] * row2[2] - row1[2] * row2[0])
		+ row0[2] * (row1[0] * row2[1] - row1[1] * row2[0]);
}

// 将三个向量拼接成一个矩阵
Mat3f concatenate(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) {
	Mat3f mat;
	mat[0] = v1;
	mat[1] = v2;
	mat[2] = v3;
	return mat;
}

// 计算四阶矩阵的行列式
float determinant(Mat4f& mat) {
	Vec4f& row0 = mat.rows[0];
	Vec4f& row1 = mat.rows[1];
	Vec4f& row2 = mat.rows[2];
	Vec4f& row3 = mat.rows[3];

	float sub_det0 = row1[1] * (row2[2] * row3[3] - row2[3] * row3[2])
		- row1[2] * (row2[1] * row3[3] - row2[3] * row3[1])
		+ row1[3] * (row2[1] * row3[2] - row2[2] * row3[1]);

	float sub_det1 = row1[0] * (row2[2] * row3[3] - row2[3] * row3[2])
		- row1[2] * (row2[0] * row3[3] - row2[3] * row3[0])
		+ row1[3] * (row2[0] * row3[2] - row2[2] * row3[0]);

	float sub_det2 = row1[0] * (row2[1] * row3[3] - row2[3] * row3[1])
		- row1[1] * (row2[0] * row3[3] - row2[3] * row3[0])
		+ row1[3] * (row2[0] * row3[1] - row2[1] * row3[0]);

	float sub_det3 = row1[0] * (row2[1] * row3[2] - row2[2] * row3[1])
		- row1[1] * (row2[0] * row3[2] - row2[2] * row3[0])
		+ row1[2] * (row2[0] * row3[1] - row2[1] * row3[0]);

	return row0[0] * sub_det0
		- row0[1] * sub_det1
		+ row0[2] * sub_det2
		- row0[3] * sub_det3;
}

// 将四个向量拼接成一个矩阵
Mat4f concatenate(const Vec4f& v1, const Vec4f& v2, const Vec4f& v3, const Vec4f& v4) {
	Mat4f mat;
	mat[0] = v1;
	mat[1] = v2;
	mat[2] = v3;
	mat[3] = v4;
	return mat;
}
#pragma endregion

#pragma region 三阶方阵
Mat3f::Mat3f()
{
	// 初始化三个行向量的值为0.0f
	rows[0] = Vec3f();
	rows[1] = Vec3f();
	rows[2] = Vec3f();
}

Mat3f Mat3f::operator*(Mat3f& a)
{
	Mat3f result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = 0.0f;
			for (int k = 0; k < 3; k++)
			{
				// 计算矩阵乘积的第i行第j列元素
				result[i][j] += rows[i][k] * a.rows[k][j];
			}
		}
	}
	return result;
}

Vec3f Mat3f::operator*(Vec3f& a)
{
	Vec3f result;
	for (int i = 0; i < 3; i++)
	{
		result[i] = 0.0f;
		for (int k = 0; k < 3; k++)
		{
			// 计算矩阵和向量的乘积的第i行元素
			result[i] += rows[i][k] * a[k];
		}
	}
	return result;
}

Vec3f& Mat3f::operator[](const int i) {
	// 检查 i 是否在范围内（0-2）
	assert(i >= 0 && i < 3);

	// 返回指定行的引用
	return rows[i];
}

Vec3f Mat3f::operator[](const int i) const {
	// 检查 i 是否在范围内（0-2）
	assert(i >= 0 && i < 3);

	// 返回指定行的副本
	return rows[i];
}
Mat3f Mat3f::transpose()
{
	Mat3f result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// 将矩阵的第i行第j列元素赋值给转置矩阵的第j行第i列元素
			result[i][j] = rows[j][i];
		}
	}
	return result;
}

Mat3f Mat3f::inverse() {
	Mat3f inv;
	float det = determinant(*this);
	if (det == 0) { // 矩阵不可逆
		return inv;
	}
	float inv_det = 1.0f / det;
	// 构造增广矩阵
	Mat3f aug = concatenate(Vec3f(1, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1));
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			aug[i][j + 3] = (i == j) ? 1 : 0;
		}
	}
	// 高斯-约旦消元
	for (int i = 0; i < 3; i++) {
		// 高斯消元
		float pivot = aug[i][i];
		for (int j = i + 1; j < 6; j++) {
			aug[i][j] /= pivot;
		}
		aug[i][i] = 1;
		for (int j = i + 1; j < 3; j++) {
			float factor = aug[j][i];
			for (int k = i + 1; k < 6; k++) {
				aug[j][k] -= factor * aug[i][k];
			}
			aug[j][i] = 0;
		}
		// 约旦消元
		for (int j = i - 1; j >= 0; j--) {
			float factor = aug[j][i];
			for (int k = i + 1; k < 6; k++) {
				aug[j][k] -= factor * aug[i][k];
			}
			aug[j][i] = 0;
		}
	}
	// 提取逆矩阵
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			inv[i][j] = aug[i][j + 3];
			inv[i][j] *= inv_det;
		}
	}
	return inv;
}

Mat3f Mat3f::identity()
{
	Mat3f E;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// 将E的对角线元素设为1，其余元素设为0
			E[i][j] = (i == j ? 1.0f : 0.0f);
		}
	}
	return E;
}

std::ostream& operator<<(std::ostream& s, Mat3f& m)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			s << m[i][j];
			if (j < 2) s << "\t";
		}
		s << "\n";
	}
	return s;
}
#pragma endregion

#pragma region 四阶方阵
Mat4f::Mat4f()
{
	// 构造函数，将矩阵的四行初始化为 Vec4f() 对象,初始化四个行向量的值为0.0f
	rows[0] = Vec4f();
	rows[1] = Vec4f();
	rows[2] = Vec4f();
	rows[3] = Vec4f();
}

Mat4f Mat4f::operator*(Mat4f& a)
{
	// 重载运算符 *，用于两个矩阵的乘法
	Mat4f result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = 0.0f;
			for (int k = 0; k < 4; k++)
			{
				// 计算矩阵相乘的结果
				result[i][j] += rows[i][k] * a.rows[k][j];
			}
		}
	}
	return result;
}

Vec4f Mat4f::operator*(Vec4f& a)
{
	// 重载运算符 *，用于矩阵与向量的乘法
	Vec4f result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 1; j++)
		{
			result[i] = 0.0f;
			for (int k = 0; k < 4; k++)
			{
				// 计算矩阵与向量相乘的结果
				result[i] += rows[i][k] * a[k];
			}
		}
	}
	return result;
}

Vec4f& Mat4f::operator[](const int i) {
	// 检查 i 是否在范围内（0-3）
	assert(i >= 0 && i < 4);

	// 返回指定行的引用
	return rows[i];
}

Vec4f Mat4f::operator[](const int i) const {
	// 检查 i 是否在范围内（0-3）
	assert(i >= 0 && i < 4);

	// 返回指定行的副本
	return rows[i];
}

Mat4f Mat4f::transpose()
{
	// 计算矩阵的转置矩阵
	Mat4f result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = rows[j][i];
		}
	return result;
}

Mat4f Mat4f::inverse()
{
	//计算矩阵的逆矩阵，这里暂时返回一个单位矩阵
	//return Mat4f::identity();

	Mat4f inv;
	float det = determinant(*this);
	if (det == 0) { // 矩阵不可逆
		return inv;
	}
	float inv_det = 1.0f / det;

	// 构造增广矩阵
	Mat4f aug = concatenate(rows[0], rows[1], rows[2], rows[3]);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			aug[i][j + 4] = (i == j) ? 1 : 0;
		}
	}

	// 高斯-约旦消元
	for (int i = 0; i < 4; i++) {
		// 高斯消元
		float pivot = aug[i][i];
		for (int j = i + 1; j < 8; j++) {
			aug[i][j] /= pivot;
		}
		aug[i][i] = 1;
		for (int j = i + 1; j < 4; j++) {
			float factor = aug[j][i];
			for (int k = i + 1; k < 8; k++) {
				aug[j][k] -= factor * aug[i][k];
			}
			aug[j][i] = 0;
		}
		// 约旦消元
		for (int j = i - 1; j >= 0; j--) {
			float factor = aug[j][i];
			for (int k = i + 1; k < 8; k++) {
				aug[j][k] -= factor * aug[i][k];
			}
			aug[j][i] = 0;
		}
	}

	// 提取逆矩阵
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			inv[i][j] = aug[i][j + 4];
			inv[i][j] *= inv_det;
		}
	}
	return inv;
}

Mat4f Mat4f::identity()
{
	// 静态方法，返回一个 4x4 的单位矩阵
	Mat4f E;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			E[i][j] = (i == j ? 1.0f : 0.0f);
		}
	return E;
}

std::ostream& operator<<(std::ostream& s, Mat4f& m)
{
	// 重载运算符 <<，用于将矩阵打印到输出流中
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			s << m[i][j];
			if (j < 3) s << "\t";
		}
		s << "\n";
	}
	return s;
}
#pragma endregion