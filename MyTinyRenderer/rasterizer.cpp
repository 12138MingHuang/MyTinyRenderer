#include <algorithm>
#include "rasterizer.h"
#include <iostream>

void rst::rasterizer::set_model(const Mat4f& m) {
	modelMartix = m;
}

void rst::rasterizer::set_view(const Mat4f& v) {
	viewMartix = v;
}

void rst::rasterizer::set_projection(const Mat4f& p) {
	projectionMatrix = p;
}

void rst::rasterizer::set_texture(Texture tex) {
	texture = tex;
}

void rst::rasterizer::clear(Buffers buf) {
    if (buf == Buffers::Color) {
        // 如果要清空颜色缓冲区，将帧缓冲区的所有像素颜色设置为黑色（RGB值为(0,0,0)）。
        std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3f(.0f, .0f, .0f));
        // 如果要清空超采样颜色缓冲区，将超采样帧缓冲区的所有像素颜色设置为黑色（RGB值为(0,0,0)）。
        std::fill(super_frame_buffer.begin(), super_frame_buffer.end(), Vec3f(.0f, .0f, .0f));
    }
    else if (buf == Buffers::Depth) {
        // 如果要清空深度缓冲区，将深度缓冲区的所有像素深度设置为负无穷大（-∞）。
        // 这样做是为了确保在渲染场景时所有像素都可以被覆盖，因为深度测试会使用深度缓冲区的值来判断像素是否被覆盖。
        std::fill(depth_buffer.begin(), depth_buffer.end(), -std::numeric_limits<float>::infinity());
        // 如果要清空超采样深度缓冲区，将超采样深度缓冲区的所有像素深度设置为负无穷大（-∞）。
        // 这样做是为了确保在渲染场景时所有像素都可以被覆盖，因为深度测试会使用超采样深度缓冲区的值来判断像素是否被覆盖。
        std::fill(super_depth_buffer.begin(), super_depth_buffer.end(), -std::numeric_limits<float>::infinity());
    }
}

void rst::rasterizer::set_pixel(Vec2i& point, Vec3f& color) {
    // 计算像素在帧缓冲区中的索引，方法是将像素在 Y 轴方向上的坐标乘以帧缓冲区的宽度，再加上像素在 X 轴方向上的坐标。
    int idx = point.y * width + point.x;
    // 将索引为 idx 的帧缓冲区元素的值设置为 color，从而将像素的颜色设置为指定的颜色。
    frame_buffer[idx] = color;
}

void rst::rasterizer::set_pixel(Vec2i& point, Vec3f&& color) {
    // 计算像素在帧缓冲区中的索引，方法是将像素在 Y 轴方向上的坐标乘以帧缓冲区的宽度，再加上像素在 X 轴方向上的坐标。
    int idx = point.y * width + point.x;
    // 将索引为 idx 的帧缓冲区元素的值设置为 color，从而将像素的颜色设置为指定的颜色。
    frame_buffer[idx] = color;
}

void rst::rasterizer::set_fragmentShader(std::function<Vec3f(fragment_shader_payload)> frag_shader) {
    fragmentShader = frag_shader;
}

void rst::rasterizer::set_vertexShader(std::function<Vec3f(vertex_shader_payload)> vert_shader) {
    vertexShader = vert_shader;
}

std::vector<Vec2f> rst::rasterizer::getSuperSampleStep(int sampl_count) {
    std::vector<Vec2f> steps;

    for (int x = 0; x < sampl_count; x++) {
        for (int y = 0; y < sampl_count; y++)
        {
            // 得到每个小像素点的坐标
            float u = (x + 0.5f) / sampl_count;
            float v = (y + 0.5f) / sampl_count;
            steps.emplace_back(u, v);
        }
    }
    return steps;
}

