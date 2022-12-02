#include "pom.h"
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <cmath>

void Pom::swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

void Pom::rotate_point(int &x, int &y, int &x0, int &y0, float angle)
{
    int x1 = x - x0;
    int y1 = y - y0;
    x = x0 + x1 * cos(angle) - y1 * sin(angle);
    y = y0 + x1 * sin(angle) + y1 * cos(angle);
}

// https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
bool Pom::is_in_triangle(int x, int y, int x0, int y0, int x1, int y1, int x2, int y2)
{
    int a = (x1 - x0) * (y - y0) - (x - x0) * (y1 - y0);
    int b = (x2 - x1) * (y - y1) - (x - x1) * (y2 - y1);
    int c = (x0 - x2) * (y - y2) - (x - x2) * (y0 - y2);
    return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0);
}

// https://math.stackexchange.com/a/76463
// https://stackoverflow.com/a/16814494/13529134
bool Pom::is_in_ellipse(int x, int y, int x0, int y0, int a, int b, float angle)
{
    // Check if a point is in an ellipse with center (x0, y0), semi-major axis a, semi-minor axis b, and angle of rotation angle
    // https://en.wikipedia.org/wiki/Ellipse#Equation_of_a_general_ellipse

    // Rotate the point
    int k = cos(angle) * (x - x0) + sin(angle) * (y - y0);
    int l = sin(angle) * (x - x0) - cos(angle) * (y - y0);

    // Check if the point is in the ellipse
    return (k * b) * (k * b) + (l * a) * (l * a) <= a*a*b*b;
}

// Wikipedia: https://en.wikipedia.org/wiki/Netpbm_format
int Pom::save_to_ppm(const char *filename, const uint32_t* pixels, size_t width, size_t height){

    FILE* f = fopen(filename, "wb");
    if(f == NULL){
        fclose(f);
        fprintf(stderr, "Error: could not open file %s for writing", filename);
        return -1;
    }

    fprintf(f, "P3\n%zu %zu\n255\n", width, height);
    if(ferror(f)){
        fclose(f);
        fprintf(stderr, "Error: could not write to file %s because of error : %d", filename, errno);
        return -1;
    }

    for (size_t i = 0; i < width * height; i++) {
        // Write the color components in the order red, green, blue to the ppm file in ASCII format.
        fprintf(f, "%d %d %d ", (pixels[i] >> 16) & 0xFF, (pixels[i] >> 8) & 0xFF, pixels[i] & 0xFF);

        if(ferror(f)){
            fclose(f);
            fprintf(stderr, "Error: could not write to file %s because of error : %d", filename, errno);
            return -1;
        }
    }

    fclose(f);

    return 0;
}

void Pom::fill(uint32_t* pixels, size_t width, size_t height, uint32_t color){
    for (size_t i = 0; i < width * height; i++) {
        pixels[i] = color;
    }
}

void Pom::draw_circle(uint32_t* pixels, size_t width, size_t height, int cx, int cy, int radius, uint32_t color){
    int x0 = cx - radius;
    int x1 = cx + radius;
    int y0 = cy - radius;
    int y1 = cy + radius;
    for (int y = y0; y < y1; y++) {
        if(0 < y && y < (int)height){
            for (int x = x0; x < x1; x++) {
                if(0 < x && x < (int)width){
                    int dx = cx - x;
                    int dy = cy - y;

                    if(dx * dx + dy * dy <= radius * radius){
                        pixels[x + y * width] = color;
                    }
                }
            }
        }
    }
}

// Bresenham's line algorithm : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void Pom::draw_line(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true) {
        if(0 < x0 && x0 < (int)width && 0 < y0 && y0 < (int)height){
            pixels[x0 + y0 * width] = color;
        }

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Pom::draw_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    draw_line(pixels, width, height, x0, y0, x1, y0, color);
    draw_line(pixels, width, height, x1, y0, x1, y1, color);
    draw_line(pixels, width, height, x1, y1, x0, y1, color);
    draw_line(pixels, width, height, x0, y1, x0, y0, color);
}

void Pom::draw_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color, bool fill){

    if(!fill){
        draw_rect(pixels, width, height, x0, y0, x1, y1, color);
        return;
    }

    if(x0 > x1) swap(x0, x1);
    if(y0 > y1) swap(y0, y1);

    for (int y = y0; y < y1; y++) {
        if(0 < y && y < (int)height){
            for (int x = x0; x < x1; x++) {
                if(0 < x && x < (int)width){
                    pixels[x + y * width] = color;
                }
            }
        }
    }
}

void Pom::fill_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    draw_rect(pixels, width, height, x0, y0, x1, y1, color, true);
}

void Pom::draw_triangle(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
    draw_line(pixels, width, height, x0, y0, x1, y1, color);
    draw_line(pixels, width, height, x1, y1, x2, y2, color);
    draw_line(pixels, width, height, x2, y2, x0, y0, color);
}

