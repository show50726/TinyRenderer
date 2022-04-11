#include "../CustomGL.h"
#include "../Utils.h"

using namespace Renderer;

struct DepthShader : IShader {
	Model* model;
	mat<3, 3, float> varying_tri;

	DepthShader(Model* model_) {
		model = model_;
	}

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
		gl_Vertex = ViewInfo::world_to_screen(gl_Vertex);
		varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		Vec3f p = varying_tri * bar;
		color = TGAColor(255, 255, 255) * (p.z / depth);
		return false;
	}
};