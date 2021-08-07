#include "CustomGL.h"

using namespace Renderer;

Matrix ViewInfo::ProjectionMatrix;
Matrix ViewInfo::ViewMatrix;
Matrix ViewInfo::ViewPortMatrix;

const Matrix ViewInfo::get_MVP() {
	//std::cout << ViewPortMatrix << ProjectionMatrix << ViewMatrix << std::endl;
	return ViewPortMatrix * ProjectionMatrix * ViewMatrix;
}

void ViewInfo::look_at(Vec3f eye, Vec3f center, Vec3f up) {
	Vec3f z = (eye - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();

	ViewMatrix = Matrix::identity();
	for (int i = 0; i < 3; i++) {
		ViewMatrix[0][i] = x[i];
		ViewMatrix[1][i] = y[i];
		ViewMatrix[2][i] = z[i];
		ViewMatrix[3][i] = -center[i];
	}

	ProjectionMatrix = Matrix::identity();
	ProjectionMatrix[3][2] = -1.0 / (eye - center).norm();
}

// Mapping [-1, 1] * [-1, 1] * [-1, 1] to [x, x + w] * [y, y + h] * [0, depth]
void ViewInfo::view_port(int x, int y, int w, int h) {
	ViewPortMatrix = Matrix::identity();

	ViewPortMatrix[0][3] = x + w / 2.0f;
	ViewPortMatrix[1][3] = y + h / 2.0f;
	ViewPortMatrix[2][3] = depth / 2.0f;

	ViewPortMatrix[0][0] = w / 2.0f;
	ViewPortMatrix[1][1] = h / 2.0f;
	ViewPortMatrix[2][2] = depth / 2.0f;
}

const Vec4f ViewInfo::world_to_screen(const Vec4f p) {
	auto pos = ViewPortMatrix * ProjectionMatrix * ViewMatrix * p;
	return pos;
}

void DrawUtils::line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	bool steep = false;
	if (std::abs(x0 - x1)<std::abs(y0 - y1)) { // if the line is steep, we transpose the image 
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0>x1) { // make it left−to−right 
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy * 2);
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			image.set(y, x, color); // if transposed, de−transpose 
		}
		else {
			image.set(x, y, color);
		}

		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void DrawUtils::horizontal_line(int x0, int x1, int y, TGAImage &image, TGAColor color) {
	if (x0 > x1) {
		std::swap(x0, x1);
	}

	for (int i = x0; i <= x1; i++) {
		image.set(i, y, color);
	}
}

// Draw triangle by drawing horizontal lines
void DrawUtils::triangle_by_horizontal_lines(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
	// sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
	auto sort_by_y = [](Vec2i& t0, Vec2i& t1, Vec2i& t2) {
		if (t0.y>t1.y) std::swap(t0, t1);
		if (t0.y>t2.y) std::swap(t0, t2);
		if (t1.y>t2.y) std::swap(t1, t2);
	};

	sort_by_y(t0, t1, t2);

	int total_height = t2.y - t0.y;

	for (int y = t0.y; y <= t2.y; y++) {
		bool upper = y >= t1.y;
		int segment_height = (upper ? (t2.y - t1.y) : (t1.y - t0.y)) + 1;

		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - (upper ? t1.y : t0.y)) / segment_height;

		Vec2i A = t0 + (t2 - t0)*alpha;
		Vec2i B = (upper ? t1 : t0) + (upper ? (t2 - t1) : (t1 - t0))*beta;

		horizontal_line(A.x, B.x, y, image, color);
	}
}


// Draw triangle by checking if each point in a bounding box is in the triangle or not
void DrawUtils::triangle_by_bounding_box_check(Vec3f* v, TGAImage &image, float* zbuffer, TGAColor color) {
	Vec2f minbbox(image.get_width() - 1, image.get_height() - 1);
	Vec2f maxbbox(0, 0);
	Vec2f clamp(minbbox);

	for (int i = 0; i < 3; i++) {
		minbbox.x = max(0.0f, min(minbbox.x, v[i].x));
		minbbox.y = max(0.0f, min(minbbox.y, v[i].y));

		maxbbox.x = min(clamp.x, max(maxbbox.x, v[i].x));
		maxbbox.y = min(clamp.y, max(maxbbox.y, v[i].y));
	}

	Vec3f p;
	for (p.x = minbbox.x; p.x <= maxbbox.x; p.x++) {
		for (p.y = minbbox.y; p.y <= maxbbox.y; p.y++) {
			std::vector<Vec3f> tmp = { proj<3>(v[0] / v[0][3]) , proj<3>(v[1] / v[1][3]) , proj<3>(v[2] / v[2][3]) };
			Vec3f bc = barycentric(tmp, proj<3>(p));

			if (!is_valid_barycentric(bc)) {
				continue;
			}

			p.z = 0;
			p.z += v[0].z*bc.x;
			p.z += v[1].z*bc.y;
			p.z += v[2].z*bc.z;

			int idx = static_cast<int>(p.x + p.y * 800);
			if (zbuffer[idx] < p.z) {
				zbuffer[idx] = p.z;
				image.set(p.x, p.y, color);
			}

		}
	}
}

void DrawUtils::model(Model* model, TGAImage &image, IShader* shader, TGAImage& zbuffer) {
	for (int i = 0; i < model->nfaces(); i++) {
		Vec4f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader->vertex(i, j);
		}

		triangle(screen_coords, image, zbuffer, shader);
	}
}

void DrawUtils::triangle(Vec4f* v, TGAImage &image, TGAImage& zbuffer, IShader* shader) {
	Vec2f minbbox(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f maxbbox(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for (int i = 0; i < 3; i++) {
		minbbox.x = min(minbbox.x, v[i].x / v[i].w);
		minbbox.y = min(minbbox.y, v[i].y / v[i].w);

		maxbbox.x = max(maxbbox.x, v[i].x / v[i].w);
		maxbbox.y = max(maxbbox.y, v[i].y / v[i].w);
	}

	Vec2i p;
	TGAColor color;
	for (p.x = minbbox.x; p.x <= maxbbox.x; p.x++) {
		for (p.y = minbbox.y; p.y <= maxbbox.y; p.y++) {
			std::vector<Vec3f> tmp = { proj<3>(v[0] / v[0][3]) , proj<3>(v[1] / v[1][3]) , proj<3>(v[2] / v[2][3]) };
			Vec3f bc = barycentric(tmp, Vec3f(p.x, p.y, 1.0f));

			if (!is_valid_barycentric(bc)) {
				continue;
			}

			float z = 0;
			z += v[0].z*bc.x;
			z += v[1].z*bc.y;
			z += v[2].z*bc.z;

			float w = 0;
			w += v[0].w*bc.x;
			w += v[1].w*bc.y;
			w += v[2].w*bc.z;


			int frag_depth = max(0, min(255, int(z / w + 0.5f)));
			if (zbuffer.get(p.x, p.y).b > frag_depth) {
				continue;
			}

			bool discard = shader->fragment(bc, color);
			if (!discard) {
				zbuffer.set(p.x, p.y, TGAColor(0, 0, frag_depth));
				image.set(p.x, p.y, color);
			}
		}
	}
}

IShader::~IShader() {}