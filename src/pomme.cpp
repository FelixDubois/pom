#include "pomme.h"
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string>
#include <stdlib.h>
#include <algorithm>

void Pomme::swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

// Wikipedia: https://en.wikipedia.org/wiki/Netpbm_format
int Pomme::save_to_ppm(const char *filename, const uint32_t* pixels, size_t width, size_t height){

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

void Pomme::fill(uint32_t* pixels, size_t width, size_t height, uint32_t color){
    for (size_t i = 0; i < width * height; i++) {
        pixels[i] = color;
    }
}

void Pomme::draw_circle(uint32_t* pixels, size_t width, size_t height, int cx, int cy, int radius, uint32_t color){
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
void Pomme::draw_line(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
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

void Pomme::draw_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    draw_line(pixels, width, height, x0, y0, x1, y0, color);
    draw_line(pixels, width, height, x1, y0, x1, y1, color);
    draw_line(pixels, width, height, x1, y1, x0, y1, color);
    draw_line(pixels, width, height, x0, y1, x0, y0, color);
}

void Pomme::draw_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color, bool fill){

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

void Pomme::fill_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    draw_rect(pixels, width, height, x0, y0, x1, y1, color, true);
}

void Pomme::draw_triangle(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
    draw_line(pixels, width, height, x0, y0, x1, y1, color);
    draw_line(pixels, width, height, x1, y1, x2, y2, color);
    draw_line(pixels, width, height, x2, y2, x0, y0, color);
}

// Barycenter : https://en.wikipedia.org/wiki/Barycenter
// French version (more maths) : https://fr.wikipedia.org/wiki/Barycentre
void Pomme::draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color){
    int x1 = x0;
    int y1 = y0 - 2 * size / 3;

    int x2 = x0 + size / 2;
    int y2 = y0 + size / 3;

    int x3 = x0 - size / 2;
    int y3 = y2;

    draw_triangle(pixels, width, height, x1, y1, x2, y2, x3, y3, color);
}

// Use the midpoint circle algorithm : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void Pomme::draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color, bool fill){

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
                    int dx1 = x2 - x1;
                    int dy1 = y2 - y1;
                    int dx2 = x3 - x1;
                    int dy2 = y3 - y1;
                    int dx3 = x - x1;
                    int dy3 = y - y1;

                    // q: what is the purpose of this?
                    // a: it's a way to check if a point is inside a triangle
                    //    it's a bit like the determinant of a matrix
                    //    if the point is inside the triangle, the determinant is positive
                    //    if the point is outside the triangle, the determinant is negative
                    //    if the point is on the edge of the triangle, the determinant is 0
                    int a = dx1 * dy2 - dx2 * dy1;
                    int a1 = dx3 * dy2 - dx2 * dy3;
                    int a2 = dx1 * dy3 - dx3 * dy1;

                    if(a >= 0){
                        // If the point is on the same side of the 2 edges, it is inside the triangle
                        if(a1 >= 0 && a2 >= 0 && a1 + a2 <= a){
                            pixels[x + y * width] = color;
                        }
                    }else{
                        // If the point is on the same side of the 2 edges, it is inside the triangle
                        if(a1 <= 0 && a2 <= 0 && a1 + a2 >= a){
                            pixels[x + y * width] = color;
                        }
                    }
                }
            }
        }
    }
}

// Ellipse wikipedia : https://en.wikipedia.org/wiki/Ellipse
// french version : https://fr.wikipedia.org/wiki/Ellipse_(math%C3%A9matiques)
void Pomme::draw_ellipse(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int rx, int ry, uint32_t color){
    for(int y = -ry; y <= ry; y++){
        for(int x = -rx; x <= rx; x++){
            int border = rx*rx*ry*ry;
            int inside = rx*rx*y*y + ry*ry*x*x;
            if(inside <= border){
                int px = cx + x;
                int py = cy + y;
                if(0 < px && px < (int)width && 0 < py && py < (int)height){
                    pixels[px + py * width] = color;
                }
            }
        }
    }
}

void Pomme::draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, int min, int max, int my, uint32_t color){
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

void Pomme::draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, uint32_t color){
    draw_x_axis(pixels, width, height, ticks_count, 0, width, 3, color);
}

