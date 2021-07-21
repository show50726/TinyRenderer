#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int>> faces_;
	std::vector<std::vector<float>> texture_coord_;
	std::vector<std::vector<int>> textures_;

public:
	Model(const char *filename, const char* texturename = NULL);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	std::vector<int> texture(int idx);
	std::vector<float> texture_coord(int idx);
	TGAImage* texture_img;
};

#endif //__MODEL_H__
