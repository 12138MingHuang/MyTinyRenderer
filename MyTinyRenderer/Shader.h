#pragma once

#include "geometry.h"
#include "Texture.h"

/**
 * @brief 顶点着色器输入数据结构
 */
struct vertex_shader_payload
{
	Vec3f position; // 顶点位置
};

/**
 * @brief 片元着色器输入数据结构
 */
struct fragment_shader_payload
{
	Vec3f view_pos; // 观察者在世界坐标系中的位置，用于计算光照等效果
	Vec3f color; // 片元颜色，由顶点着色器计算并传递给片元着色器
	Vec3f normal; // 片元法向量，用于计算光照等效果
	Vec2f tex_coords; // 纹理坐标，用于从纹理中采样颜色
	Texture* texture; // 指向纹理对象的指针，用于在片元着色器中对纹理进行采样
	Vec3f flatNormal; // 三角形面法向量

	/**
	 * @brief 默认构造函数
	 *
	 * 将纹理指针初始化为 nullptr，避免指针悬空
	 */
	fragment_shader_payload() : texture(nullptr) {}

	/**
	 * @brief 构造函数
	 * @param _color 片元颜色
	 * @param _normal 片元法向量
	 * @param texCoord 纹理坐标
	 * @param tex 指向纹理对象的指针
	 */
	fragment_shader_payload(const Vec3f& _color, const Vec3f& _normal, const Vec2f& texCoord, Texture* tex)
		:color(_color), normal(_normal), tex_coords(texCoord), texture(tex) {}

	/**
	 * @brief 构造函数
	 * @param _color 片元颜色
	 * @param _normal 片元法向量
	 * @param texCoord 纹理坐标
	 * @param tex 指向纹理对象的指针
	 * @param _flatNormal 三角形面法向量
	 */
	fragment_shader_payload(const Vec3f& _color, const Vec3f& _normal, const Vec2f& texCoord, Texture* tex, const Vec3f& _flatNormal)
		:color(_color), normal(_normal), tex_coords(texCoord), texture(tex), flatNormal(_flatNormal) {}
};
