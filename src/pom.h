#ifndef POM_H
#define POM_H

#include <stdio.h>
#include <stdint.h>

class Pom
{
public:
    // TODO : Typedef uint32_t to Color
    // TODO : Typedef uint32_t* to Pixels
    // TODO : Struc Point

    static int save_to_ppm(const char *filename, const uint32_t *pixels, size_t width, size_t height);

    static void fill(uint32_t *pixels, size_t width, size_t height, uint32_t color);

    static void draw_circle(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int radius, uint32_t color);

    static void draw_line(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color);

    static void draw_rect(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color);
    static void draw_rect(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color, bool fill);

    static void fill_rect(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color);

    static void draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    static void draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color, bool fill);
    static void draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color);
    static void draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color, bool fill);

    static void draw_rotated_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, float angle, uint32_t color, bool fill);
    static void draw_rotated_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, float angle, uint32_t color);

    static void draw_ellipse(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int rx, int ry, uint32_t color);
    static void draw_rotated_ellipse(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int rx, int ry, float angle, uint32_t color);
    
    static void draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, int min, int max, int my, uint32_t color);
    static void draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, uint32_t color);

    // TODO : draw_y_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, int min, int max, int mx, uint32_t color);
    // TODO : draw_y_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, uint32_t color);

    // TODO : plot(uint32_t *pixels, size_t width, size_t height, int* xs, int* ys, uint32_t color);

private:
    static void swap(int &a, int &b);
    static void rotate_point(int &x, int &y, int &x0, int &y0, float angle);
    static bool is_in_triangle(int x, int y, int x0, int y0, int x1, int y1, int x2, int y2);
    static bool is_in_ellipse(int x, int y, int cx, int cy, int rx, int ry, float angle);
};

#endif // POM_H