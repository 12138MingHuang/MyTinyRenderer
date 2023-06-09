#pragma once

#include "tgaimage.h"

class Texture {
private:
	TGAImage image_data;
public:
	int width, height;// 贴图纹理的宽与高
	//加载图片纹理
	Texture(const char* filename) {
		image_data.read_tga_file(filename);
		image_data.flip_vertically();

		width = image_data.get_width();
		height = image_data.get_height();
	}
	//获取贴图纹理的uv坐标上的颜色
	TGAColor getColor(float u, float v) {
		return image_data.get(u, v);
	}
};