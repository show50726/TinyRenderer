#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), norms_(), uv_coord_(), texture_img(), normal_img(), specular_img() {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	
	if (in.fail()) {
		std::cerr << "Loading file " << filename << " failed!" << std::endl;
		return;
	}

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
			std::vector<Vec3i> f;
			Vec3i tmp;
			iss >> trash;
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
				tmp[0]--;
				tmp[1]--;
				tmp[2]--;
				f.push_back(tmp);
			}
			faces_.push_back(f);
		}
		else if (!line.compare(0, 3, "vt ")) {
			Vec2f uv;
			iss >> trash >> trash;
			for (int i = 0; i < 2; i++) {
				iss >> uv[i];
			}

			uv_coord_.push_back(uv);
		}
		else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			Vec3f n;
			for (int i = 0; i < 3; i++) {
				iss >> n[i];
			}
			norms_.push_back(n);
		}
	}

	texture_img = new TGAImage();
	normal_img = new TGAImage();
	specular_img = new TGAImage();

	load_texture(filename, "_diffuse.tga", texture_img);
	load_texture(filename, "_nm.tga", normal_img);
	load_texture(filename, "_spec.tga", specular_img);

	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

void Model::load_texture(std::string filename, const char* suffix, TGAImage* img) {
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		bool ok = img->read_tga_file(texfile.c_str());

		if (!ok) {
			std::cerr << "Loading file " << texfile << " failed!" << std::endl;
			return;
		}

		img->flip_vertically();
	}
}

Model::~Model() {
	delete texture_img;
	delete normal_img;
	delete specular_img;
}

int Model::nverts() {
	return (int)verts_.size();
}

int Model::nfaces() {
	return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
	std::vector<int> face;
	for (int i = 0; i < faces_[idx].size(); i++) {
		face.push_back(faces_[idx][i][0]);
	}
	return face;
}

Vec3f Model::vert(int i) {
	return verts_[i];
}

Vec3f Model::vert(int iface, int nthvert) {
	return verts_[faces_[iface][nthvert][0]];
}

Vec3f Model::normal(int iface, int nthvert) {
	int idx = faces_[iface][nthvert][2];
	return norms_[idx].normalize();
}

Vec3f Model::normal(Vec2f uv) {
	Vec2i uv_(uv[0] * normal_img->get_width(), uv[1] * normal_img->get_height());
	TGAColor c = normal_img->get(uv_[0], uv_[1]);
	Vec3f res;
	for (int i = 0; i < 3; i++) {
		res[2 - i] = (float) c.raw[i] / 255.0f*2.0f - 1.0f;
	}
	return res;
}

Vec2f Model::uv(int iface, int nthvert) {
	return uv_coord_[faces_[iface][nthvert][1]];
}

TGAColor Model::diffuse(Vec2f uv) {
	Vec2i uv_(uv[0] * texture_img->get_width(), uv[1] * texture_img->get_height());
	return texture_img->get(uv_[0], uv_[1]);
}

float Model::specular(Vec2f uv) {
	Vec2i uv_(uv[0] * texture_img->get_width(), uv[1] * texture_img->get_height());
	return specular_img->get(uv_[0], uv_[1]).r / 1.0f;
}