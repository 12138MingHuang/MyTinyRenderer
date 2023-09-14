/**

@file rasterizer.h
@brief 渲染器类的头文件，负责将 3D 场景渲染到 2D 帧缓冲区中。由 ZhangBin 编写。
*/
#pragma once

#include <vector>
#include <optional>
#include <functional>
#include <limits>

#include "geometry.h"
#include "Texture.h"
#include "Shader.h"
#include "Triangle.h"

namespace rst {

	/**

	@brief 枚举类，表示可以清除的不同类型的缓冲区。
	*/
	enum class Buffers
	{
		Color = 1,
		Depth = 2
	};
	/**

	@brief 枚举类，表示可以渲染的基本图元类型。
	*/
	enum class Primitive
	{
		Line,
		Triangle
	};
	/**

	@brief 渲染器类负责将 3D 场景渲染到 2D 帧缓冲区中。
	*/
	class rasterizer
	{
	private:
		Mat4f modelMartix; // 用于变换 3D 模型的模型矩阵。
		Mat4f viewMartix; // 用于定义相机视角的视图矩阵。
		Mat4f projectionMatrix; // 用于透视变换的投影矩阵。

		std::vector<float> depth_buffer; // 用于深度缓冲的深度缓冲区。
		std::vector<float> super_depth_buffer; // 用于超采样深度缓冲的深度缓冲区。

		int width; // 帧缓冲区的宽度。
		int height; // 帧缓冲区的高度。

		std::optional<Texture> texture; // 用于纹理映射的纹理。

		std::function<Vec3f(fragment_shader_payload)> fragmentShader; // 用于着色像素的片段着色器函数。
		std::function<Vec3f(vertex_shader_payload)> vertexShader; // 用于变换顶点的顶点着色器函数。

		/**

		@brief 绘制两个点之间的直线。
		*/
		void draw_line();
		/**

		@brief 光栅化单个三角形。也就是要进行采样，可以采用包围盒采样或逐行检测采样，这里采用前者
		@param t 要光栅化的三角形。
		*/
		void rasterizer_triangle(Triangle& t);

		/**

		@brief 光栅化单个三角形。也就是要进行采样，可以采用包围盒采样或逐行检测采样，这里采用前者
		@param t 要光栅化的三角形。
		*/
		void rasterizer_triangle_msaa(Triangle& t, int sample_count);

		/**
		* @brief 光栅化单个三角形。也就是要进行采样，可以采用包围盒采样或逐行检测采样，这里采用前者。
		* @param t 要光栅化的三角形。
		* @param view_pos 三角形的三个顶点在视口坐标系中的坐标。
		* 		
		*/
		void rasterizer_triangle_new(Triangle& t, std::vector<Vec3f> view_pos);

		void rasterizer_triangle_msaa_new(Triangle& t, std::vector<Vec3f> view_pos, int sample_count);
	public:
		std::vector<Vec3f> frame_buffer; // 存储像素颜色的帧缓冲区。
		std::vector<Vec3f> super_frame_buffer; // 用于超采样的帧缓冲区。

		/**
		 * @brief 构造函数，创建指定宽度和高度的渲染器对象。
		 * @param w 帧缓冲区的宽度。
		 * @param h 帧缓冲区的高度。
		 * @param sample_count 采样点数目的平方根。默认是2。例如，如果sample_count是2，则将生成4个采样点。
		 */
		rasterizer(int w, int h, int sample_count = 2) : width(w), height(h) {
			frame_buffer.resize(w * h);
			depth_buffer.resize(w * h);
			super_frame_buffer.resize(w * h * sample_count * sample_count);
			super_depth_buffer.resize(w * h * sample_count * sample_count);
			texture = std::nullopt;
		}

		/**
		 * @brief 设置用于变换 3D 模型的模型矩阵。
		 * @param m 模型矩阵。
		 */
		void set_model(const Mat4f& m);

		/**
		 * @brief 设置用于定义相机视角的视图矩阵。
		 * @param v 视图矩阵。
		 */
		void set_view(const Mat4f& v);

		/**
		 * @brief 设置用于透视变换的投影矩阵。
		 * @param p 投影矩阵。
		 */
		void set_projection(const Mat4f& p);

		/**
		 * @brief 设置用于纹理映射的纹理。
		 * @param tex 纹理。
		 */
		void set_texture(Texture tex);

		/**
		 * @brief 清除指定的缓冲区。
		 * @param buf 要清除的缓冲区。
		 */
		void clear(Buffers buf);

		/**
		 * @brief 设置指定点的颜色。
		 * @param point 点坐标。
		 * @param color 颜色值。
		 */
		void set_pixel(Vec2i& point, Vec3f& color);

		/**
		 * @brief 设置指定点的颜色。
		 * @param point 点坐标。
		 * @param color 颜色值。
		 */
		void set_pixel(Vec2i& point, Vec3f&& color);

		/**
		 * @brief 设置用于变换顶点的顶点着色器函数。
		 * @param vert_shader 顶点着色器函数。
		 */
		void set_vertexShader(std::function<Vec3f(vertex_shader_payload)> vert_shader);

		/**
		 * @brief 设置用于着色像素的片段着色器函数。
		 * @param frag_shader 片段着色器函数。
		 */
		void set_fragmentShader(std::function<Vec3f(fragment_shader_payload)> frag_shader);

		/**
		 * @brief 3D 渲染管线中的顶点变换和光栅化阶段，主要实现了将三维模型的顶点数据转换为屏幕坐标
		 *
		 * 该函数会遍历 TriangleList 中的每个三角形，对每个三角形进行逐像素的光栅化，从而将三角形绘制到帧缓冲区中。
		 * MVP 矩阵的计算：将模型坐标系的三维坐标转换为裁剪空间的四维坐标。
		 * 透视除法的实现：将裁剪空间的坐标除以齐次坐标 w，得到归一化设备坐标。
		 * 视口变换的实现：将归一化设备坐标映射到屏幕坐标，并进行坐标系的转换。
		 * 三角形光栅化的实现：根据三角形的顶点坐标，计算其内部的像素坐标，并进行像素着色。
		 *
		 * @param TriangleList 要绘制的三角形列表。
		 */
		void draw(std::vector<Triangle>& TriangleList);

		/**
		* @brief 生成超采样的采样点向量。
		* 这个函数用于生成一个大小为sample_count*sample_count的采样点向量，其中采样点的坐标是(u, v)，而u和v的值是在0到1之间的浮点数。这个向量将被用来在三角形的像素上执行超采样，以提高渲染质量。
		* @param sample_count 采样点数目的平方根。例如，如果sample_count是2，则将生成4个采样点。
		* @return 包含所有采样点的std::vector<Vec2f>对象。
		*/
		std::vector<Vec2f> getSuperSampleStep(int sample_count = 2);

		/**
		 * @brief 计算超采样缓冲区中给定像素坐标的索引。
		 * @param x 像素在 X 轴上的坐标。
		 * @param y 像素在 Y 轴上的坐标。
		 * @param sample_count 采样点数目的平方根。默认是2。例如，如果sample_count是2，则将生成4个采样点。
		 * @return 超采样缓冲区中给定像素坐标的索引。
		 */
		int get_super_index(int x, int y, int sample_count = 2)
		{
			// 计算超采样缓冲区中给定像素坐标的索引。
			return (height - 1 - y) * width * sample_count * sample_count + x * sample_count * sample_count;
		}
	};

} // namespace rst