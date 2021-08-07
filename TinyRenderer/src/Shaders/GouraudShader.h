#include "../CustomGL.h"
#include "../Utils.h"

using namespace Renderer;

struct GoraudShader : IShader {
	Model* model;
	mat<2, 3, float> varying_uv;
	mat<4, 4, float> uniform_MVP;
	mat<4, 4, float> uniform_MVP_IT;  

	GoraudShader(Model* model_) {
		model = model_;
	}

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f vert = embed<4>(model->vert(iface, nthvert));
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));

		return ViewInfo::world_to_screen(vert);
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		Vec2f uv = varying_uv * bar;
		Vec3f n = proj<3>(uniform_MVP_IT*embed<4>(model->normal(uv))).normalize();
		Vec3f l = proj<3>(uniform_MVP*embed<4>(light_dir)).normalize();
		Vec3f r = (n*(n*l*2.0f) - l).normalize();
		float intensity = r * Vec3f(0, 0, 1);   // View vector in screen coordinate is (0, 0, 1)
		float spec = pow(max(intensity, 0.0f), model->specular(uv));
		//std::cout << spec << std::endl;
		float diff = max(0.0f, n*l);
		TGAColor c = model->diffuse(uv);
		color = c;

		for (int i = 0; i < 3; i++) {
			color.raw[i] = static_cast<unsigned char>(min(10 + c.raw[i] * (1.2 * diff + 8.0 * spec), 255.0));
		}
		return false;
	}
};