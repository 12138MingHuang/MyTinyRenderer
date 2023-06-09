#pragma once

#include <fstream>

#pragma pack(push,1)
struct TGA_Header {
    char idlength; // 图像ID的长度
    char colormaptype; // 调色板类型，0为无调色板，1为有调色板
    char datatypecode; // 图像数据类型，2、3为无压缩，10、11为RLE压缩
    short colormaporigin; // 调色板在文件中的起始位置
    short colormaplength; // 调色板中颜色的数量
    char colormapdepth; // 调色板中每个颜色占用的位数
    short x_origin; // 图像左下角的x坐标，通常为0
    short y_origin; // 图像左下角的y坐标，通常为0
    short width; // 图像宽度
    short height; // 图像高度
    char bitsperpixel; // 每个像素占用的位数，通常为8、16、24或32
    char imagedescriptor; // 图像描述符，通常为0
};
#pragma pack(pop)

// TGA图像中的颜色类
struct TGAColor {
    unsigned char bgra[4]; // 颜色的BGR值，以及一个额外的alpha值
    unsigned char bytespp; // 每个像素占用的字节数

    // 构造函数，初始化颜色值为0
    TGAColor() : bgra(), bytespp(1) {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
    }

    // 构造函数，根据RGB值以及alpha值初始化颜色
    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : bgra(), bytespp(4) {
        bgra[0] = B;
        bgra[1] = G;
        bgra[2] = R;
        bgra[3] = A;
    }

    // 构造函数，根据单一值初始化颜色
    TGAColor(unsigned char v) : bgra(), bytespp(1) {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
        bgra[0] = v;
    }

    // 构造函数，根据字节数和指针初始化颜色
    TGAColor(const unsigned char* p, unsigned char bpp) : bgra(), bytespp(bpp) {
        for (int i = 0; i < (int)bpp; i++) {
            bgra[i] = p[i];
        }
        for (int i = bpp; i < 4; i++) {
            bgra[i] = 0;
        }
    }

    // 颜色乘以一个浮点数，用于调整颜色强度
    TGAColor operator *(float intensity) const {
        TGAColor res = *this;
        intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity)); // 限制强度在0到1之间
        for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
        return res;
    }
};

// TGA图像处理类
class TGAImage {
protected:
    unsigned char* data; // 存储图像数据的缓冲区指针
    int width; // 图像宽度
    int height; // 图像高度
    int bytespp; // 每个像素占用的字节数

    // 读取RLE压缩的图像数据
    bool load_rle_data(std::ifstream& in);

    // 将图像数据写入到文件中，使用RLE压缩
    bool unload_rle_data(std::ofstream& out);
public:
    enum Format {
        GRAYSCALE = 1, RGB = 3, RGBA = 4// 图像格式
    };

    // 构造函数，创建一个空图像
    TGAImage();

    // 构造函数，创建指定大小的图像
    TGAImage(int w, int h, int bpp);

    // 拷贝构造函数，复制另一个图像
    TGAImage(const TGAImage& img);

    // 从文件中读取TGA图像数据
    bool read_tga_file(const char* filename);

    // 将TGA图像数据写入到文件中,默认RLE压缩
    bool write_tga_file(const char* filename, bool rle = true);

    // 水平翻转图像
    bool flip_horizontally();

    // 垂直翻转图像
    bool flip_vertically();

    // 缩放图像至指定大小
    bool scale(int w, int h);

    // 获取指定像素的颜色值
    TGAColor get(int x, int y);

    // 设置指定像素的颜色值
    bool set(int x, int y, TGAColor& c);

    // 设置指定像素的颜色值
    bool set(int x, int y, const TGAColor& c);

    // 析构函数，释放图像数据缓冲区
    ~TGAImage();

    // 拷贝赋值，复制另一个图像
    TGAImage& operator =(const TGAImage& img);

    // 获取图像宽度
    int get_width();

    // 获取图像高度
    int get_height();

    // 获取每个像素占用的字节数
    int get_bytespp();

    // 获取图像数据缓冲区指针
    unsigned char* buffer();

    // 清空图像数据缓冲区
    void clear();
};