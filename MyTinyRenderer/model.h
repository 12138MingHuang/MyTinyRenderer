#pragma once

#include <vector>

#include "geometry.h"
#include "tgaimage.h"
#include "Texture.h"
#include "Triangle.h"

class Model {
private:
	//一张可能存在的纹理
	Texture* tex = nullptr;
	// 顶点数量和面片数量
	int vertNum, faceNum;
public:
	//所有三角面片组成的数组
	std::vector<Triangle> TriangleList;

	//根据.obj文件路径导入模型
	Model(const char* filename);
	// 析构函数，用于释放模型资源
	~Model();
	//返回模型顶点数量
	int nverts();
	//返回模型面片数量
	int nfaces();
};