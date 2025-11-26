#ifndef LSMATH
#define LSMATH

#include <iostream>
#include <vector>
#include <algorithm>

namespace LsMath {
    template <class T>
    class Vector2 {
    public:
        T x, y;

        // 构造函数
        Vector2() : x(0), y(0) {}
        Vector2(T _x, T _y) : x(_x), y(_y) {}

        // 使用默认的拷贝构造函数和赋值运算符
        Vector2(const Vector2& other) = default;
        Vector2& operator=(const Vector2& other) = default;

        // 算术运算符重载
        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }

        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }

        // 标量乘法（注意：使用模板类型 T）
        Vector2 operator*(T scalar) const {
            return Vector2(x * scalar, y * scalar);
        }

        Vector2 operator/(T scalar) const {
            return Vector2(x / scalar, y / scalar);
        }

        // 复合赋值运算符
        Vector2& operator+=(const Vector2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2& operator-=(const Vector2& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vector2& operator/=(T scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        // 点积
        T dot(const Vector2& other) const {
            return x * other.x + y * other.y;
        }

        // 长度（返回 T 类型）
        T length() const {
            return static_cast<T>(sqrt(static_cast<double>(x * x + y * y)));
        }

        // 平方长度（避免开方运算，性能更好）
        T lengthSquared() const {
            return x * x + y * y;
        }

        // 归一化（返回新的向量）
        Vector2 normalized() const {
            T len = length();
            if (len > 0) {
                return Vector2(x / len, y / len);
            }
            return *this;
        }

        // 归一化（修改当前向量）
        void normalize() {
            T len = length();
            if (len > 0) {
                x /= len;
                y /= len;
            }
        }

        // 比较运算符
        bool operator==(const Vector2& other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2& other) const {
            return !(*this == other);
        }

        // 友元函数：标量乘法（标量在前）
        friend Vector2 operator*(T scalar, const Vector2& vec) {
            return Vector2(scalar * vec.x, scalar * vec.y);
        }

        // 输出流运算符
        friend std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
            os << "(" << vec.x << ", " << vec.y << ")";
            return os;
        }
    };

    // 常用的类型别名
    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;
    using Vector2i = Vector2<int>;

    template <class T>
    class LsRect {
    public:
        T x, y, width, height;

        // 构造函数 - 移除函数体的大括号
        LsRect() : x(0), y(0), width(0), height(0) {}
        LsRect(T _x, T _y, T _width, T _height) : x(_x), y(_y), width(_width), height(_height) {}

        // 通过位置和大小构造 - 移除函数体的大括号
        void setRectSize(const Vector2<T> position, const Vector2<T> size) {
            x = position.x;
            y = position.y;
            width = size.x;
            height = size.y;
        }

        // 通过两个点构造 - 移除函数体的大括号
        void setRectMM(const Vector2<T> min, const Vector2<T> max) {
            x = min.x;
            y = min.y;
            width = max.x - min.x;
            height = max.y - min.y;
        }


        // 使用默认的拷贝构造函数和赋值运算符
        LsRect(const LsRect& other) = default;
        LsRect& operator=(const LsRect& other) = default;

        // 获取各个边的位置
        T left() const { return x; }
        T right() const { return x + width; }
        T top() const { return y; }
        T bottom() const { return y + height; }

        // 获取角点
        Vector2<T> position() const { return Vector2<T>(x, y); }
        Vector2<T> size() const { return Vector2<T>(width, height); }
        Vector2<T> center() const { return Vector2<T>(x + width / 2, y + height / 2); }
        Vector2<T> _min() const { return Vector2<T>(x, y); }
        Vector2<T> _max() const { return Vector2<T>(x + width, y + height); }
        Vector2<T> topLeft() const { return Vector2<T>(x, y); }
        Vector2<T> topRight() const { return Vector2<T>(x + width, y); }
        Vector2<T> bottomLeft() const { return Vector2<T>(x, y + height); }
        Vector2<T> bottomRight() const { return Vector2<T>(x + width, y + height); }

        // 设置各个边的位置
        void setLeft(T left) { width += x - left; x = left; }
        void setRight(T right) { width = right - x; }
        void setTop(T top) { height += y - top; y = top; }
        void setBottom(T bottom) { height = bottom - y; }

        // 判断点是否在矩形内
        bool contains(const Vector2<T>& point) const {
            return point.x >= x && point.x <= x + width &&
                point.y >= y && point.y <= y + height;
        }

        bool contains(T px, T py) const {
            return contains(Vector2<T>(px, py));
        }

        // 判断矩形是否相交
        bool intersects(const LsRect& other) const {
            return !(other.x > x + width ||
                other.x + other.width < x ||
                other.y > y + height ||
                other.y + other.height < y);
        }

        // 获取相交区域
        LsRect intersection(const LsRect& other) const {
            T left = max(x, other.x);
            T right = min(x + width, other.x + other.width);
            T top = max(y, other.y);
            T bottom = min(y + height, other.y + other.height);

            if (right > left && bottom > top) {
                return LsRect(left, top, right - left, bottom - top);
            }
            return LsRect(); // 返回空矩形
        }

        // 获取并集区域（包含两个矩形的最小矩形）
        LsRect unionWith(const LsRect& other) const {
            T left = min(x, other.x);
            T right = max(x + width, other.x + other.width);
            T top = min(y, other.y);
            T bottom = max(y + height, other.y + other.height);

            return LsRect(left, top, right - left, bottom - top);
        }

        // 扩展矩形（正数扩展，负数收缩）
        LsRect expanded(T amount) const {
            return LsRect(x - amount, y - amount, width + 2 * amount, height + 2 * amount);
        }

        LsRect expanded(T horizontal, T vertical) const {
            return LsRect(x - horizontal, y - vertical,
                width + 2 * horizontal, height + 2 * vertical);
        }

        // 移动矩形
        LsRect translated(const Vector2<T>& offset) const {
            return LsRect(x + offset.x, y + offset.y, width, height);
        }

        LsRect translated(T dx, T dy) const {
            return LsRect(x + dx, y + dy, width, height);
        }

        // 缩放矩形（以中心点为基准）
        LsRect scaled(T scale) const {
            Vector2<T> center = this->center();
            T newWidth = width * scale;
            T newHeight = height * scale;
            return LsRect(center.x - newWidth / 2, center.y - newHeight / 2, newWidth, newHeight);
        }

        LsRect scaled(T scaleX, T scaleY) const {
            Vector2<T> center = this->center();
            T newWidth = width * scaleX;
            T newHeight = height * scaleY;
            return LsRect(center.x - newWidth / 2, center.y - newHeight / 2, newWidth, newHeight);
        }

        Vector2<T> constraints(const Vector2<T> _point) {
            Vector2<T> Opoint = _point;
            if (_point.x > x) { Opoint.x = x; }
            if (_point.y > y) { Opoint.y = y; }
            if (_point.x < right()) { Opoint.x = right(); }
            if (_point.y < bottom()) { Opoint.x = bottom(); }
            return _point;
        }

        // 面积
        T area() const {
            return width * height;
        }

        // 周长
        T perimeter() const {
            return 2 * (width + height);
        }

        // 判断是否为空矩形
        bool isEmpty() const {
            return width <= 0 || height <= 0;
        }

        // 比较运算符
        bool operator==(const LsRect& other) const {
            return x == other.x && y == other.y &&
                width == other.width && height == other.height;
        }

        bool operator!=(const LsRect& other) const {
            return !(*this == other);
        }

        // 算术运算符：矩形相加（并集）
        LsRect operator+(const LsRect& other) const {
            return this->unionWith(other);
        }

        // 算术运算符：矩形相减（无标准定义，这里返回移动后的矩形）
        LsRect operator-(const Vector2<T>& offset) const {
            return this->translated(Vector2<T>(-offset.x, -offset.y));
        }

        // 标量乘法（缩放）
        LsRect operator*(T scalar) const {
            return this->scaled(scalar);
        }

        LsRect operator/(T scalar) const {
            if (scalar != 0) {
                return this->scaled(static_cast<T>(1) / scalar);
            }
            return *this; // 避免除以零
        }

        // 复合赋值运算符
        LsRect& operator+=(const Vector2<T>& offset) {
            x += offset.x;
            y += offset.y;
            return *this;
        }

        LsRect& operator-=(const Vector2<T>& offset) {
            x -= offset.x;
            y -= offset.y;
            return *this;
        }

        LsRect& operator*=(T scalar) {
            *this = this->scaled(scalar);
            return *this;
        }

        LsRect& operator/=(T scalar) {
            if (scalar != 0) {
                *this = this->scaled(static_cast<T>(1) / scalar);
            }
            return *this;
        }

        // 输出流运算符
        friend std::ostream& operator<<(std::ostream& os, const LsRect& rect) {
            os << "Rect(" << rect.x << ", " << rect.y << ", "
                << rect.width << ", " << rect.height << ")";
            return os;
        }
    };

    // 常用的类型别名
    using LsRectf = LsRect<float>;
    using LsRectd = LsRect<double>;
    using LsRecti = LsRect<int>;

    bool isPointInPolygon(Vector2d P, const std::vector<Vector2d>& polygon);
}

#endif