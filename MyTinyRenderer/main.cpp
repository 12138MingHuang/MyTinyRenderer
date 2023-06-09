#include <iostream>
#include "geometry.h"
#include "model.h"
#include "Shader.h"
#include "rasterizer.h"
#include "camera.h"

const int width = 800;
const int height = 800;

Model* model = nullptr;

Vec3f eye_position(1.f, 1.f, 3.f);//相机摆放的位置
Vec3f center(0.f, 0.f, 0.f);//相机中心指向center
Camera camera(eye_position, Vec3f(0.f, 1.f, 0.f), center - eye_position);
Vec3f light_dir(0, 0, 1);//平行光方向

Vec3f cameraPos(0, 0, 3);//相机摆放的位置
Vec3f lightDir(0, 0, -1);//平行光方向


//模型变换矩阵
Mat4f modelMatrix()
{
	return Mat4f::identity();
}

//视图变换矩阵
Mat4f viewMatrix1()
{
	return Mat4f::identity();
}
//视图变换矩阵
Mat4f viewMatrix()
{
	Mat4f view = Mat4f::identity();
	Mat4f r_inverse = Mat4f::identity();
	Mat4f t_inverse = Mat4f::identity();
	for (int i = 0; i < 3; i++) {
		r_inverse[0][i] = camera.Right[i];
		r_inverse[1][i] = camera.Up[i];
		r_inverse[2][i] = -camera.Front[i];

		t_inverse[i][3] = -camera.Position[i];
	}
	view = r_inverse * t_inverse;
	return view;
}

//透视投影变换矩阵
Mat4f projectionMatrix()
{
	Mat4f projection = Mat4f::identity();
	projection[3][2] = -1.0f / eye_position.z;
	//projection[3][2] = -1.0f / cameraPos.z;
	return projection;
}

//定义顶点着色器
Vec3f vertex_shader(const vertex_shader_payload& payload) {
	return payload.position;
}

//定义片元着色器函数
Vec3f normal_fragment_shader(const fragment_shader_payload& payload)
{
	Vec3f normal_frag = payload.normal;
	Vec3f return_color = (normal_frag.normalize() + Vec3f(1.0f, 1.0f, 1.0f)) * 0.5;

	return Vec3f(return_color.x * 255, return_color.y * 255, return_color.z * 255);
}

//Flat着色
Vec3f F_fragment_shader(const fragment_shader_payload& payload) {
	Vec3f color_frag;
	Vec3f flatNormal_frag = payload.flatNormal;
	float intensity = std::max(0.f, flatNormal_frag.normalize() * light_dir.normalize());
	// 使用强度值和固定颜色值计算顶点颜色。
	color_frag = Vec3f(255, 255, 255) * intensity;
	return color_frag;
}

//Gouraud着色
Vec3f G_fragment_shader(const fragment_shader_payload& payload) {
	Vec3f color_frag;
	Vec3f normal_frag = payload.normal;

	/*
	计算法向量与光线方向之间的夹角, 余弦值越大，表示法向量和光源方向越接近，顶点的光照强度就越高。
	由于余弦值的范围是[-1, 1]，为了将其转换为颜色强度，我们需要将其映射到[0, 1]的范围内。
	具体来说，我们可以使用 std::max(0.f, ...) 将余弦值和0取最大值，以确保强度值不会小于0。
	*/
	float intensity = std::max(0.f, normal_frag.normalize() * light_dir.normalize());
	// 使用强度值和固定颜色值计算顶点颜色。
	color_frag = Vec3f(255, 255, 255) * intensity;
	return color_frag;
}


int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("res/objs/african_head.obj");
	}

	std::cout << model->nfaces() << " " << model->nverts() << std::endl;

	//创建TGA图像
	TGAImage image(width, height, TGAImage::Format::RGB);

	//存储所有需要绘制的三角形面片

	//创建光栅化对象
	rst::rasterizer r(width, height, 4);

	//给定纹理并且设置
	Texture tex("res/objs/african_head_diffuse.tga");
	r.set_texture(tex);

	//清空帧缓冲和zBuffer
	r.clear(rst::Buffers::Color);
	r.clear(rst::Buffers::Depth);

	//设置MVP矩阵
	r.set_model(modelMatrix());
	r.set_view(viewMatrix());
	r.set_projection(projectionMatrix());

	//设置顶点着色器和片元着色器
	r.set_vertexShader(vertex_shader);
	//r.set_fragmentShader(normal_fragment_shader);
	//r.set_fragmentShader(F_fragment_shader);
	r.set_fragmentShader(G_fragment_shader);

	//绘制模型
	r.draw(model->TriangleList);

	//将frame_buffer帧缓冲中的颜色值写入image中
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Vec3f color = r.frame_buffer[j * width + i];
			image.set(i, j, TGAColor(color.x, color.y, color.z, 255));
		}
	}
	image.flip_vertically();
	image.write_tga_file("output.tga");
	delete model;
}