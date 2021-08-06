#include "../CustomGL.h"
#include "../Utils.h"

using namespace Renderer;

struct GoraudShader : IShader {
	Model* model;
	Vec3f varying_intensity;
	mat<2, 3, float> varying_uv;

	GoraudShader(Model* model_) {
		model = model_;
	}

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f vert = embed<4>(model->vert(iface, nthvert));
		Vec3f lightDir = light_dir;
		lightDir.normalize();
		varying_intensity[nthvert] = max(0.0f, model->normal(iface, nthvert) * lightDir);
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));

		return ViewInfo::world_to_screen(vert);
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		float intensity = varying_intensity * bar;
		Vec2f uv = varying_uv * bar;
		color = model->diffuse(uv) * intensity;
		return false;
	}
};