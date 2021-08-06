#include "Utils.h"

Vec3f barycentric(std::vector<Vec3f>& v, Vec3f p) {
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
