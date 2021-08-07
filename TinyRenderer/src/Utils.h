#pragma once
#include <vector>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"


const TGAColor WHITE = TGAColor(255, 255, 255, 0);
const TGAColor RED = TGAColor(0, 0, 255, 0);
const TGAColor GREEN = TGAColor(255, 0, 0, 0);

const Vec3f light_dir(1, 1, 0);

const float depth = 255.0;
const int height = 800;
const int width = 800;

/////////////////////////////////////////////////////////////////////////

Vec3f barycentric(std::vector<Vec3f>& v, Vec3f p);
bool is_valid_barycentric(const Vec3f& bc);
template <typename T> const T max(const T& a, const T& b);
template <typename T> const T min(const T& a, const T& b);

/////////////////////////////////////////////////////////////////////////


template <typename T> const T max(const T& a, const T& b) {
	return a > b ? a : b;
}

template <typename T> const T min(const T& a, const T& b) {
	return a < b ? a : b;
}
