#pragma once
#include <vector>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 0);
const TGAColor RED = TGAColor(0, 0, 255, 0);
const TGAColor GREEN = TGAColor(255, 0, 0, 0);

const int depth = 255;
const int height = 800;
const int width = 800;

/////////////////////////////////////////////////////////////////////////

Vec3f barycentric(Vec3f* v, Vec3f p);
bool is_valid_barycentric(const Vec3f& bc);
template <typename T> const T max(const T& a, const T& b);
template <typename T> const T min(const T& a, const T& b);

/////////////////////////////////////////////////////////////////////////

class ViewInfo {
public:
	static void look_at(Vec3f eye, Vec3f center, Vec3f up);
	static void view_port(int x, int y, int w, int h);
	static const Vec3f world_to_screen(const Vec3f pos);

private:
	static Matrix ViewMatrix;
	static Matrix ProjectionMatrix;
	static Matrix ViewPortMatrix;
};

class DrawUtils {
public:
	static void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
	static void triangle_by_horizontal_lines(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
	static void triangle_by_bounding_box_check(Vec3f* v, TGAImage &image, float* zbuffer, TGAColor color);
	static void triangle(Vec3f* v, TGAImage &image, float* zbuffer, std::vector<Vec2f>& texture_coord, TGAImage &texture, float intensity = 1);
	static void model(Model* model, Vec3f light_dir, TGAImage &image);

private:
	static void horizontal_line(int x0, int x1, int y, TGAImage &image, TGAColor color);
};

template <typename T> const T max(const T& a, const T& b) {
	return a > b ? a : b;
}

template <typename T> const T min(const T& a, const T& b) {
	return a < b ? a : b;
}
