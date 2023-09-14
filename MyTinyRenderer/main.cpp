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

struct light
{
	Vec3f position;//光源位置
	Vec3f intensity;//光源强度
};

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

//Phong着色
Vec3f phong_fragment_shader(const fragment_shader_payload& payload) {

	Vec3f ka = Vec3f(0.005, 0.005, 0.005);//环境光系数
	Vec3f kd = payload.color;//漫反射系数
	Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);//高光(镜面)反射系数

	auto l1 = light{ {20,20,20},{500,500,500} };
	auto l2 = light{ {-20,20,0},{500,500,500} };
	std::vector<light> lights = { l1,l2 };//光源

	Vec3f amb_light_intensity{ 10,10,10 };//环境光强度
	Vec3f eye_pos = { 0,0,10 };//相机位置

	float p = 150;//高光系数

	Vec3f color = payload.color;//顶点颜色
	Vec3f point = payload.view_pos;//顶点位置
	Vec3f normal = payload.normal;//顶点法向量

	Vec3f result_color = Vec3f(0, 0, 0);//最终颜色

	/*
	* 环境光，漫反射，镜面反射都是通过标量与标量相乘得到的；
	* 尽管使用了向量，但是向量的每个分量都是相同的(就代表是一个标量)，所以这里使用向量的逐元素乘法
	*/

	//计算环境光
	Vec3f ambient = { 0,0,0 };
	ambient = ka.cwiseProduct(amb_light_intensity); //环境光 = 环境光系数 * 环境光强度,  ka * Ia

	//漫反射
	Vec3f diffuse = { 0,0,0 };
	//镜面反射
	Vec3f specular = { 0,0,0 };

	for (auto& light : lights)
	{
		Vec3f light_dir = light.position - point;//光线方向
		float r2 = light_dir.norm() * light_dir.norm();//光线方向的模长的平方
		light_dir.normalize();//光线方向归一化

		//计算漫反射
		diffuse = diffuse + kd.cwiseProduct(light.intensity / r2) * std::max(0.f, normal * light_dir); //漫反射 = 漫反射系数 * (光强度 / 光线方向的模长的平方) * (法向量 * 光线方向),  kd * (I/r^2)*max(0,n*l)

		Vec3f view_dir = (eye_pos - point).normalize();//视线方向
		Vec3f half_dir = (light_dir + view_dir).normalize();//半程向量

		//计算镜面反射
		specular = specular + ks.cwiseProduct(light.intensity / r2) * std::pow(std::max(0.f, normal * half_dir), p); //镜面反射 = 镜面反射系数 * (光强度 / 光线方向的模长的平方) * (法向量 * 半程向量)^p  , ks * (I/r^2)*max(0,n*h)^p
	}

	result_color = ambient + diffuse + specular; //最终颜色 = 环境光 + 漫反射 + 镜面反射

	return result_color;
}