void rst::rasterizer::draw(std::vector<Triangle>& TriangleList) {
    // 这里其实是(f-n)/2    (f+n)/2,将n设为0，f设为255
    float f1 = (255 - .0) / 2.;
    float f2 = (255 + .0) / 2.;

    // 计算MVP矩阵
    Mat4f mvp = projectionMatrix * viewMartix * modelMartix;

    // 遍历三角形列表
    for (auto& t : TriangleList) {
        // 初始化深度值和新三角形
        int depth = 255;// 认为n = 0.0f, f = 255.0f
        Triangle newtri = t;

        std::vector<Vec4f> mm {
            (viewMartix * modelMartix * t.v[0]),
            (viewMartix * modelMartix * t.v[1]),
            (viewMartix * modelMartix * t.v[2])
        };

        std::vector<Vec3f> viewspace_pos;

        for (const Vec4f& v : mm) {
            viewspace_pos.push_back(Vec3f(v.x, v.y, v.z));
        }


        // 计算经过MVP变换的点，将三角形的顶点转化为Vec4f类型，并存储在数组v中
        Vec4f v[3];
        for (int i = 0; i < 3; i++)
        {
            v[i] = t.v[i];
        }
        // 将顶点坐标乘以MVP矩阵，得到CVV裁剪空间
        for (auto& vec : v) {
            Vec4f tmp;
            tmp = mvp * vec;
            vec.x = tmp[0];
            vec.y = tmp[1];
            vec.z = tmp[2];
            vec.w = tmp[3];
        }

        // 进行透视除法，将每个顶点坐标归一化，即将其除以其对应的w分量，将坐标转化到标准化设备坐标系(NDC)
        for (auto& vec : v)
        {
            vec.x = vec.x / vec.w;
            vec.y = vec.y / vec.w;
            vec.z = vec.z / vec.w;
        }

        // 视口变换，将每个顶点坐标从NDC空间转换到屏幕空间
        for (auto& vec : v)
        {
            // 屏幕宽度
            float w = width * 3.f / 4.f;
            // 屏幕高度
            float h = height * 3.f / 4.f;
            // 屏幕左下角,x_offset 和 y_offset 是微调因子，用于微调物体在屏幕上的位置。将它们分别加到 x_pixel 和 y_pixel 上，得到物体在屏幕上的实际位置。
            float x_offset = width / 8.f;
            float y_offset = height / 8.f;

            // 计算屏幕坐标
            vec.x = w / 2.f * vec.x + w / 2.f + x_offset;
            vec.y = h / 2.f * vec.y + h / 2.f + y_offset;
            vec.z = vec.z * f1 + f2;
        }

        // 将新的顶点坐标存储在新的三角形中
        for (int i = 0; i < 3; i++) {
            newtri.v[i] = v[i];
        }


        //newtri.computeFColor({ 1,0,0 });
        //newtri.computeGColor({ 1,0,0 });
        //newtri.setFlatNormal();
        newtri.setColor(0, 148, 121.0, 92.0);
        newtri.setColor(1, 148, 121.0, 92.0);
        newtri.setColor(2, 148, 121.0, 92.0);


        // 光栅化新三角形，生成最终的图像
        //rasterizer_triangle(newtri);
        //rasterizer_triangle_msaa(newtri,2);
        //rasterizer_triangle_new(newtri,viewspace_pos);
        rasterizer_triangle_msaa_new(newtri, viewspace_pos, 2);
    }
}

/**
 * @brief 计算点(x,y)在三角形内的重心坐标
 *
 * @param pts 三角形的三个顶点，类型为Vec4f结构体数组
 * @param x 待计算重心坐标的点的x坐标
 * @param y 待计算重心坐标的点的y坐标
 * @return 一个tuple类型的值，包含三个浮点数alpha、beta和gamma，它们分别表示点(x,y)在三角形内的三个顶点上的重心坐标
 */
