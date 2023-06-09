#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "model.h"


Model::Model(const char* filename) :vertNum(0), faceNum(0) {
	std::vector<Vec4f> verts_; // 存储模型的所有顶点
	std::vector<std::vector<Vec3i>> faces_; // 存储模型的所有面（由三个顶点构成）
	std::vector<Vec3f> norms_; // 存储模型的所有法向量
	std::vector<Vec2f> uv_; // 存储模型的所有纹理坐标

	std::ifstream in; // 创建一个输入文件流
	in.open(filename, std::ifstream::in); // 打开模型文件
	if (in.fail()) return; // 如果打开失败，则直接返回
	std::string line; // 创建一个字符串变量，用于存储每行读取的内容
	while (!in.eof()) { // 逐行读取模型文件
		std::getline(in, line); // 获取当前行的内容
		std::istringstream iss(line.c_str()); // 将当前行的内容转换为字符串流
		char trash; // 用于存储无用字符
		if (!line.compare(0, 2, "v ")) { // 如果当前行以"v "开头，则表示该行为顶点数据
			iss >> trash; // 读取无用字符"v"
			Vec4f v; // 创建一个四维向量，用于存储顶点坐标
			for (int i = 0; i < 3; i++) iss >> v[i]; // 依次读取顶点坐标的x、y、z分量
			v[3] = 1.0f; // 将顶点坐标的w分量设置为1
			verts_.push_back(v); // 将当前顶点坐标添加到顶点列表中
		}
		else if (!line.compare(0, 3, "vt ")) { // 如果当前行以"vt "开头，则表示该行为纹理坐标数据
			iss >> trash >> trash; // 读取无用字符"vt"
			Vec2f uv; // 创建一个二维向量，用于存储纹理坐标
			for (int i = 0; i < 2; i++) iss >> uv[i]; // 依次读取纹理坐标的u、v分量
			uv_.push_back(uv); // 将当前纹理坐标添加到纹理坐标列表中
		}
		else if (!line.compare(0, 3, "vn ")) { // 如果当前行以"vn "开头，则表示该行为法向量数据
			iss >> trash >> trash; // 读取无用字符"vn"
			Vec3f normal; // 创建一个三维向量，用于存储法向量
			for (int i = 0; i < 3; i++) iss >> normal[i]; // 依次读取法向量的x、y、z分量
			norms_.push_back(normal); // 将当前法向量添加到法向量列表中
		}
		else if (!line.compare(0, 2, "f ")) { // 如果当前行以"f "开头，则表示该行为面数据
			std::vector<Vec3i> f; // 创建一个向量列表，用于存储面的三个顶点
			Vec3i tmp; // 创建一个三维向量，用于临时存储每个顶点的索引
			iss >> trash; // 读取无用字符"f"
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) { // 依次读取每个顶点的索引
				for (int i = 0; i < 3; i++) tmp[i]--; // 将顶点索引减1（因为顶点索引是从1开始的，而存储时需要从0开始）
				f.push_back(tmp); // 将当前顶点索引添加到当前面的顶点列表中
			}
			faces_.push_back(f); // 将当前面添加到面列表中
		}
	}

	// 在标准错误流中打印了模型中顶点、面、纹理坐标和法向量的数量。
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;


// 创建一个TriangleList向量，其大小为面的数量。
TriangleList.resize(faces_.size());
// 清空该列表
TriangleList.clear();

// 对于每个面，将其顶点、纹理坐标和法向量存储到一个Triangle对象中，并将该对象添加到TriangleList向量中。
for (int i = 0; i < faces_.size(); i++)
{
	Triangle tmp;

	std::vector<Vec3i> face = faces_[i];
	Vec3i face_vertex_1 = face[0];
	Vec3i face_vertex_2 = face[1];
	Vec3i face_vertex_3 = face[2];

	tmp.v[0] = verts_[face_vertex_1.x];
	tmp.v[1] = verts_[face_vertex_2.x];
	tmp.v[2] = verts_[face_vertex_3.x];

	tmp.texCoords[0] = uv_[face_vertex_1.y];
	tmp.texCoords[1] = uv_[face_vertex_2.y];
	tmp.texCoords[2] = uv_[face_vertex_3.y];

	tmp.normal[0] = norms_[face_vertex_1.z];
	tmp.normal[1] = norms_[face_vertex_2.z];
	tmp.normal[2] = norms_[face_vertex_3.z];

	TriangleList.push_back(tmp);

	// 可选：打印Triangle对象的顶点信息。
	//for (int j = 0; j < 3; j++)
		//std::cout << TriangleList[i].v[j] << std::endl;
}

// 设置vertNum和faceNum成员变量，分别表示模型中顶点和面的数量。
vertNum = static_cast<int>(verts_.size());
faceNum = static_cast<int>(faces_.size());
}

	// 这是Model类的析构函数，没有实现任何功能。
	Model::~Model() {
	}

	// 返回模型中顶点的数量。
	int Model::nverts() {
		return vertNum;
	}

	// 返回模型中面的数量。
	int Model::nfaces() {
		return faceNum;
	}