//纹理着色
Vec3f texture_fragment_shader(const fragment_shader_payload& payload) {
	Vec3f return_color = { 0, 0, 0 };
	if (payload.texture)
	{
		// 从纹理中获取颜色
		return_color = Vec3f(payload.texture->getColor(payload.tex_coords.x, payload.tex_coords.y).bgra[2] / 255.0f,
			payload.texture->getColor(payload.tex_coords.x, payload.tex_coords.y).bgra[1] / 255.0f,
			payload.texture->getColor(payload.tex_coords.x, payload.tex_coords.y).bgra[0] / 255.0f);
	}
	Vec3f texture_color;
	texture_color = return_color * 255;

	Vec3f ka = Vec3f(0.005, 0.005, 0.005);//环境光系数
	Vec3f kd = texture_color / 255.f;//漫反射系数
	Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);//高光(镜面)反射系数

	auto l1 = light{ {20,20,20},{500,500,500} };
	auto l2 = light{ {-20,20,0},{500,500,500} };
	std::vector<light> lights = { l1,l2 };//光源

	Vec3f amb_light_intensity{ 10,10,10 };//环境光强度
	Vec3f eye_pos = { 0,0,10 };//相机位置

	float p = 150;//高光系数

	Vec3f color = payload.color;//顶点颜色
	Vec3f point = payload.view_pos;//顶点位置
	Vec3f normal = payload.normal;//顶点法向量

	Vec3f result_color = Vec3f(0, 0, 0);//最终颜色

	//计算环境光
	Vec3f ambient = { 0,0,0 };
	ambient = ka.cwiseProduct(amb_light_intensity); //环境光 = 环境光系数 * 环境光强度,  ka * Ia

	//漫反射
	Vec3f diffuse = { 0,0,0 };
	//镜面反射
	Vec3f specular = { 0,0,0 };

	for (auto& light : lights)
	{
		Vec3f light_dir = light.position - point;//光线方向
		float r2 = light_dir.norm() * light_dir.norm();//光线方向的模长的平方
		light_dir.normalize();//光线方向归一化

		//计算漫反射
		diffuse = diffuse + kd.cwiseProduct(light.intensity / r2) * std::max(0.f, normal * light_dir); //漫反射 = 漫反射系数 * (光强度 / 光线方向的模长的平方) * (法向量 * 光线方向),  kd * (I/r^2)*max(0,n*l)

		Vec3f view_dir = (eye_pos - point).normalize();//视线方向
		Vec3f half_dir = (light_dir + view_dir).normalize();//半程向量

		//计算镜面反射
		specular = specular + ks.cwiseProduct(light.intensity / r2) * std::pow(std::max(0.f, normal * half_dir), p); //镜面反射 = 镜面反射系数 * (光强度 / 光线方向的模长的平方) * (法向量 * 半程向量)^p  , ks * (I/r^2)*max(0,n*h)^p
	}

	result_color = ambient + diffuse + specular; //最终颜色 = 环境光 + 漫反射 + 镜面反射

	return result_color;
}

//凹凸纹理着色
Vec3f bump_fragment_shader(const fragment_shader_payload& payload)
{

	Vec3f ka = Vec3f(0.005, 0.005, 0.005);//环境光系数
	Vec3f kd = payload.color;//漫反射系数
	Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);//高光(镜面)反射系数

	auto l1 = light{ {20,20,20},{500,500,500} };
	auto l2 = light{ {-20,20,0},{500,500,500} };

	std::vector<light> lights = { l1,l2 };//光源
	Vec3f amb_light_intensity{ 10,10,10 };//环境光强度
	Vec3f eye_pos = { 0,0,10 };//相机位置

	float p = 150;//高光系数

	Vec3f color = payload.color;//顶点颜色
	Vec3f point = payload.view_pos;//顶点位置
	Vec3f normal = payload.normal;//顶点法向量

	// 设置凹凸贴图参数
	float kh = 0.2, kn = 0.1;
	float u = payload.tex_coords.x;
	float v = payload.tex_coords.y;
	float w = payload.texture->width;
	float h = payload.texture->height;

	// 计算切线（t）、副切线（b）和法线（normal）的 TBN 矩阵
	auto [x, y, z] = std::tuple{ normal[0], normal[1], normal[2] };
	Vec3f t = { x * y / sqrt(x * x + z * z), sqrt(x * x + z * z), z * y / sqrt(x * x + z * z) };
	Vec3f b = normal ^ t;
	Mat3f TBN;
	TBN << t, b, normal;

	// 计算法线贴图的偏移量以更新法线
	float dU = kh * kn * (payload.texture->getColor(u + 1.0f / w, v).bgra[2] - payload.texture->getColor(u, v).bgra[2]);
	float dV = kh * kn * (payload.texture->getColor(u, v + 1.0f / h).bgra[2] - payload.texture->getColor(u, v).bgra[2]);
	Vec3f ln = { -dU, -dV, 1 };
	normal = (TBN * ln).normalize();

	Vec3f result_color = { 0, 0, 0 };
	result_color = normal;

	return result_color * 255.f;
}

