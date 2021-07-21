#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename, const char* texturename) : verts_(), faces_() {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i<3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 2, "f ")) {
			std::vector<int> f;
			std::vector<int> t;
			int itrash, idx, idx1;
			iss >> trash;
			while (iss >> idx >> trash >> idx1 >> trash >> itrash) {
				idx--; // in wavefront obj all indices start at 1, not zero
				idx1--;
				f.push_back(idx);
				t.push_back(idx1);
			}
			faces_.push_back(f);
			textures_.push_back(t);
		}
		else if (!line.compare(0, 3, "vt ")) {
			std::vector<float> t_coords;
			int itrash, idx;
			float coord;

			iss >> trash;
			iss >> trash;
			for (int i = 0; i < 3; i++) {
				iss >> coord;
				t_coords.push_back(coord);
			}

			texture_coord_.push_back(t_coords);
		}
	}

	if (texturename != NULL) {
		texture_img = new TGAImage();
		texture_img->read_tga_file(texturename);
		texture_img->flip_vertically();
	}

	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {
	delete texture_img;
}

int Model::nverts() {
	return (int)verts_.size();
}

int Model::nfaces() {
	return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
	return faces_[idx];
}

Vec3f Model::vert(int i) {
	return verts_[i];
}

std::vector<int> Model::texture(int idx) {
	return textures_[idx];
}

std::vector<float> Model::texture_coord(int idx) {
	return texture_coord_[idx];
}