void Pom::draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color, bool fill){
    if(!fill){
        draw_triangle(pixels, width, height, x0, y0, x1, y1, x2, y2, color);
    }else{
        int minx = std::min(x0, std::min(x1, x2));
        int maxx = std::max(x0, std::max(x1, x2));
        int miny = std::min(y0, std::min(y1, y2));
        int maxy = std::max(y0, std::max(y1, y2));

        for (int y = miny; y <= maxy; y++) {
            if(0 <= y && y < (int)height){
                for (int x = minx; x <= maxx; x++) {
                    if(0 <= x && x < (int)width){
                        if(is_in_triangle(x, y, x0, y0, x1, y1, x2, y2)){
                            pixels[x + y * width] = color;
                        }
                    }
                }
            }
        }
    }
}

// Barycenter : https://en.wikipedia.org/wiki/Barycenter
// French version (more maths) : https://fr.wikipedia.org/wiki/Barycentre
void Pom::draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color){
    int x1 = x0;
    int y1 = y0 - 2 * size / 3;

    int x2 = x0 + size / 2;
    int y2 = y0 + size / 3;

    int x3 = x0 - size / 2;
    int y3 = y2;

    draw_triangle(pixels, width, height, x1, y1, x2, y2, x3, y3, color);
}

// Use the midpoint circle algorithm : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void Pom::draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color, bool fill){

    if(!fill){
        draw_triangle(pixels, width, height, x0, y0, size, color);
        return;
    }

    // Calculate the 3 edge points of the triangle
    /*    
                (x1, y1)
                   /\
                  /  \
                 /  + \ (x0,y0) 
                /______\    
        (x3, y3)         (x2, y2)   
    */

    int x1 = x0;
    int y1 = y0 - 2 * size / 3;

    int x2 = x0 + size / 2;
    int y2 = y0 + size / 3;

    int x3 = x0 - size / 2;
    int y3 = y2;


    // Calculate the extremities of the triangle
    int min_x = std::min(x1, std::min(x2, x3));
    int max_x = std::max(x1, std::max(x2, x3));
    int min_y = std::min(y1, std::min(y2, y3));
    int max_y = std::max(y1, std::max(y2, y3));

    for (int y = min_y; y < max_y; y++) {
        if(0 < y && y < (int)height){
            for (int x = min_x; x < max_x; x++) {
                if(0 < x && x < (int)width){
                    if(is_in_triangle(x, y, x1, y1, x2, y2, x3, y3)){
                        pixels[x + y * width] = color;
                    }
                }
            }
        }
    }
}

void Pom::draw_rotated_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, float angle, uint32_t color, bool fill){
    // Convert angle to radians
    angle = - angle * M_PI / 180.0;
    
    int x1 = x0;
    int y1 = y0 - 2 * size / 3;

    int x2 = x0 + size / 2;
    int y2 = y0 + size / 3;

    int x3 = x0 - size / 2;
    int y3 = y2;

    // Rotate the 3 points
    rotate_point(x1, y1, x0, y0, angle);
    rotate_point(x2, y2, x0, y0, angle);
    rotate_point(x3, y3, x0, y0, angle);

    draw_triangle(pixels, width, height, x1, y1, x2, y2, x3, y3, color, fill);
}

void Pom::draw_rotated_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, float angle, uint32_t color){
    draw_rotated_triangle(pixels, width, height, x0, y0, size, angle, color, false);    
}

// Ellipse wikipedia : https://en.wikipedia.org/wiki/Ellipse
// french version : https://fr.wikipedia.org/wiki/Ellipse_(math%C3%A9matiques)
void Pom::draw_ellipse(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int rx, int ry, uint32_t color){
    for(int y = -ry; y <= ry; y++){
        for(int x = -rx; x <= rx; x++){
            int px = cx + x;
            int py = cy + y;
            if(0 <= px && px < (int)width && 0 <= py && py < (int)height){
                if(is_in_ellipse(x, y, 0, 0, rx, ry, 0)){
                    pixels[px + py * width] = color;
                }
            }
        }
    }
}

void Pom::draw_rotated_ellipse(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int rx, int ry, float angle, uint32_t color){
    // Convert the angle to radians
    angle = - angle * M_PI / 180.0;
    
    // Get the extremities of the ellipse with the angle
    int min_x = cx - std::max(rx, ry);
    int max_x = cx + std::max(rx, ry);
    int min_y = cy - std::max(rx, ry);
    int max_y = cy + std::max(rx, ry);
    
    for(int y = min_y; y <= max_y; y++){
        if(0 <= y && y < (int)height){
            for(int x = min_x; x <= max_x; x++){
                if(0 <= x && x < (int)width){
                    if(is_in_ellipse(x, y, cx, cy, rx, ry, angle)){
                        pixels[x + y * width] = color;
                    }
                }
            }
        }
    }
}

void Pom::draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, int min, int max, int my, uint32_t color){
    int y = std::min(height - my, height - 3);

    for(int x = min; x < max; x++){
        pixels[x + y * width] = color;

        if(x % (width / ticks_count) == 0){
            for(int i = 0; i < 3; i++){
                pixels[x + (y - i) * width] = color;
            }
        }
    }
}

void Pom::draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, uint32_t color){
    draw_x_axis(pixels, width, height, ticks_count, 0, width, 3, color);
}