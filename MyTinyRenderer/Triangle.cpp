#include <iostream>
#include "Triangle.h"

Triangle::Triangle() {
	v[0] = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
	v[2] = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);

	color[0] = Vec3f(0.0f, 0.0f, 0.0f);
	color[1] = Vec3f(0.0f, 0.0f, 0.0f);
	color[2] = Vec3f(0.0f, 0.0f, 0.0f);

	texCoords[0] = Vec2f(0.0f, 0.0f);
	texCoords[1] = Vec2f(0.0f, 0.0f);

	normal[0] = Vec3f(0.0f, 0.0f, 0.0f);
	normal[1] = Vec3f(0.0f, 0.0f, 0.0f);
	normal[2] = Vec3f(0.0f, 0.0f, 0.0f);

	flatNormal = Vec3f(0.0f, 0.0f, 0.0f);
}

void Triangle::computeGColor(Vec3f light_dir)
{
	// 对于每个顶点，计算光线方向与法向量之间的夹角，并将结果作为强度值。
	for (int i = 0; i < 3; i++)
	{
		/*
		计算法向量与光线方向之间的夹角, 余弦值越大，表示法向量和光源方向越接近，顶点的光照强度就越高。
		由于余弦值的范围是[-1, 1]，为了将其转换为颜色强度，我们需要将其映射到[0, 1]的范围内。
		具体来说，我们可以使用 std::max(0.f, ...) 将余弦值和0取最大值，以确保强度值不会小于0。
		*/
		float intensity = std::max(0.f, normal[i].normalize() * light_dir.normalize());

		// 使用强度值和固定颜色值计算顶点颜色。
		color[i] = Vec3f(255, 255, 255) * intensity;
	}
}

// 计算局部颜色的函数，需要输入光线方向light_dir。
void Triangle::computeFColor(Vec3f light_dir)
{
	// 计算三角形的法向量。
	Vec3f normal_ = (Vec3f(v[0].x, v[0].y, v[0].z) - Vec3f(v[2].x, v[2].y, v[2].z)) ^ (Vec3f(v[1].x, v[1].y, v[1].z) - Vec3f(v[0].x, v[0].y, v[0].z));
	normal_.normalize();

	// 对于每个顶点，计算光线方向与法向量之间的夹角，并将结果作为强度值。
	float intensity = std::max(0.f, normal_ * light_dir.normalize());
	for (int i = 0; i < 3; i++)
	{
		// 使用强度值和固定颜色值计算顶点颜色。
		color[i] = Vec3f(255, 255, 255) * intensity;
	}
}

void Triangle::setFlatNormal() {
	// 计算三角形的法向量。
	Vec3f normal_ = (Vec3f(v[0].x, v[0].y, v[0].z) - Vec3f(v[2].x, v[2].y, v[2].z)) ^ (Vec3f(v[1].x, v[1].y, v[1].z) - Vec3f(v[0].x, v[0].y, v[0].z));
	flatNormal = normal_.normalize();
}

void Triangle::setVertex(int ind, Vec4f ver) {
	v[ind] = ver;
}
void Triangle::setNormal(int ind, Vec3f n) {
	normal[ind] = n;
}

void Triangle::setColor(int ind, float r, float g, float b) {
	if ((r < 0.0) || (r > 255.) ||
		(g < 0.0) || (g > 255.) ||
		(b < 0.0) || (b > 255.)) {
		fprintf(stderr, "ERROR! Invalid color values");
		fflush(stderr);
		exit(-1);
	}

	color[ind] = Vec3f((float)r / 255., (float)g / 255., (float)b / 255.);
	return;
}