#pragma once
#include "tgaimage.h"
#include "geometry.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 0);
const TGAColor RED = TGAColor(0, 0, 255, 0);
const TGAColor GREEN = TGAColor(255, 0, 0, 0);

Vec3f barycentric(Vec2i *pts, Vec2i p);
bool is_valid_barycentric(const Vec3f& bc);
template <typename T> const T max(const T& a, const T& b);
template <typename T> const T min(const T& a, const T& b);

class DrawUtils {
public:
	static void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
	static void triangle_by_horizontal_lines(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
	static void triangle_by_bounding_box_check(Vec2i* v, TGAImage &image, TGAColor color);

private:
	static void horizontal_line(int x0, int x1, int y, TGAImage &image, TGAColor color);
};

template <typename T> const T max(const T& a, const T& b) {
	return a > b ? a : b;
}

template <typename T> const T min(const T& a, const T& b) {
	return a < b ? a : b;
}
