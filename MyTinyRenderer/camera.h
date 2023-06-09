#pragma once
#include "geometry.h"

class Camera {
private:

public:
	Vec3f Position;
	Vec3f WorldUp;
	Vec3f Front;
	Vec3f Up;
	Vec3f Right;

	//默认初始化相机位置为坐标原点，且默认以向上为基准，默认朝-z方向看
	Camera(Vec3f position = Vec3f(0.0f, 0.0f, 0.0f), Vec3f worldup = Vec3f(0.0f, 1.0f, 0.0f), Vec3f front = Vec3f(0.0f, 0.0f, -1.0f)) {
		Position = position;
		WorldUp = worldup;
		Front = front.normalize();
		Right = (Front ^ WorldUp).normalize();
		Up = (Right ^ Front).normalize();
	}
};