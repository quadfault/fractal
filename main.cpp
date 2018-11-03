// main.cpp - Fractals.
// Written by quadfault
// 9/14/18

#include <complex>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "coord-generator.h"

static constexpr auto aspect_ratio = 16.0 / 9.0;
static constexpr auto bound_squared = 4.0;
static constexpr auto max_escape_time = 255;
static constexpr auto image_width = 1366.0;
static constexpr auto image_height = image_width / aspect_ratio;
static constexpr auto complex_frame_width = 4.0;
static constexpr auto complex_frame_height = complex_frame_width / aspect_ratio;
static constexpr auto complex_frame_origin = std::complex<double> {};

static std::complex<double>
complex_point_from_pixel_at(double x, double y) {
    // Map the image plane to the complex frame, so that the center of the
    // image is at the origin of the complex frame.

    auto scale_x = complex_frame_width / image_width;
    auto scale_y = complex_frame_height / image_height;

    std::complex<double> complex_point(
        (x - (image_width / 2.0)) * scale_x,
       -(y - (image_height / 2.0)) * scale_y
    );

    // Now translate the complex frame into position on the complex plane.
    complex_point += complex_frame_origin;

    return complex_point;
}

static bool has_escaped(std::complex<double> z) {
    return norm(z) > bound_squared;
}

static int escape_time(std::complex<double> c) {
    std::complex<double> z;

    int iterations;
    for (iterations = 0; iterations < max_escape_time; ++iterations) {
        z = z * z + c;

        if (has_escaped(z)) {
            break;
        }
    }

    return iterations;
}

static void generate_fractal_worker(coord_generator* p_cg,
                                    unsigned char* p_image)
{
    for (;;) {
        auto xy_maybe = p_cg->next();
        if (!xy_maybe) {
            break;
        }

        auto [x, y] = *xy_maybe;
        auto c = complex_point_from_pixel_at(static_cast<double>(x),
                                             static_cast<double>(y));

        double scale = 255.0 / static_cast<double>(max_escape_time);
        double color_scaled =
            static_cast<double>(max_escape_time - escape_time(c))
            * scale;
        unsigned char color = static_cast<unsigned char>(color_scaled);
        p_image[static_cast<std::size_t>(y * image_width + x)] = color;
    }
}

static unsigned char* generate_fractal() {
    auto image_size = static_cast<std::size_t>(image_width * image_height);
    unsigned char* image { new unsigned char[image_size] };
    coord_generator cg(static_cast<std::size_t>(image_width),
                       static_cast<std::size_t>(image_height));

    std::thread t1(generate_fractal_worker, &cg, image);
    std::thread t2(generate_fractal_worker, &cg, image);
    std::thread t3(generate_fractal_worker, &cg, image);
    std::thread t4(generate_fractal_worker, &cg, image);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return image;
}

int main() {
    std::cout << "Generating fractal..." << std::endl;

    auto image = generate_fractal();

    std::cout << "Writing image to disk..." << std::endl;

    std::ofstream output("fractal.pgm");
    output << "P2\n"
           << static_cast<std::size_t>(image_width) << " "
           << static_cast<std::size_t>(image_height) << "\n"
           << "255\n";

    auto image_size = static_cast<std::size_t>(image_width * image_height);
    for (std::size_t i { 0 }; i < image_size; ++i) {
        output << static_cast<unsigned int>(image[i]) << "\n";
    }

    delete[] image;
}
