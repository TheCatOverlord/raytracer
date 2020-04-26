#include <raylib.h>
#include "rtweekend.h" // Include the header file that includes everything
#include "hittable_list.h" // header for the list of hittable objects
#include "sphere.h"        // header for spheres
#include "camera.h"
#include "hittable.h"
#include "material.h"
#include <iostream>    // iostream so we can output stuff
#include <chrono>
using namespace std::chrono;

vec3 ray_color(const ray& r, const hittable& world, int depth);

const int image_width = 1600;
const int image_height = 800;
const int samples_per_pixel = 256;
const int max_depth = 96;
hittable_list world;
camera cam;

int main() 
{
    InitWindow(image_width, image_height, "Raytracing!");

    // the center bubble
	world.add(make_shared<sphere>(
		vec3(0, 0, -1), 0.45, make_shared<dielectric>(1.5))); 
	world.add(make_shared<sphere>(
		vec3(0, 0, -1), -0.40, make_shared<dielectric>(1.4)));
	
	// the floor
	world.add(make_shared<sphere>(
		vec3(0, -100.5, -1), 100, make_shared<lambertian>(vec3(0.8, 0.8, 0.0)))); 

	world.add(make_shared<sphere>(vec3(0.5, 0, -2), 0.5, make_shared<lambertian>(vec3(1.0, 0.1, 0.1))));  // red sphere behind
	world.add(make_shared<sphere>(vec3(1, 0, -1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.3), 0.3)));    // right sphere
	world.add(make_shared<sphere>(vec3(-1, 0, -1), 0.5, make_shared<metal>(vec3(0.8, 0.8, 0.8), 0.05)));   // left sphere
	world.add(make_shared<sphere>(vec3(-0.8, 0, 0.6), 0.5, make_shared<dielectric>(1.3)));                   // hidden behind the camera
	world.add(make_shared<sphere>(vec3(0.0, 0, 0.6), 0.5, make_shared<metal>(vec3(0.0, 1.0, 0.0), 0.1))); // metal hidden

	//SetTargetFPS(60);
	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();
	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();
	auto start = high_resolution_clock::now();
	for (int j = image_height - 1; j >= 0; j--)                             
	{
		for (int f = 0; f < 2; f++)
		{
			BeginDrawing();
			for (int i = 0; i < image_width; i++)
			{
				vec3 color(0, 0, 0);
				for (int s = 0; s < samples_per_pixel; s++) {
					auto u = (i + random_double()) / image_width;
					auto v = (j + random_double()) / image_height;
					ray r = cam.get_ray(u, v);
					color += ray_color(r, world, max_depth);
				}
				vec3 pixel = color.write_color(samples_per_pixel);
				Color pixelColor;
				pixelColor.r = pixel.x();
				pixelColor.g = pixel.y();
				pixelColor.b = pixel.z();
				pixelColor.a = 255;
				DrawPixel(i, -j + image_height, pixelColor);
			}
			EndDrawing();
			if (WindowShouldClose()) {
				Image screenshot = GetScreenData();
				ExportImage(screenshot, "render.png");
				CloseWindow();
				return 0;
			}
		}
	}
	auto stop = high_resolution_clock::now();
	auto durationMS = duration_cast<milliseconds>(stop-start);
	SetWindowTitle(FormatText("%d ms", durationMS));
	Image screenshot = GetScreenData();
	ExportImage(screenshot, "render.png");
	SetTargetFPS(30);
    while (!WindowShouldClose())
    {
		BeginDrawing();
		EndDrawing();
    }
	
    CloseWindow();
    return 0;
}

vec3 ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec;

	if (depth <= 0)
		return vec3(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		vec3 attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return vec3(0, 0, 0);
	}
	
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}