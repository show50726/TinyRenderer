#include "Utils.h"

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

Vec3f barycentric(Vec3f* v, Vec3f p) {
	Vec3f f0 = v[0] - p;
	Vec3f f1 = v[1] - v[0];
	Vec3f f2 = v[2] - v[0];
	Vec3f cv1 = Vec3f(f0.x, f1.x, f2.x);
	Vec3f cv2 = Vec3f(f0.y, f1.y, f2.y);

	Vec3f bv = cross(cv1, cv2);

	if (std::abs(bv.x) > 1e-2)
		return Vec3f(1.0f - (bv.y + bv.z) / bv.x, bv.y / bv.x, bv.z / bv.x); 

	return Vec3f(-1, 1, 1);
}

bool is_valid_barycentric(const Vec3f& bc) {
	if (bc.x < 0 || bc.y < 0 || bc.z < 0)
		return false;

	return true;
}

const Vec3f screen_to_world(const Vec3f& p) {
	return Vec3f((int)((p.x + 1.0) * 800 / 2.0f), (int)((p.y + 1.0) * 800 / 2.0f), p.z);
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
			Vec3f bc = barycentric(v, p);

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

void DrawUtils::model(Model* model, Vec3f light_dir, TGAImage &image) {
	float *zbuffer = new float[800*800];
	for (int i = 0; i < 800 * 800; i++) {
		zbuffer[i] = -100000010;
	}

	for (int i = 0; i < model->nfaces(); i++) {
		auto face = model->face(i);
		Vec3f screen_coord[3];
		Vec3f world_coord[3];

		for (int j = 0; j < 3; j++) {
			Vec3f v = model->vert(face[j]);
			screen_coord[j] = screen_to_world(v);
			world_coord[j] = v;
		}

		Vec3f v1(world_coord[1] - world_coord[0]);
		Vec3f v2(world_coord[2] - world_coord[0]);
		Vec3f n = cross(v2, v1);
		n.normalize();

		float intensity = n * light_dir;

		if (intensity > 0) {
			// texture sampling 
			auto texture = model->texture(i);
			int w = model->texture_img->get_width();
			int h = model->texture_img->get_height();
			std::vector<Vec2f> text_coord;
			for (auto id : texture) {
				Vec2f coord(model->texture_coord(id)[0] * w, model->texture_coord(id)[1] * h);
				text_coord.push_back(coord);
			}

			triangle(screen_coord, image, zbuffer, text_coord, *(model->texture_img), intensity);
		}
	}

	delete[] zbuffer;
}

void DrawUtils::triangle(Vec3f* v, TGAImage &image, float* zbuffer, std::vector<Vec2f>& texture_coord, TGAImage &texture, float intensity) {
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
			Vec3f bc = barycentric(v, p);

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

				Vec2f pixel_texture_coord(bc.x*texture_coord[0] + bc.y*texture_coord[1] + bc.z*texture_coord[2]);
				TGAColor color = texture.get((int)pixel_texture_coord.x, (int)pixel_texture_coord.y) * intensity;

				image.set(p.x, p.y, color);
			}

		}
	}
}