static std::tuple<float, float, float> computeBarycentric2D(const Vec4f* pts, float x, float y) {
    // 从Vec4f结构体数组pts中获取三角形的三个顶点坐标
    float xa = pts[0].x;
    float ya = pts[0].y;
    float xb = pts[1].x;
    float yb = pts[1].y;
    float xc = pts[2].x;
    float yc = pts[2].y;

    // 计算点(x,y)在三角形内的重心坐标alpha、beta和gamma
    float gamma = ((ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya) / ((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
    float beta = ((ya - yc) * x + (xc - xa) * y + xa * yc - xc * ya) / ((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
    float alpha = 1 - gamma - beta;

    // 将alpha、beta和gamma作为tuple类型的值返回
    return { alpha, beta, gamma };
}


//void rst::rasterizer::rasterizer_triangle_msaa(Triangle& t, int sample_count = 2) {
//	const Vec4f* pts = t.v;
//
//	float minx = std::min({ t.v[0].x,t.v[1].x,t.v[2].x });
//	float maxx = std::max({ t.v[0].x,t.v[1].x,t.v[2].x });
//	float miny = std::min({ t.v[0].y,t.v[1].y,t.v[2].y });
//	float maxy = std::max({ t.v[0].y,t.v[1].y,t.v[2].y });
//
//	int min_x = (int)std::floor(minx);
//	int max_x = (int)std::ceil(maxx);
//	int min_y = (int)std::floor(miny);
//	int max_y = (int)std::ceil(maxy);
//
//	for (int i = min_x; i <= max_x; i++) {
//		for (int j = min_y; j <= max_y; j++) {
//			Vec2i point(i, j);
//			int count = 0;
//			for (auto& step : getSuperSampleStep(sample_count))
//			{
//				auto [alpha, beta, gamma] = computeBarycentric2D(pts, static_cast<float>(i + step.x), static_cast<float>(j + step.y));
//				if (alpha < 0 || beta < 0 || gamma < 0) continue;
//				count++;
//			}
//            if (count > 0) {
//                // 对于每个在三角形内部的像素点，计算出其深度值、纹理坐标、颜色和法向量等信息，并调用fragmentShader函数对这些信息进行处理，得到最终的像素颜色
//                auto [alpha, beta, gamma] = computeBarycentric2D(pts, static_cast<float>(i + 0.5), static_cast<float>(j + 0.5));
//                float z_interpolation = alpha * pts[0].z + beta * pts[1].z + gamma * pts[2].z;
//                Vec2f uv_interpolation = t.texCoords[0] * alpha + t.texCoords[1] * beta + t.texCoords[2] * gamma;
//                Vec3f color_interpolation = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;
//                Vec3f normal_interpolation = t.normal[0] * alpha + t.normal[1] * beta + t.normal[2] * gamma;
//                fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr);
//                // 比较当前像素点的深度值与深度缓冲区中该像素点处的深度值，如果当前像素点的深度值更大，则将其深度值更新，并将最终的像素颜色赋值给该像素点
//                if (z_interpolation > depth_buffer[static_cast<int>(i + j * width)]) {
//                    depth_buffer[static_cast<int>(i + j * width)] = z_interpolation;
//                    auto pixel_color = fragmentShader(payload);
//                    //set_pixel(point, pixel_color);
//                    set_pixel(point, Vec3f(pixel_color.x * count / (sample_count * sample_count),
//                        pixel_color.y * count / (sample_count * sample_count), pixel_color.z * count / (sample_count * sample_count)));// 设置像素点颜色
//                }
//            }
//		}
//	}
//}


void rst::rasterizer::rasterizer_triangle(Triangle& t) {
    const Vec4f* pts = t.v;

    float minx = std::min({ t.v[0].x,t.v[1].x,t.v[2].x });
    float maxx = std::max({ t.v[0].x,t.v[1].x,t.v[2].x });
    float miny = std::min({ t.v[0].y,t.v[1].y,t.v[2].y });
    float maxy = std::max({ t.v[0].y,t.v[1].y,t.v[2].y });

    int min_x = (int)std::floor(minx);
    int max_x = (int)std::ceil(maxx);
    int min_y = (int)std::floor(miny);
    int max_y = (int)std::ceil(maxy);

    for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
            Vec2i point(i, j);

            /*
             *像素通常被看作是一个点，其坐标为左上角的整数坐标。
             *例如，(0,0)表示屏幕左上角的像素，(1,0)表示屏幕上第二个像素，(0,1)表示屏幕左边第二个像素。
             *如果我们直接使用整数坐标来计算像素的重心坐标，那么很有可能会出现误差，导致像素填充不完整或者出现锯齿形状。
             *因此，在计算重心坐标时，我们通常会将像素坐标加上0.5，这样可以将像素坐标放在像素中心位置，从而减小误差和锯齿的出现。
            */
            auto [alpha, beta, gamma] = computeBarycentric2D(pts, static_cast<float>(i + 0.5), static_cast<float>(j + 0.5));
            if (alpha < 0 || beta < 0 || gamma < 0) continue;

            // 对于每个在三角形内部的像素点，计算出其深度值、纹理坐标、颜色和法向量等信息，并调用fragmentShader函数对这些信息进行处理，得到最终的像素颜色
            float z_interpolation = alpha * pts[0].z + beta * pts[1].z + gamma * pts[2].z;
            Vec2f uv_interpolation = t.texCoords[0] * alpha + t.texCoords[1] * beta + t.texCoords[2] * gamma;
            Vec3f color_interpolation = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;
            Vec3f normal_interpolation = t.normal[0] * alpha + t.normal[1] * beta + t.normal[2] * gamma;
            //fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr);
            fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr, t.flatNormal);

            // 比较当前像素点的深度值与深度缓冲区中该像素点处的深度值，如果当前像素点的深度值更大，则将其深度值更新，并将最终的像素颜色赋值给该像素点
            if (z_interpolation > depth_buffer[static_cast<int>(i + j * width)]) {
                depth_buffer[static_cast<int>(i + j * width)] = z_interpolation;
                auto pixel_color = fragmentShader(payload);
                set_pixel(point, pixel_color); // 设置像素点颜色
            }
        }
    }
}

void rst::rasterizer::rasterizer_triangle_msaa(Triangle& t, int sample_count = 2) {
	const Vec4f* pts = t.v;

	float minx = std::min({ t.v[0].x,t.v[1].x,t.v[2].x });
	float maxx = std::max({ t.v[0].x,t.v[1].x,t.v[2].x });
	float miny = std::min({ t.v[0].y,t.v[1].y,t.v[2].y });
	float maxy = std::max({ t.v[0].y,t.v[1].y,t.v[2].y });

	int min_x = (int)std::floor(minx);
	int max_x = (int)std::ceil(maxx);
	int min_y = (int)std::floor(miny);
	int max_y = (int)std::ceil(maxy);

	for (int i = min_x; i <= max_x; i++) {
		for (int j = min_y; j <= max_y; j++) {
			Vec2i point(i, j);
            //判断是否通过了深度测试
            int judge = 0;
			for (int k = 0; k < sample_count * sample_count; k++)
			{
				auto [alpha, beta, gamma] = computeBarycentric2D(pts, static_cast<float>(i + getSuperSampleStep(sample_count)[k].x), static_cast<float>(j + getSuperSampleStep(sample_count)[k].y));
				if (alpha < 0 || beta < 0 || gamma < 0) continue;
				// 对于每个在三角形内部的像素点，计算出其深度值、纹理坐标、颜色和法向量等信息，并调用fragmentShader函数对这些信息进行处理，得到最终的像素颜色
				float z_interpolation = alpha * pts[0].z + beta * pts[1].z + gamma * pts[2].z;
				Vec2f uv_interpolation = t.texCoords[0] * alpha + t.texCoords[1] * beta + t.texCoords[2] * gamma;
				Vec3f color_interpolation = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;
				Vec3f normal_interpolation = t.normal[0] * alpha + t.normal[1] * beta + t.normal[2] * gamma;
                //fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr);
                fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr, t.flatNormal);
				// 比较当前像素点的深度值与深度缓冲区中该像素点处的深度值，如果当前像素点的深度值更大，则将其深度值更新，并将最终的像素颜色赋值给该像素点
				if (z_interpolation > super_depth_buffer[get_super_index(i, j, sample_count)+k]) {
                    judge = 1;
                    auto pixel_color = fragmentShader(payload);
                    super_depth_buffer[get_super_index(i, j, sample_count) + k] = z_interpolation;
                    super_frame_buffer[get_super_index(i, j, sample_count) + k] = pixel_color;
				}
			}
            if (judge)
                //若像素的四个样本中有一个通过了深度测试，就需要对该像素进行着色，因为有一个通过就说明有颜色，就需要着色。
            {
                Vec3f color = Vec3f(0.0f, 0.0f, 0.0f);
                for (int l = 0; l < sample_count; ++l) {
                    for (int m = 0; m < sample_count; ++m) {
                        color = color + super_frame_buffer[get_super_index(i, j, sample_count) + l * sample_count + m];
                    }
                }
                color = color * (1 / float(sample_count * sample_count));
                set_pixel(point, color);
            }
		}
	}
}


void rst::rasterizer::rasterizer_triangle_new(Triangle& t, std::vector<Vec3f> view_pos) {
    const Vec4f* pts = t.v;

    float minx = std::min({ t.v[0].x,t.v[1].x,t.v[2].x });
    float maxx = std::max({ t.v[0].x,t.v[1].x,t.v[2].x });
    float miny = std::min({ t.v[0].y,t.v[1].y,t.v[2].y });
    float maxy = std::max({ t.v[0].y,t.v[1].y,t.v[2].y });

    int min_x = (int)std::floor(minx);
    int max_x = (int)std::ceil(maxx);
    int min_y = (int)std::floor(miny);
    int max_y = (int)std::ceil(maxy);

    for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
            Vec2i point(i, j);

            /*
             *像素通常被看作是一个点，其坐标为左上角的整数坐标。
             *例如，(0,0)表示屏幕左上角的像素，(1,0)表示屏幕上第二个像素，(0,1)表示屏幕左边第二个像素。
             *如果我们直接使用整数坐标来计算像素的重心坐标，那么很有可能会出现误差，导致像素填充不完整或者出现锯齿形状。
             *因此，在计算重心坐标时，我们通常会将像素坐标加上0.5，这样可以将像素坐标放在像素中心位置，从而减小误差和锯齿的出现。
            */
            auto [alpha, beta, gamma] = computeBarycentric2D(pts, static_cast<float>(i + 0.5), static_cast<float>(j + 0.5));
            if (alpha < 0 || beta < 0 || gamma < 0) continue;

            // 对于每个在三角形内部的像素点，计算出其深度值、纹理坐标、颜色和法向量等信息，并调用fragmentShader函数对这些信息进行处理，得到最终的像素颜色
            float z_interpolation = alpha * pts[0].z + beta * pts[1].z + gamma * pts[2].z;
            Vec2f uv_interpolation = t.texCoords[0] * alpha + t.texCoords[1] * beta + t.texCoords[2] * gamma;
            Vec3f color_interpolation = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;
            Vec3f normal_interpolation = t.normal[0] * alpha + t.normal[1] * beta + t.normal[2] * gamma;
            Vec3f shadingcoords_interpolated = view_pos[0] * alpha + view_pos[1] * beta + view_pos[2] * gamma;

            //fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr);
            fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr, t.flatNormal);

            payload.view_pos = shadingcoords_interpolated;

            // 比较当前像素点的深度值与深度缓冲区中该像素点处的深度值，如果当前像素点的深度值更大，则将其深度值更新，并将最终的像素颜色赋值给该像素点
            if (z_interpolation > depth_buffer[static_cast<int>(i + j * width)]) {
                depth_buffer[static_cast<int>(i + j * width)] = z_interpolation;
                auto pixel_color = fragmentShader(payload);
                set_pixel(point, pixel_color); // 设置像素点颜色
            }
        }
    }
}

