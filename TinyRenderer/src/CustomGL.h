#pragma once

#include <vector>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include "Utils.h"

namespace Renderer {
	struct IShader {
		virtual ~IShader();
		virtual Vec4f vertex(int iface, int nthvert) = 0;
		virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
	};

	class ViewInfo {
	public:
		static void look_at(Vec3f eye, Vec3f center, Vec3f up);
		static void view_port(int x, int y, int w, int h);
		static const Vec4f world_to_screen(const Vec4f p);
		static const Matrix get_MVP();

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
		static void triangle(Vec4f* v, TGAImage &image, TGAImage& zbuffer, IShader* shader);
		static void model(Model* model, TGAImage &image, IShader* shader, TGAImage& zbuffer);

	private:
		static void horizontal_line(int x0, int x1, int y, TGAImage &image, TGAColor color);
	};
}
