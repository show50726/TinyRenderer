#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "Utils.h"
#include "Shaders/GouraudShader.h"
#include "Shaders/DepthShader.h"
#include "Shaders/ShadowMapShader.h"

Model *model = NULL;
using namespace Renderer;

int main()
{
	float* zbuffer = new float[width * height];
	float* shadowbuffer = new float[width * height];
	
	for (int i = height * width; --i;) {
		zbuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();
	}
	
	model = new Model("obj/african_head.obj");

	/*Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

	DrawUtils::triangle_by_bounding_box_check(t0, image, RED);
	DrawUtils::triangle_by_bounding_box_check(t1, image, WHITE);
	DrawUtils::triangle_by_bounding_box_check(t2, image, GREEN);*/

	Vec3f eye(1, 1, 4);
	Vec3f center(0, 0, 0);
	Vec3f up(0, 1, 0);

	{
		TGAImage image(width, height, TGAImage::RGB);
		ViewInfo::look_at(light_dir, center, up);
		ViewInfo::view_port(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
		ViewInfo::projection(0);
	
		DepthShader shader(model);
		//Matrix mvp = ViewInfo::get_MVP();
		//shader.uniform_MVP = mvp;
		//shader.uniform_MVP_IT = mvp.invert_transpose();

		DrawUtils::model(model, image, &shader, shadowbuffer);

		image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
		image.write_tga_file("output.tga");
	}

	Matrix M = ViewInfo::get_MVP();

	{
		TGAImage frame(width, height, TGAImage::RGB);
		ViewInfo::look_at(eye, center, up);
		ViewInfo::view_port(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
		ViewInfo::projection(-1.f / (eye - center).norm());

		Matrix mvp = ViewInfo::get_MVP();
		ShadowMapShader shader(model, shadowbuffer, ViewInfo::ViewMatrix, (ViewInfo::ProjectionMatrix * ViewInfo::ViewMatrix).invert_transpose(), M * mvp.invert());
		DrawUtils::model(model, frame, &shader, zbuffer);
		frame.flip_vertically(); // to place the origin in the bottom left corner of the image
		frame.write_tga_file("framebuffer.tga");
	}

	delete model;
	system("pause");
	return 0;
}