void rst::rasterizer::rasterizer_triangle_msaa_new(Triangle& t, std::vector<Vec3f> view_pos, int sample_count) {
    const Vec4f* pts = t.v;

    float minx = std::min({ t.v[0].x,t.v[1].x,t.v[2].x });
    float maxx = std::max({ t.v[0].x,t.v[1].x,t.v[2].x });
    float miny = std::min({ t.v[0].y,t.v[1].y,t.v[2].y });
    float maxy = std::max({ t.v[0].y,t.v[1].y,t.v[2].y });

    int min_x = (int)std::floor(minx);
    int max_x = (int)std::ceil(maxx);
    int min_y = (int)std::floor(miny);
    int max_y = (int)std::ceil(maxy);

    for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
            Vec2i point(i, j);
            //判断是否通过了深度测试
            int judge = 0;
            for (int k = 0; k < sample_count * sample_count; k++)
            {
                auto [alpha, beta, gamma] = computeBarycentric2D(pts, static_cast<float>(i + getSuperSampleStep(sample_count)[k].x), static_cast<float>(j + getSuperSampleStep(sample_count)[k].y));
                if (alpha < 0 || beta < 0 || gamma < 0) continue;
                // 对于每个在三角形内部的像素点，计算出其深度值、纹理坐标、颜色和法向量等信息，并调用fragmentShader函数对这些信息进行处理，得到最终的像素颜色
                float z_interpolation = alpha * pts[0].z + beta * pts[1].z + gamma * pts[2].z;
                Vec2f uv_interpolation = t.texCoords[0] * alpha + t.texCoords[1] * beta + t.texCoords[2] * gamma;
                Vec3f color_interpolation = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;
                Vec3f normal_interpolation = t.normal[0] * alpha + t.normal[1] * beta + t.normal[2] * gamma;
                Vec3f shadingcoords_interpolated = view_pos[0] * alpha + view_pos[1] * beta + view_pos[2] * gamma;

                //fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr);
                fragment_shader_payload payload(color_interpolation, normal_interpolation, uv_interpolation, texture ? &*texture : nullptr, t.flatNormal);

                payload.view_pos = shadingcoords_interpolated;

                // 比较当前像素点的深度值与深度缓冲区中该像素点处的深度值，如果当前像素点的深度值更大，则将其深度值更新，并将最终的像素颜色赋值给该像素点
                if (z_interpolation > super_depth_buffer[get_super_index(i, j, sample_count) + k]) {
                    judge = 1;
                    auto pixel_color = fragmentShader(payload);
                    super_depth_buffer[get_super_index(i, j, sample_count) + k] = z_interpolation;
                    super_frame_buffer[get_super_index(i, j, sample_count) + k] = pixel_color;
                }
            }
            if (judge)
                //若像素的四个样本中有一个通过了深度测试，就需要对该像素进行着色，因为有一个通过就说明有颜色，就需要着色。
            {
                Vec3f color = Vec3f(0.0f, 0.0f, 0.0f);
                for (int l = 0; l < sample_count; ++l) {
                    for (int m = 0; m < sample_count; ++m) {
                        color = color + super_frame_buffer[get_super_index(i, j, sample_count) + l * sample_count + m];
                    }
                }
                color = color * (1 / float(sample_count * sample_count));
                set_pixel(point, color);
            }
        }
    }
}