//凹凸纹理着色(也计算了环境光、漫反射和镜面反射的贡献，但它额外考虑了法线贴图对顶点位置的影响。这使得它可以模拟出更真实的表面细节，例如表面的凹凸。)
Vec3f displacement_fragment_shader(const fragment_shader_payload& payload)
{

	Vec3f ka = Vec3f(0.005, 0.005, 0.005);//环境光系数
	Vec3f kd = payload.color;//漫反射系数
	Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);//高光(镜面)反射系数

	auto l1 = light{ {20,20,20},{500,500,500} };
	auto l2 = light{ {-20,20,0},{500,500,500} };

	std::vector<light> lights = { l1,l2 };//光源
	Vec3f amb_light_intensity{ 10,10,10 };//环境光强度
	Vec3f eye_pos = { 0,0,10 };//相机位置

	float p = 150;//高光系数

	Vec3f color = payload.color;//顶点颜色
	Vec3f point = payload.view_pos;//顶点位置
	Vec3f normal = payload.normal;//顶点法向量

	// 设置凹凸贴图参数
	float kh = 0.2, kn = 0.1;
	float u = payload.tex_coords.x;
	float v = payload.tex_coords.y;
	float w = payload.texture->width;
	float h = payload.texture->height;

	// 计算切线（t）、副切线（b）和法线（normal）的 TBN 矩阵
	auto [x, y, z] = std::tuple{ normal[0], normal[1], normal[2] };
	Vec3f t = { x * y / sqrt(x * x + z * z), sqrt(x * x + z * z), z * y / sqrt(x * x + z * z) };
	Vec3f b = normal ^ t;
	Mat3f TBN;
	TBN << t, b, normal;

	// 计算法线贴图的偏移量以更新法线
	float dU = kh * kn * (payload.texture->getColor(u + 1.0f / w, v).bgra[2] - payload.texture->getColor(u, v).bgra[2]);
	float dV = kh * kn * (payload.texture->getColor(u, v + 1.0f / h).bgra[2] - payload.texture->getColor(u, v).bgra[2]);
	Vec3f ln = { -dU, -dV, 1 };
	point = point + normal * kn * payload.texture->getColor(u, v).bgra[2];
	normal = (TBN * ln).normalize();

	Vec3f result_color = { 0, 0, 0 };

	//计算环境光
	Vec3f ambient = { 0,0,0 };
	ambient = ka.cwiseProduct(amb_light_intensity); //环境光 = 环境光系数 * 环境光强度,  ka * Ia

	//漫反射
	Vec3f diffuse = { 0,0,0 };
	//镜面反射
	Vec3f specular = { 0,0,0 };

	for (auto& light : lights)
	{
		Vec3f light_dir = light.position - point;//光线方向
		float r2 = light_dir.norm() * light_dir.norm();//光线方向的模长的平方
		light_dir.normalize();//光线方向归一化

		//计算漫反射
		diffuse = diffuse + kd.cwiseProduct(light.intensity / r2) * std::max(0.f, normal * light_dir); //漫反射 = 漫反射系数 * (光强度 / 光线方向的模长的平方) * (法向量 * 光线方向),  kd * (I/r^2)*max(0,n*l)

		Vec3f view_dir = (eye_pos - point).normalize();//视线方向
		Vec3f half_dir = (light_dir + view_dir).normalize();//半程向量

		//计算镜面反射
		specular = specular + ks.cwiseProduct(light.intensity / r2) * std::pow(std::max(0.f, normal * half_dir), p); //镜面反射 = 镜面反射系数 * (光强度 / 光线方向的模长的平方) * (法向量 * 半程向量)^p  , ks * (I/r^2)*max(0,n*h)^p
	}

	result_color = ambient + diffuse + specular; //最终颜色 = 环境光 + 漫反射 + 镜面反射

	return result_color;
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
	//r.set_fragmentShader(normal_fragment_shader); //法线着色
	//r.set_fragmentShader(F_fragment_shader); //Flat着色
	//r.set_fragmentShader(G_fragment_shader); //Gouraud着色
	//r.set_fragmentShader(phong_fragment_shader); //Phong着色
	//r.set_fragmentShader(texture_fragment_shader); //纹理着色
	//r.set_fragmentShader(bump_fragment_shader); //凹凸纹理着色
	//r.set_fragmentShader(displacement_fragment_shader); //凹凸纹理着色

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