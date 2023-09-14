#pragma once
#include <ostream>

#pragma region 二维向量模板
template <class t>
class Vec2 {
public:
	t x, y;//x,y坐标
	Vec2<t>() :x(t()), y(t()) {}//构造函数
	Vec2<t>(t _x, t _y) : x(_x), y(_y) {}
	Vec2<t>(const Vec2<t>& v) { *this = v; }//拷贝构造函数
	Vec2<t>& operator=(const Vec2<t>& v)//拷贝赋值函数
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
		}
		return *this;
	}
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(x + V.x, y + V.y); }//向量加法
	inline Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t>(x - V.x, y - V.y); }//向量减法
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(x * f, y * f); }//向量的数乘运算
	inline Vec2<t> operator/(const t& V) const { return Vec2<t>(x / V, y / V); }// 向量与标量的除法
	inline t operator *(const Vec2<t>& V) const { return x * V.x + y * V.y; } // 点乘运算

	inline t& operator[](const int idx) { if (idx <= 0) return x; else return y; }

	inline Vec2<t> cwiseProduct(const Vec2<t>& V) const { return Vec2<t>(x * V.x, y * V.y); } // 逐元素乘法
	float norm() const { return std::sqrt(x * x + y * y); }//向量的模长
	Vec2<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }//向量归一化

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);//向量输出
};

template <class t>
std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

//模板特化,并且重命名
typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
#pragma endregion

#pragma region 三维向量模板
template <typename t>
struct Vec3 {
	t x, y, z;
	Vec3<t>() :x(t()), y(t()), z(t()) {}
	Vec3<t>(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	Vec3<t>(const Vec3<t>& v) { *this = v; }
	Vec3<t>& operator=(const Vec3<t>& v)
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
		}
		return *this;
	}
	inline Vec3<t> operator +(const Vec3<t>& V) const { return Vec3<t>(x + V.x, y + V.y, z + V.z); }//向量加法
	inline Vec3<t> operator -(const Vec3<t>& V) const { return Vec3<t>(x - V.x, y - V.y, z - V.z); }//向量减法
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }//向量的数乘运算
	inline Vec3<t> operator/(const t& V) const { return Vec3<t>(x / V, y / V, z / V); }// 向量与标量的除法
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }//向量叉乘运算（外积）
	inline t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }//向量的点乘（内积)

	inline t& operator[](const int idx) { if (idx <= 0) return x; else if (idx == 1) return y; else return z; }

	inline Vec3<t> cwiseProduct(const Vec3<t>& v) const { return Vec3<t>(x * v.x, y * v.y, z * v.z); }//逐元素乘法运算
	float norm() const { return std::sqrt(x * x + y * y + z * z); }//向量的模长
	Vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }//向量归一化

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);//向量输出
};

template <class t>
std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

//模板特化,并且重命名
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;
#pragma endregion

#pragma region 四维向量模板
template <class t>
struct Vec4
{
	t x, y, z, w;
	Vec4<t>() :x(t()), y(t()), z(t()), w(t()) {}
	Vec4<t>(t _x, t _y, t _z, t _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vec4<t>(const Vec4<t>& v) { *this = v; }
	Vec4<t>& operator=(const Vec4<t>& v)
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
			this->w = v.w;
		}
		return *this;
	}
	inline Vec4<t> operator +(const Vec4<t>& V) const { return Vec4<t>(x + V.x, y + V.y, z + V.z, w + V.w); }//向量加法
	inline Vec4<t> operator -(const Vec4<t>& V) const { return Vec4<t>(x - V.x, y - V.y, z - V.z, w - V.w); }//向量减法
	inline Vec4<t> operator *(float f)          const { return Vec4<t>(x * f, y * f, z * f, w * f); }//向量的数乘运算
	inline Vec4<t> operator/(const t& V) const { return Vec4<t>(x / V, y / V, z / V ,w / V); }// 向量与标量的除法
	inline t       operator *(const Vec4<t>& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }//向量的点乘（内积）

	inline t& operator[](const int idx) { if (idx <= 0) return x; else if (idx == 1) return y; else if (idx == 2) return z; else return w; }

	inline Vec4<t> cwiseProduct(const Vec4<t>& v) const { return Vec4<t>(x * v.x, y * v.y, z * v.z, w * v.w); }//逐元素乘法
	float norm() const { return std::sqrt(x * x + y * y + z * z + w * w); }//向量的模长
	Vec4<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }//向量归一化

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec4<t>& v);//向量输出
};

template <class t>
std::ostream& operator<<(std::ostream& s, Vec4<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
	return s;
}



//模板特化,并且重命名
typedef Vec4<float> Vec4f;
typedef Vec4<int>   Vec4i;
#pragma endregion

#pragma region 三阶方阵

/**
 * Mat3f类表示一个3x3的浮点数矩阵，包含三个Vec3f类型的行向量。
 */
class Mat3f {
public:
	Vec3f rows[3];  // 三个行向量

	/**
	 * @brief默认构造函数，创建一个全零的3x3矩阵。
	 */
	Mat3f();

	/**
	 * @brief获取或修改矩阵的第i行。
	 * @param i 要获取或修改的行号，取值范围为0~2。
	 * @return 返回第i行的引用，可以用于直接修改该行的元素。
	 */
	Vec3f& operator[](const int i);

	/**
	 * @brief获取矩阵的第i行。
	 * @param i 要获取的行号，取值范围为0~2。
	 * @return 返回一个Vec3f类型的对象，表示第i行的元素。
	 */
	Vec3f operator[](const int i) const;

