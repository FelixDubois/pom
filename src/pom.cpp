#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <cmath>
#include <png.h>

#define TICK_SIZE 5
#define AXIS_MARGIN 3

void pom_swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

void pom_rotate_point(int &x, int &y, int &x0, int &y0, float angle)
{
    int x1 = x - x0;
    int y1 = y - y0;
    x = x0 + x1 * cos(angle) - y1 * sin(angle);
    y = y0 + x1 * sin(angle) + y1 * cos(angle);
}

// https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
bool pom_is_in_triangle(int x, int y, int x0, int y0, int x1, int y1, int x2, int y2)
{
    int a = (x1 - x0) * (y - y0) - (x - x0) * (y1 - y0);
    int b = (x2 - x1) * (y - y1) - (x - x1) * (y2 - y1);
    int c = (x0 - x2) * (y - y2) - (x - x2) * (y0 - y2);
    return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0);
}

// https://math.stackexchange.com/a/76463
// https://stackoverflow.com/a/16814494/13529134
bool pom_is_in_ellipse(int x, int y, int x0, int y0, int a, int b, float angle)
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
int pom_save_to_ppm(const char *filename, const uint32_t* pixels, size_t width, size_t height){

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

// save the pixels to a image file (png) 
void  pom_save_to_png( const char *filename, const uint32_t *pixels, size_t width, size_t height) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file %s for writing", filename);
        return ;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fclose(fp);
        fprintf(stderr, "Error: could not create png write struct");
        return ;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        fprintf(stderr, "Error: could not create png info struct");
        return ;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fprintf(stderr, "Error: could not set png jmpbuf");
        return ;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    png_bytep row = (png_bytep) malloc(3 * width * sizeof(png_byte));

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            row[j * 3 + 0] = (pixels[i * width + j] >> 16) & 0xFF;
            row[j * 3 + 1] = (pixels[i * width + j] >> 8) & 0xFF;
            row[j * 3 + 2] = pixels[i * width + j] & 0xFF;
        }
        png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, NULL);

    free(row);
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
}

void pom_fill(uint32_t* pixels, size_t width, size_t height, uint32_t color){
    for (size_t i = 0; i < width * height; i++) {
        pixels[i] = color;
    }
}

void pom_draw_circle(uint32_t* pixels, size_t width, size_t height, int cx, int cy, int radius, uint32_t color){
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
void pom_draw_line(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true) {
        if(0 <= x0 && x0 < (int)width && 0 <= y0 && y0 < (int)height){
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

void pom_draw_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    pom_draw_line(pixels, width, height, x0, y0, x1, y0, color);
    pom_draw_line(pixels, width, height, x1, y0, x1, y1, color);
    pom_draw_line(pixels, width, height, x1, y1, x0, y1, color);
    pom_draw_line(pixels, width, height, x0, y1, x0, y0, color);
}

void pom_draw_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color, bool fill){

    if(!fill){
        pom_draw_rect(pixels, width, height, x0, y0, x1, y1, color);
        return;
    }

    if(x0 > x1) pom_swap(x0, x1);
    if(y0 > y1) pom_swap(y0, y1);

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

void pom_fill_rect(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color){
    pom_draw_rect(pixels, width, height, x0, y0, x1, y1, color, true);
}

void pom_draw_triangle(uint32_t* pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
    pom_draw_line(pixels, width, height, x0, y0, x1, y1, color);
    pom_draw_line(pixels, width, height, x1, y1, x2, y2, color);
    pom_draw_line(pixels, width, height, x2, y2, x0, y0, color);
}

void pom_draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color, bool fill){
    if(!fill){
        pom_draw_triangle(pixels, width, height, x0, y0, x1, y1, x2, y2, color);
    }else{
        int minx = std::min(x0, std::min(x1, x2));
        int maxx = std::max(x0, std::max(x1, x2));
        int miny = std::min(y0, std::min(y1, y2));
        int maxy = std::max(y0, std::max(y1, y2));

        for (int y = miny; y <= maxy; y++) {
            if(0 <= y && y < (int)height){
                for (int x = minx; x <= maxx; x++) {
                    if(0 <= x && x < (int)width){
                        if(pom_is_in_triangle(x, y, x0, y0, x1, y1, x2, y2)){
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
void pom_draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color){
    int x1 = x0;
    int y1 = y0 - 2 * size / 3;

    int x2 = x0 + size / 2;
    int y2 = y0 + size / 3;

    int x3 = x0 - size / 2;
    int y3 = y2;

    pom_draw_triangle(pixels, width, height, x1, y1, x2, y2, x3, y3, color);
}

// Use the midpoint circle algorithm : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void pom_draw_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, uint32_t color, bool fill){

    if(!fill){
        pom_draw_triangle(pixels, width, height, x0, y0, size, color);
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
                    if(pom_is_in_triangle(x, y, x1, y1, x2, y2, x3, y3)){
                        pixels[x + y * width] = color;
                    }
                }
            }
        }
    }
}

void pom_draw_rotated_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, float angle, uint32_t color, bool fill){
    // Convert angle to radians
    angle = - angle * M_PI / 180.0;
    
    int x1 = x0;
    int y1 = y0 - 2 * size / 3;

    int x2 = x0 + size / 2;
    int y2 = y0 + size / 3;

    int x3 = x0 - size / 2;
    int y3 = y2;

    // Rotate the 3 points
    pom_rotate_point(x1, y1, x0, y0, angle);
    pom_rotate_point(x2, y2, x0, y0, angle);
    pom_rotate_point(x3, y3, x0, y0, angle);

    pom_draw_triangle(pixels, width, height, x1, y1, x2, y2, x3, y3, color, fill);
}

void pom_draw_rotated_triangle(uint32_t *pixels, size_t width, size_t height, int x0, int y0, size_t size, float angle, uint32_t color){
    pom_draw_rotated_triangle(pixels, width, height, x0, y0, size, angle, color, false);    
}

// Ellipse wikipedia : https://en.wikipedia.org/wiki/Ellipse
// french version : https://fr.wikipedia.org/wiki/Ellipse_(math%C3%A9matiques)
void pom_draw_ellipse(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int rx, int ry, uint32_t color){
    for(int y = -ry; y <= ry; y++){
        for(int x = -rx; x <= rx; x++){
            int px = cx + x;
            int py = cy + y;
            if(0 <= px && px < (int)width && 0 <= py && py < (int)height){
                if(pom_is_in_ellipse(x, y, 0, 0, rx, ry, 0)){
                    pixels[px + py * width] = color;
                }
            }
        }
    }
}

void pom_draw_rotated_ellipse(uint32_t *pixels, size_t width, size_t height, int cx, int cy, int rx, int ry, float angle, uint32_t color){
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
                    if(pom_is_in_ellipse(x, y, cx, cy, rx, ry, angle)){
                        pixels[x + y * width] = color;
                    }
                }
            }
        }
    }
}

