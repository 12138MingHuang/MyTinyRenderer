#pragma once

#include "geometry.h"
#include "Texture.h"

class Triangle {
private:

public:
	Vec4f v[3];// 三个顶点坐标（已经定义为齐次坐标，所以是Vec4f）
	Vec3f normal[3];// 三个顶点的法向量
	Vec2f texCoords[3];// 三个顶点的纹理坐标
	Vec3f color[3];// 三个顶点的颜色值
	Vec3f flatNormal;// 计算三角形的法向量

	Triangle();

	/**
	 * 计算三角形顶点颜色的 Gouraud Shading 算法。
	 * 对于每个顶点，计算光线方向与法向量之间的夹角，并将结果作为强度值。
	 * 使用强度值和固定颜色值计算顶点颜色。
	 *
	 * @param light_dir 光线方向向量
	 *
	 * @return 无返回值，但会更新三角形顶点的颜色值
	*/
	void computeGColor(Vec3f light_dir);

	/**
	 * 计算三角形顶点颜色的 Flat Shading 算法。
	 * 对于每个三角形面，计算光线方向与法向量之间的夹角，并将结果作为强度值。
	 * 使用强度值和固定颜色值计算顶点颜色。
	 *
	 * @param light_dir 光线方向向量
	 *
	 * @return 无返回值，但会更新三角形顶点的颜色值
	*/
	void computeFColor(Vec3f light_dir);

	/**
	* 计算并且获取三角形的法向量
	*/
	void setFlatNormal();
};