	/**
	 * @brief计算两个矩阵的乘积。
	 * @param a 另一个3x3矩阵。
	 * @return 返回一个新的Mat3f对象，表示两个矩阵的乘积。
	 */
	Mat3f operator*(Mat3f& a);

	/**
	 * @brief计算矩阵和向量的乘积。
	 * @param a 一个三维向量。
	 * @return 返回一个新的Vec3f对象，表示矩阵和向量的乘积。
	 */
	Vec3f operator*(Vec3f& a);

	/**
	 * @brief返回当前矩阵的转置矩阵。
	 * @return 返回一个新的Mat3f对象，表示当前矩阵的转置矩阵。
	 */
	Mat3f transpose();

	/**
	 * @brief返回当前矩阵的逆矩阵。
	 * @return 返回一个新的Mat3f对象，表示当前矩阵的逆矩阵，如果不存在逆矩阵，则返回一个全零矩阵。
	 */
	Mat3f inverse();

	/**
	 * @brief返回一个3x3的单位矩阵。
	 * @return 返回一个新的Mat3f对象，表示一个3x3的单位矩阵。
	 */
	static Mat3f identity();

	/**
	* @brief重载流插入运算符，在 Mat3f 类中添加一个成员函数用于按行添加 Vec3f 向量
	* @param v 要输出的Vec3f对象。
	* @return 返回三维矩阵的引用，可以用于链式输出。
	* 	 
	*/
	Mat3f& operator<<(const Vec3f& v);

	/**
	 * @brief重载流插入运算符，用于将矩阵的值输出到流中。
	 * @param s 输出流对象。
	 * @param m 要输出的Mat3f对象。
	 * @return 返回流对象的引用，可以用于链式输出。
	 */
	friend std::ostream& operator<<(std::ostream& s, Mat3f& m);
	/**
	 * @brief 计算 3x3 矩阵的行列式
	 *
	 * @param mat 要计算行列式的矩阵
	 * @return float 矩阵的行列式
	 */
	friend float determinant(Mat3f& mat);

	/**
	 * @brief 将三个向量按列拼接成一个 3x3 矩阵
	 *
	 * @param v1 第一列的向量
	 * @param v2 第二列的向量
	 * @param v3 第三列的向量
	 * @return Mat3f 拼接成的矩阵
	 */
	friend Mat3f concatenate(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3);
};
#pragma endregion

#pragma region 四阶方阵

/**
 * Mat4f类表示一个4x4的浮点数矩阵，包含四个Vec4f类型的行向量。
 */
class Mat4f
{
public:
	Vec4f rows[4]; // 存储 4x4 矩阵的四行，每行为 Vec4f 类型的向量

	/**
	 * @brief 构造函数，将矩阵的四行初始化为 Vec4f() 对象
	 */
	Mat4f();

	/**
	 * @brief 重载运算符 []，用于访问矩阵的某一行
	 * @param i 索引值，表示矩阵的第 i 行
	 * @return 返回矩阵第 i 行的引用
	 */
	Vec4f& operator[](const int i);

	/**
	 * @brief 重载运算符 []，用于访问矩阵的某一行
	 * @param i 索引值，表示矩阵的第 i 行
	 * @return 返回矩阵第 i 行的值
	 */
	Vec4f operator [](const int i) const;

	/**
	 * @brief 重载运算符 *，用于两个矩阵的乘法
	 * @param a 另一个 Mat4f 类型的矩阵
	 * @return 返回两个矩阵相乘的结果
	 */
	Mat4f operator*(Mat4f& a);

	/**
	 * @brief 重载运算符 *，用于矩阵与向量的乘法
	 * @param a Vec4f 类型的向量
	 * @return 返回矩阵与向量相乘的结果
	 */
	Vec4f operator*(Vec4f& a);

	/**
	 * @brief 计算矩阵的转置矩阵
	 * @return 返回矩阵的转置矩阵
	 */
	Mat4f transpose();

	/**
	 * @brief 计算矩阵的逆矩阵
	 * @return 返回矩阵的逆矩阵，如果矩阵不可逆，如果不存在逆矩阵，则返回一个全零矩阵
	 */
	Mat4f inverse();

	/**
	 * @brief 静态方法，返回一个 4x4 的单位矩阵
	 * @return 返回一个 4x4 的单位矩阵
	 */
	static Mat4f identity();

	/**
	 * @brief 重载运算符 <<，用于将矩阵打印到输出流中
	 * @param s 输出流对象
	 * @param m 要打印的矩阵对象
	 * @return 返回输出流对象
	 */
	friend std::ostream& operator<<(std::ostream& s, Mat4f& m);

	/**
	 * @brief 计算 4x4 矩阵的行列式
	 *
	 * @param mat 要计算行列式的矩阵
	 * @return float 矩阵的行列式
	 */
	friend float determinant(Mat4f& mat);

	/**
	 * @brief 将四个向量按列拼接成一个 4x4 矩阵
	 *
	 * @param v1 第一列的向量
	 * @param v2 第二列的向量
	 * @param v3 第三列的向量
	 * @param v4 第四列的向量
	 * @return Mat4f 拼接成的矩阵
	 */
	friend Mat4f concatenate(const Vec4f& v1, const Vec4f& v2, const Vec4f& v3, const Vec4f& v4);
};
#pragma endregion