void pom_draw_grid(uint32_t *pixels, size_t width, size_t height, int nb_rows, int nb_cols, uint32_t color){
    for(int i = 1; i <= nb_rows; i++){
        pom_draw_line(pixels, width, height, 0, i * height / (nb_rows + 1), width, i * height / (nb_rows + 1), color);
    }

    for(int i = 1; i <= nb_cols; i++){
        pom_draw_line(pixels, width, height, i * width / (nb_cols + 1 ), 0, i * width / (nb_cols + 1 ), height, color);
    }
}

void pom_draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, size_t tick_height, int min, int max, int my, uint32_t color){
    int y = std::min(height - my, height - tick_height);

    pom_draw_line(pixels, width, height, min, y, max, y, color);

    int delta = max - min;

    for(int i = 1; i <= ticks_count; i++){
        pom_draw_line(pixels, width, height, min + i * delta / (ticks_count + 1), y, min + i * delta / (ticks_count + 1), y - tick_height, color);
    }
}

void pom_draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, int min, int max, int my, uint32_t color){
    pom_draw_x_axis(pixels, width, height, ticks_count, TICK_SIZE, min, max, my, color);
}

void pom_draw_x_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, uint32_t color){
    pom_draw_x_axis(pixels, width, height, ticks_count, TICK_SIZE, 0, width, AXIS_MARGIN, color);
}

void pom_draw_y_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, size_t tick_width, int min, int max, int mx, uint32_t color){
    int x = mx;

    pom_draw_line(pixels, width, height, x, min, x, max, color);

    int delta = max - min;

    for(int i = 1; i <= ticks_count; i++){
        pom_draw_line(pixels, width, height, x, min + i * delta / (ticks_count + 1), x + tick_width, min + i * delta / (ticks_count + 1), color);
    }
}

void pom_draw_y_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, int min, int max, int mx, uint32_t color){
    pom_draw_y_axis(pixels, width, height, ticks_count, TICK_SIZE, min, max, mx, color);
}

void pom_draw_y_axis(uint32_t *pixels, size_t width, size_t height, int ticks_count, uint32_t color){
    pom_draw_y_axis(pixels, width, height, ticks_count, TICK_SIZE, 0, height, AXIS_MARGIN, color);
}

void pom_draw_axes(uint32_t *pixels, size_t width, size_t height, int ticks_count_x, int ticks_count_y, uint32_t color){
    pom_draw_x_axis(pixels, width, height, ticks_count_x, color);
    pom_draw_y_axis(pixels, width, height, ticks_count_y, color);
}

void pom_draw_axes(uint32_t *pixels, size_t width, size_t height, int ticks_count, uint32_t color){
    pom_draw_axes(pixels, width, height, ticks_count, ticks_count, color);
}

void pom_draw_polygon(uint32_t *pixels, size_t width, size_t height, int *pointsX, int *pointsY, int points_count, uint32_t color){
    for(int i = 0; i < points_count; i++){
        pom_draw_line(pixels, width, height, pointsX[i], pointsY[i], pointsX[(i + 1) % points_count], pointsY[(i + 1) % points_count], color);
    }
}