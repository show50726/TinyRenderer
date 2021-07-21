#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "Utils.h"

Model *model = NULL;
const int width = 800;
const int height = 800;

int main()
{
	TGAImage image(width, height, TGAImage::RGB);
	model = new Model("obj/african_head.obj", "african_head_diffuse.tga");

	/*Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

	DrawUtils::triangle_by_bounding_box_check(t0, image, RED);
	DrawUtils::triangle_by_bounding_box_check(t1, image, WHITE);
	DrawUtils::triangle_by_bounding_box_check(t2, image, GREEN);*/
	Vec3f light_dir(0, 0, -1);
	DrawUtils::model(model, light_dir, image);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

