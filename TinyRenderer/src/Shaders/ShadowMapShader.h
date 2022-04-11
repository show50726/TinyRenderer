#include "../CustomGL.h"
#include "../Utils.h"

using namespace Renderer;

struct ShadowMapShader : IShader {
	Model* model;
	float* shadowbuffer;
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_tri;
	mat<4, 4, float> uniform_M;
	mat<4, 4, float> uniform_M_IT;
	mat<4, 4, float> uniform_Mshadow;

	ShadowMapShader(
		Model* model_,
		float* shadowbuffer,
		Matrix M,
		Matrix M_IT,
		Matrix Mshadow) : model(model_), shadowbuffer(shadowbuffer), uniform_M(M), uniform_M_IT(M_IT), uniform_Mshadow(Mshadow), varying_tri(), varying_uv() {}

	virtual Vec4f vertex(int iface, int nthvert) {
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
		gl_Vertex = ViewInfo::world_to_screen(gl_Vertex);
		varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		Vec4f sb_p = uniform_Mshadow * embed<4>(varying_tri * bar);
		sb_p = sb_p / sb_p[3];
		int idx = int(sb_p[0]) + int(sb_p[1]) * width;
		float shadow = 0.3f + 0.7f * (shadowbuffer[idx] < sb_p[2] + 43.34f);
		Vec2f uv = varying_uv * bar;
		Vec3f n = proj<3>(uniform_M_IT * embed<4>(model->normal(uv))).normalize();
		Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
		Vec3f r = (n * (n * l * 2.0f) - l).normalize();
		float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
		float diff = std::max(0.0f, n * l);
		TGAColor c = model->diffuse(uv);
		for (int i = 0; i < 3; i++) color.raw[i] = std::min<float>(20.0f + c.raw[i] * shadow * (1.2f * diff + 0.6f * spec), 255);
		return false;
	}
};